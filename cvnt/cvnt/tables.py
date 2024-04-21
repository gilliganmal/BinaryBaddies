from database import db
from dataclasses import dataclass
# from implant_pb2 import * 

@dataclass
class Implant(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    implant_id = db.Column(db.String)
    computer_name = db.Column(db.String)
    username = db.Column(db.String)

# Tasks Table: Keep track of jobs sent to implants that are in progress/finished
@dataclass 
class Task(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    implant_id: str =  db.Column(db.String)
    task_id: str = db.Column(db.String)
    status: str = db.Column(db.String)
    implant_guid:int   = db.Column(db.Integer)
    task_opcode:str  = db.Column(db.String)
    task_args:str  = db.Column(db.String)


# Clients Table: Keep track of operators connected to the C2 via the client
@dataclass
class Client(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    client_id: str = db.Column(db.String)


