#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "DataSerialization.hpp"
#include "MetaDataModels.h"

namespace py = pybind11;

PYBIND11_MODULE(streaming_protocol, m) {
    py::class_<MetadataModels::BaseMetadata>(m, "BaseMetadata")
        .def(py::init<>())
        .def_readwrite("type", &MetadataModels::BaseMetadata::type)
        .def_readwrite("chunk_id", &MetadataModels::BaseMetadata::chunk_id)
        .def_readwrite("chunk_index", &MetadataModels::BaseMetadata::chunk_index)
        .def_readwrite("server_hash", &MetadataModels::BaseMetadata::server_hash)
        .def_readwrite("client_hash", &MetadataModels::BaseMetadata::client_hash)
        .def_readwrite("size_kb", &MetadataModels::BaseMetadata::size_kb)
        .def_readwrite("timestamp", &MetadataModels::BaseMetadata::timestamp)
        .def_readwrite("sequence_id", &MetadataModels::BaseMetadata::sequence_id)
        .def_readwrite("version", &MetadataModels::BaseMetadata::version)
        .def_readwrite("retry_count", &MetadataModels::BaseMetadata::retry_count)
        .def_readwrite("processing_time_ms", &MetadataModels::BaseMetadata::processing_time_ms)
        .def("get_type", &MetadataModels::BaseMetadata::get_type)
        .def("is_valid", &MetadataModels::BaseMetadata::is_valid)
        .def("to_dict", &MetadataModels::BaseMetadata::to_dict);

    py::class_<MetadataModels::TimelineMetadata, MetadataModels::BaseMetadata>(m, "TimelineMetadata")
        .def(py::init<>())
        .def_readwrite("start_time", &MetadataModels::TimelineMetadata::start_time)
        .def_readwrite("end_time", &MetadataModels::TimelineMetadata::end_time)
        .def_readwrite("markers", &MetadataModels::TimelineMetadata::markers)
        .def_readwrite("events", &MetadataModels::TimelineMetadata::events)
        .def_readwrite("dependencies", &MetadataModels::TimelineMetadata::dependencies)
        .def_readwrite("is_keyframe", &MetadataModels::TimelineMetadata::is_keyframe)
        .def("get_type", &MetadataModels::TimelineMetadata::get_type)
        .def("to_dict", &MetadataModels::TimelineMetadata::to_dict);

    py::class_<MetadataModels::AudioMetadata, MetadataModels::BaseMetadata>(m, "AudioMetadata")
        .def(py::init<>())
        .def_readwrite("format", &MetadataModels::AudioMetadata::format)
        .def_readwrite("sample_rate", &MetadataModels::AudioMetadata::sample_rate)
        .def_readwrite("channels", &MetadataModels::AudioMetadata::channels)
        .def_readwrite("duration_ms", &MetadataModels::AudioMetadata::duration_ms)
        .def_readwrite("is_final", &MetadataModels::AudioMetadata::is_final)
        .def_readwrite("silence_detected", &MetadataModels::AudioMetadata::silence_detected)
        .def_readwrite("peak_amplitude", &MetadataModels::AudioMetadata::peak_amplitude)
        .def("get_type", &MetadataModels::AudioMetadata::get_type)
        .def("to_dict", &MetadataModels::AudioMetadata::to_dict);

    py::class_<DataSerializer>(m, "DataSerializer")
        .def(py::init<>())
        .def("decode", [](DataSerializer& self, const py::bytes& data) {
            std::string str = static_cast<std::string>(data);
            std::vector<uint8_t> input_data(str.begin(), str.end());
            
            auto [metadata, payload] = self.decode(input_data);
            
            return std::make_tuple(std::move(metadata), 
                                 py::bytes(reinterpret_cast<const char*>(payload.data()), 
                                         payload.size()));
        })
        .def("encode", [](DataSerializer& self, 
                         const MetadataModels::BaseMetadata& metadata, 
                         const py::bytes& payload) {
            std::string str = static_cast<std::string>(payload);
            std::vector<uint8_t> bytes_payload(str.begin(), str.end());
            
            auto result = self.encode(metadata, bytes_payload);
            return py::bytes(reinterpret_cast<const char*>(result.data()), 
                           result.size());
        });
}