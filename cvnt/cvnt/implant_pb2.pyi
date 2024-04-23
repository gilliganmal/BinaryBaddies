from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class RegisterImplant(_message.Message):
    __slots__ = ("Password", "ImplantID", "Username", "ComputerName")
    PASSWORD_FIELD_NUMBER: _ClassVar[int]
    IMPLANTID_FIELD_NUMBER: _ClassVar[int]
    USERNAME_FIELD_NUMBER: _ClassVar[int]
    COMPUTERNAME_FIELD_NUMBER: _ClassVar[int]
    Password: str
    ImplantID: str
    Username: str
    ComputerName: str
    def __init__(self, Password: _Optional[str] = ..., ImplantID: _Optional[str] = ..., Username: _Optional[str] = ..., ComputerName: _Optional[str] = ...) -> None: ...

class TaskRequest(_message.Message):
    __slots__ = ("TaskID", "Opcode", "Args")
    TASKID_FIELD_NUMBER: _ClassVar[int]
    OPCODE_FIELD_NUMBER: _ClassVar[int]
    ARGS_FIELD_NUMBER: _ClassVar[int]
    TaskID: int
    Opcode: int
    Args: str
    def __init__(self, TaskID: _Optional[int] = ..., Opcode: _Optional[int] = ..., Args: _Optional[str] = ...) -> None: ...

class TaskResponse(_message.Message):
    __slots__ = ("TaskID", "ImplantID", "Response")
    TASKID_FIELD_NUMBER: _ClassVar[int]
    IMPLANTID_FIELD_NUMBER: _ClassVar[int]
    RESPONSE_FIELD_NUMBER: _ClassVar[int]
    TaskID: int
    ImplantID: str
    Response: bytes
    def __init__(self, TaskID: _Optional[int] = ..., ImplantID: _Optional[str] = ..., Response: _Optional[bytes] = ...) -> None: ...

class ImplantCheckin(_message.Message):
    __slots__ = ("ImplantID", "Resp")
    IMPLANTID_FIELD_NUMBER: _ClassVar[int]
    RESP_FIELD_NUMBER: _ClassVar[int]
    ImplantID: str
    Resp: TaskResponse
    def __init__(self, ImplantID: _Optional[str] = ..., Resp: _Optional[_Union[TaskResponse, _Mapping]] = ...) -> None: ...
