from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class Command(_message.Message):
    __slots__ = ("ImplantID", "cmd", "args")
    IMPLANTID_FIELD_NUMBER: _ClassVar[int]
    CMD_FIELD_NUMBER: _ClassVar[int]
    ARGS_FIELD_NUMBER: _ClassVar[int]
    ImplantID: str
    cmd: str
    args: str
    def __init__(self, ImplantID: _Optional[str] = ..., cmd: _Optional[str] = ..., args: _Optional[str] = ...) -> None: ...

class ClientTaskRequest(_message.Message):
    __slots__ = ("ImplantID", "JobID", "Function", "Inputs")
    IMPLANTID_FIELD_NUMBER: _ClassVar[int]
    JOBID_FIELD_NUMBER: _ClassVar[int]
    FUNCTION_FIELD_NUMBER: _ClassVar[int]
    INPUTS_FIELD_NUMBER: _ClassVar[int]
    ImplantID: str
    JobID: str
    Function: str
    Inputs: str
    def __init__(self, ImplantID: _Optional[str] = ..., JobID: _Optional[str] = ..., Function: _Optional[str] = ..., Inputs: _Optional[str] = ...) -> None: ...

class ClientTaskResponse(_message.Message):
    __slots__ = ("ImplantID", "JobID", "Output")
    IMPLANTID_FIELD_NUMBER: _ClassVar[int]
    JOBID_FIELD_NUMBER: _ClassVar[int]
    OUTPUT_FIELD_NUMBER: _ClassVar[int]
    ImplantID: int
    JobID: int
    Output: str
    def __init__(self, ImplantID: _Optional[int] = ..., JobID: _Optional[int] = ..., Output: _Optional[str] = ...) -> None: ...

class Packet(_message.Message):
    __slots__ = ("message", "CSRF")
    MESSAGE_FIELD_NUMBER: _ClassVar[int]
    CSRF_FIELD_NUMBER: _ClassVar[int]
    message: str
    CSRF: str
    def __init__(self, message: _Optional[str] = ..., CSRF: _Optional[str] = ...) -> None: ...
