
from database import db
from dataclasses import dataclass
import os # for random

# Implants Table
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

'''
class Task(db.Model):
    id =  db.Column(db.Integer, primary_key = True)
    task_id = db.Column(db.String)
    status = db.Column(db.String)
    implant_guid = db.Column(db.String)
    task_opcode = db.Column(db.Integer)
    task_args = db.Column(db.String)
'''

# Tasks Table: Keep track of jobs sent to implants that are in progress/finished
@dataclass 
class Task(db.Model):
    id: int  =  db.Column(db.Integer, primary_key = True)
    task_id: str = db.Column(db.String)
    status: str = db.Column(db.String)
    implant_guid:int   = db.Column(db.Integer)
    task_opcode:str  = db.Column(db.String)
    task_args:str  = db.Column(db.String)

# Clients Table: Keep track of operators connected to the C2 via the client
@dataclass
class Client(db.Model):
    id: int = db.Column(db.Integer, primary_key = True)


# MAKE MODEL FUNCTIONS
def make_implant(id, computer_name, username, computer_GUID, privileges, connecting_IP_addr, session_key, sleep_frq, checkin_frq, first_seen, last_seen, expected_checkin):
    i = Implant(
        id = id,
        computer_name = computer_name, 
        username = username,
        computer_GUID = computer_GUID, 
        privileges = privileges,
        connecting_IP_addr = connecting_IP_addr,
        session_key = session_key,
        sleep_frq = sleep_frq,
        checkin_frq = checkin_frq,
        first_seen = first_seen,
        last_seen = last_seen,
        expected_checkin = expected_checkin
    )
    return i

# Task Statuses
STATUS_CREATED  = "implant created"
STATUS_TASK_RECIEVED = "implant pulled down task"
STATUS_TASK_COMPLETE = "implant successfully compeleted task"
STATUS_TASK_FAILED = "implant failed ot complete task"

# Generate a random task ID
def generate_task_id():
    return os.urandom(16).hex()

def make_task(id, task_id, status, implant_id, task_opcode, task_args):
    t = Task(
        id = id,
        task_id = generate_task_id(), 
        status = STATUS_CREATED,
        implant_id = implant_id,
        task_opcode = task_opcode,
        task_args  = task_args
    )
    return t

def make_client(id):
    c = Client(
        id = id
    )
    return c


opcodes = {
    "exec": 1,
    "whoami": 2,
}
