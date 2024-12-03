#pragma once
#include "MetaDataModels.h"
#include <tuple>
#include <string_view>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cstdint>

class DataSerializer {
public:
    static constexpr const char* DELIMITER = "--DELIMITER--";

    // Helper function to safely convert string to float
    static float safe_stof(const std::string& str, float default_value = 0.0f) {
        if (str.empty()) return default_value;
        try {
            return std::stof(str);
        } catch (...) {
            return default_value;
        }
    }

    // Helper function to safely convert string to int
    static int safe_stoi(const std::string& str, int default_value = 0) {
        if (str.empty()) return default_value;
        try {
            return std::stoi(str);
        } catch (...) {
            return default_value;
        }
    }

    std::tuple<std::unique_ptr<MetadataModels::BaseMetadata>, std::vector<uint8_t>> 
    decode(const std::vector<uint8_t>& data) {
        // Convert binary data to string for parsing
        std::string str_data(data.begin(), data.end());
        auto parts = split_data(str_data);
        
        if (parts.empty()) {
            throw std::runtime_error("Empty data received");
        }

        // First part is the type
        std::string type(parts[0]);
        auto metadata = MetadataModels::create_metadata(type);

        if (type == "timeline") {
            auto* timeline = dynamic_cast<MetadataModels::TimelineMetadata*>(metadata.get());
            if (timeline) {
                populate_base_metadata(*timeline, parts);
                populate_timeline_metadata(*timeline, parts);
            }
        } else if (type == "audio") {
            auto* audio = dynamic_cast<MetadataModels::AudioMetadata*>(metadata.get());
            if (audio) {
                populate_base_metadata(*audio, parts);
                populate_audio_metadata(*audio, parts);
            }
        } else {
            populate_base_metadata(*metadata, parts);
        }

        // Last part is the payload
        std::vector<uint8_t> payload;
        if (parts.size() > 1) {
            auto payload_view = parts.back();
            payload.assign(
                reinterpret_cast<const uint8_t*>(payload_view.data()),
                reinterpret_cast<const uint8_t*>(payload_view.data() + payload_view.size())
            );
        }
        
        return {std::move(metadata), payload};
    }

    std::vector<uint8_t> encode(const MetadataModels::BaseMetadata& metadata,
                               const std::vector<uint8_t>& payload) {
        std::string result;

        // Add base metadata
        result += metadata.type + DELIMITER;
        result += metadata.chunk_id + DELIMITER;
        result += std::to_string(metadata.chunk_index) + DELIMITER;
        result += metadata.server_hash + DELIMITER;
        result += metadata.client_hash + DELIMITER;
        result += std::to_string(metadata.size_kb) + DELIMITER;
        result += metadata.timestamp + DELIMITER;
        result += metadata.sequence_id + DELIMITER;
        result += metadata.version + DELIMITER;
        result += std::to_string(metadata.retry_count) + DELIMITER;
        result += std::to_string(metadata.processing_time_ms) + DELIMITER;

        // Add specific metadata based on type
        if (const auto* timeline = dynamic_cast<const MetadataModels::TimelineMetadata*>(&metadata)) {
            result += std::to_string(timeline->start_time) + DELIMITER;
            result += std::to_string(timeline->end_time) + DELIMITER;
            result += "[]" + std::string(DELIMITER);  // markers
            result += "[]" + std::string(DELIMITER);  // events
            result += "[]" + std::string(DELIMITER);  // dependencies
            result += std::string(timeline->is_keyframe ? "True" : "False") + DELIMITER;
        }
        else if (const auto* audio = dynamic_cast<const MetadataModels::AudioMetadata*>(&metadata)) {
            result += audio->format + DELIMITER;
            result += std::to_string(audio->sample_rate) + DELIMITER;
            result += std::to_string(audio->channels) + DELIMITER;
            result += std::to_string(audio->duration_ms) + DELIMITER;
            result += std::string(audio->is_final ? "True" : "False") + DELIMITER;
            result += std::string(audio->silence_detected ? "True" : "False") + DELIMITER;
            result += std::to_string(audio->peak_amplitude) + DELIMITER;
        }

        // Convert to vector<uint8_t> 
        std::vector<uint8_t> bytes_result(result.begin(), result.end());
        
        // Append payload
        bytes_result.insert(bytes_result.end(), payload.begin(), payload.end());

        return bytes_result;
    }

private:
    std::vector<std::string_view> split_data(const std::string& data) {
        std::vector<std::string_view> result;
        std::string_view view(data);
        
        size_t delimiter_len = strlen(DELIMITER);
        size_t start = 0;
        size_t end = view.find(DELIMITER);
        
        while (end != std::string_view::npos) {
            result.push_back(view.substr(start, end - start));
            start = end + delimiter_len;
            end = view.find(DELIMITER, start);
        }
        
        // Add the last part (payload)
        if (start < view.length()) {
            result.push_back(view.substr(start));
        }
        
        return result;
    }

    void populate_base_metadata(MetadataModels::BaseMetadata& metadata, 
                              const std::vector<std::string_view>& parts) {
        if (parts.size() < 11) {
            throw std::runtime_error("Insufficient data for base metadata");
        }

        metadata.type = std::string(parts[0]);
        metadata.chunk_id = std::string(parts[1]);
        metadata.chunk_index = safe_stoi(std::string(parts[2]));
        metadata.server_hash = std::string(parts[3]);
        metadata.client_hash = std::string(parts[4]);
        metadata.size_kb = safe_stof(std::string(parts[5]));
        metadata.timestamp = std::string(parts[6]);
        metadata.sequence_id = std::string(parts[7]);
        metadata.version = std::string(parts[8]);
        metadata.retry_count = safe_stoi(std::string(parts[9]));
        metadata.processing_time_ms = safe_stof(std::string(parts[10]));
    }

    void populate_timeline_metadata(MetadataModels::TimelineMetadata& metadata,
                                  const std::vector<std::string_view>& parts) {
        if (parts.size() < 16) {
            throw std::runtime_error("Insufficient data for timeline metadata");
        }

        size_t base_offset = 11;
        metadata.start_time = safe_stof(std::string(parts[base_offset]));
        metadata.end_time = safe_stof(std::string(parts[base_offset + 1]));
        
        // Parse arrays if needed
        if (std::string(parts[base_offset + 2]) != "[]") {
            // Parse markers
        }
        if (std::string(parts[base_offset + 3]) != "[]") {
            // Parse events
        }
        if (std::string(parts[base_offset + 4]) != "[]") {
            // Parse dependencies
        }

        metadata.is_keyframe = (std::string(parts[base_offset + 5]) == "True");
    }

    void populate_audio_metadata(MetadataModels::AudioMetadata& metadata,
                               const std::vector<std::string_view>& parts) {
        if (parts.size() < 18) {
            throw std::runtime_error("Insufficient data for audio metadata");
        }

        size_t base_offset = 11;
        metadata.format = std::string(parts[base_offset]);
        metadata.sample_rate = safe_stoi(std::string(parts[base_offset + 1]));
        metadata.channels = safe_stoi(std::string(parts[base_offset + 2]));
        metadata.duration_ms = safe_stof(std::string(parts[base_offset + 3]));
        metadata.is_final = (std::string(parts[base_offset + 4]) == "True");
        metadata.silence_detected = (std::string(parts[base_offset + 5]) == "True");
        metadata.peak_amplitude = safe_stof(std::string(parts[base_offset + 6]));
    }
};