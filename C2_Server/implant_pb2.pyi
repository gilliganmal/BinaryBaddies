from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class RegisterImplant(_message.Message):
    __slots__ = ("Password", "GUID", "Username", "Hostname")
    PASSWORD_FIELD_NUMBER: _ClassVar[int]
    GUID_FIELD_NUMBER: _ClassVar[int]
    USERNAME_FIELD_NUMBER: _ClassVar[int]
    HOSTNAME_FIELD_NUMBER: _ClassVar[int]
    Password: str
    GUID: str
    Username: str
    Hostname: str
    def __init__(self, Password: _Optional[str] = ..., GUID: _Optional[str] = ..., Username: _Optional[str] = ..., Hostname: _Optional[str] = ...) -> None: ...

class TaskRequest(_message.Message):
    __slots__ = ("TaskGuid", "Opcode", "Args")
    TASKGUID_FIELD_NUMBER: _ClassVar[int]
    OPCODE_FIELD_NUMBER: _ClassVar[int]
    ARGS_FIELD_NUMBER: _ClassVar[int]
    TaskGuid: str
    Opcode: int
    Args: str
    def __init__(self, TaskGuid: _Optional[str] = ..., Opcode: _Optional[int] = ..., Args: _Optional[str] = ...) -> None: ...

class TaskResponse(_message.Message):
    __slots__ = ("TaskGuid", "GUID", "Response")
    TASKGUID_FIELD_NUMBER: _ClassVar[int]
    GUID_FIELD_NUMBER: _ClassVar[int]
    RESPONSE_FIELD_NUMBER: _ClassVar[int]
    TaskGuid: str
    GUID: str
    Response: bytes
    def __init__(self, TaskGuid: _Optional[str] = ..., GUID: _Optional[str] = ..., Response: _Optional[bytes] = ...) -> None: ...

class ImplantCheckin(_message.Message):
    __slots__ = ("GUID", "Resp")
    GUID_FIELD_NUMBER: _ClassVar[int]
    RESP_FIELD_NUMBER: _ClassVar[int]
    GUID: str
    Resp: TaskResponse
    def __init__(self, GUID: _Optional[str] = ..., Resp: _Optional[_Union[TaskResponse, _Mapping]] = ...) -> None: ...
