from cvnt.database import db
from dataclasses import dataclass

@dataclass
class Implant(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    implant_id = db.Column(db.String)
    computer_name = db.Column(db.String)
    username = db.Column(db.String)
    public_key = db.Column(db.String)
    ip_addr = db.Column(db.String)
    checkin_frq = db.Column(db.Integer)
    first_seen = db.Column(db.DateTime)
    last_seen = db.Column(db.DateTime)
    latitude = db.Column(db.String)
    longitude = db.Column(db.String)


@dataclass
class Task(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    implant_id = db.Column(db.String)
    task_id = db.Column(db.Integer)
    status = db.Column(db.String)
    task_opcode = db.Column(db.Integer)
    task_args = db.Column(db.String)
    task_output = db.Column(db.String)


'''
# Clients Table: Keep track of operators connected to the C2 via the client
@dataclass
class Client(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    client_id: str = db.Column(db.String)
'''

