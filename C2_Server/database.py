# database.py

from flask_sqlalchemy import SQLAlchemy
from dataclasses import dataclass
import os # for random

import implant_statuses

db  = SQLAlchemy()

# Implant Table
@dataclass
class Implant(db.Model):
    # Implant ID: Create an ID for the implant to distinguish is from others
    id =  db.Column(db.Integer, primary_key = True)
    # Computer Name: What computer did it connect from?
    computer_name = db.Column(db.String)
    # Username: What user are you running as?
    username = db.Column(db.String)
    # GUID: What is the computer's GUID?
    computer_GUID = db.Column(db.String)
    # Integrity: What privileges do you have?
    privileges = db.Column(db.String)
    # Connecting IP address: what address did it connect from?
    connecting_IP_addr = db.Column(db.String)
    # Session Key: After you negotiated a session key, store it per agent
    session_key = db.Column(db.String)
    # Sleep: How often does the agent check in?
    sleep_frq = db.Column(db.String)
    # Jitter: How random of a check in is it?
    checkin_frq = db.Column(db.String)
    # First Seen: When did the agent first check in
    first_seen = db.Column(db.String)
    # Last Seen: When was the the last time you saw the agent?
    last_seen = db.Column(db.String)
    # Expected Check in: When should you expect to see the agent again?
    expected_checkin = db.Column(db.String)


# Generate a random task ID
def generate_task_id():
    return os.urandom(16).hex()

# Commands : Keep track of which operators issued what command

# Task Table: Keep track of jobs sent to implants that are in progress/finished
@dataclass 
class Task(db.Model):
    id: int  =  db.Column(db.Integer, primary_key = True)
    task_id: str = db.Column(db.String)
    status: str = db.Column(db.String)
    implant_id:int   = db.Column(db.Integer)
    task_opcode:str  = db.Column(db.String)
    task_args:str  = db.Column(db.String)


# Clients: Keep track of operators connected to the C2 via the client
@dataclass
class Client(db.Model):
    id: int = db.Column(db.Integer, primary_key = True)


# MAKE MODEL FUNCTIONS
# TO DO FILL IN BLANKS
def make_implant():
    i = Implant()
    return i

def make_task():
    t = Task(
        task_id=make_task_id(), 
        status = STATUS_CREATED,
        implant_id = implant_id,
        task_args  = task_args
        )
    return t

def make_client():
    c = Client()
    return c