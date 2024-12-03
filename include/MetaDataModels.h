#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <map>

namespace MetadataModels {

struct BaseMetadata {
    std::string type = "unknown";
    std::string chunk_id;
    int chunk_index = 0;
    std::string server_hash;
    std::string client_hash;
    float size_kb = 0.0f;
    std::string timestamp;
    std::string sequence_id;
    std::string version = "1.0";
    int retry_count = 0;
    float processing_time_ms = 0.0f;

    virtual ~BaseMetadata() = default;
    virtual std::string get_type() const { return "base"; }
    bool is_valid() const { return client_hash == server_hash; }
    
    virtual std::map<std::string, std::variant<std::string, int, float, bool, std::vector<std::string>>> to_dict() const {
        return {
            {"type", type},
            {"chunk_id", chunk_id},
            {"chunk_index", chunk_index},
            {"server_hash", server_hash},
            {"client_hash", client_hash},
            {"size_kb", size_kb},
            {"timestamp", timestamp},
            {"sequence_id", sequence_id},
            {"version", version},
            {"retry_count", retry_count},
            {"processing_time_ms", processing_time_ms}
        };
    }
};

struct TimelineMetadata : public BaseMetadata {
    float start_time = 0.0f;
    float end_time = 0.0f;
    std::vector<std::string> markers;
    std::vector<std::string> events;
    std::vector<std::string> dependencies;
    bool is_keyframe = false;

    std::string get_type() const override { return "timeline"; }
    
    std::map<std::string, std::variant<std::string, int, float, bool, std::vector<std::string>>> to_dict() const override {
        auto base = BaseMetadata::to_dict();
        base["start_time"] = start_time;
        base["end_time"] = end_time;
        base["markers"] = markers;
        base["events"] = events;
        base["dependencies"] = dependencies;
        base["is_keyframe"] = is_keyframe;
        return base;
    }
};

struct AudioMetadata : public BaseMetadata {
    std::string format = "unknown";
    int sample_rate = 16000;
    int channels = 1;
    float duration_ms = 0.0f;
    bool is_final = false;
    bool silence_detected = false;
    float peak_amplitude = 0.0f;

    std::string get_type() const override { return "audio"; }
    
    std::map<std::string, std::variant<std::string, int, float, bool, std::vector<std::string>>> to_dict() const override {
        auto base = BaseMetadata::to_dict();
        base["format"] = format;
        base["sample_rate"] = sample_rate;
        base["channels"] = channels;
        base["duration_ms"] = duration_ms;
        base["is_final"] = is_final;
        base["silence_detected"] = silence_detected;
        base["peak_amplitude"] = peak_amplitude;
        return base;
    }
};

using MetadataVariant = std::variant<BaseMetadata, TimelineMetadata, AudioMetadata>;

static std::unique_ptr<BaseMetadata> create_metadata(const std::string& type) {
    if (type == "timeline") return std::make_unique<TimelineMetadata>();
    if (type == "audio") return std::make_unique<AudioMetadata>();
    return std::make_unique<BaseMetadata>();
}

} // namespace MetadataModels