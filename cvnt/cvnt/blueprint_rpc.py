# rpc blueprint

from flask import Blueprint, request , abort 
from cvnt.implant_pb2 import * 
from cvnt.database import db
from cvnt.models import *
import jsonify

rpc = Blueprint("rpc", __name__)

password = "password"

@rpc.route("/get_csrf", methods=["GET"])
def assign_csrf():
    print(f'Generating a CSRF Token')
    return jsonify({'token': generate_csrf()})

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

'''
@rpc.route("/testpb", methods=["POST"])
def handle_pbtest():
    print(f'TEST PB ROUTE THING')
    register = RegisterImplant()
    req_data = request.get_data()
    register.ParseFromString(req_data)
    print(register)
    return ""
'''

