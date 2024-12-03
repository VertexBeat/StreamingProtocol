from typing import Tuple, List, Optional, Dict, Union, Any

class BaseMetadata:
    type: str
    chunk_id: str
    chunk_index: int
    server_hash: str
    client_hash: str
    size_kb: float
    timestamp: str
    sequence_id: str
    version: str
    retry_count: int
    processing_time_ms: float

    def __init__(self) -> None: ...
    def get_type(self) -> str: ...
    def is_valid(self) -> bool: ...
    def to_dict(self) -> Dict[str, Union[str, int, float, bool, List[str]]]: ...

class TimelineMetadata(BaseMetadata):
    start_time: float
    end_time: float
    markers: List[str]
    events: List[str]
    dependencies: List[str]
    is_keyframe: bool

    def __init__(self) -> None: ...
    def get_type(self) -> str: ...
    def to_dict(self) -> Dict[str, Union[str, int, float, bool, List[str]]]: ...

class AudioMetadata(BaseMetadata):
    format: str
    sample_rate: int
    channels: int
    duration_ms: float
    is_final: bool
    silence_detected: bool
    peak_amplitude: float

    def __init__(self) -> None: ...
    def get_type(self) -> str: ...
    def to_dict(self) -> Dict[str, Union[str, int, float, bool, List[str]]]: ...

class DataSerializer:
    def __init__(self) -> None: ...
    def decode(self, data: bytes) -> Tuple[BaseMetadata, bytes]: ...
    def encode(self, metadata: BaseMetadata, payload: bytes) -> bytes: ...