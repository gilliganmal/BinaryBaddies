import requests 
from urllib.parse  import urljoin 
from implant_pb2 import *
from client_pb2 import Command

c2 = "http://127.0.0.1:5000"


task_list = "/task/list"
task_create = "task/create"
register = "/register"

def create_dummy_task(implant_id=1, opcode="exec", args="whoami /priv"):
    r = requests.get(urljoin(c2, "/get_command"))
    if r.status_code == 200:
        command = Command()
        command.ParseFromString(r.content)
        if command.cmd:
            # Use command.cmd to create the task
            print(f"Creating task with command: {command.cmd}")
            # Your task creation logic here
            return True
    print("Sad!")
    return False


'''
def create_dummy_implant():
    r = RegisterImplant()
    r.Password = "foobar"
    r.Username = "lol"
    r.Hostname = "Ghost"
    r.GUID = "xxxxxxxxxxxx"
    out = r.SerializeToString()
    r = requests.post(urljoin( c2, register), data = out)
'''

create_dummy_task()