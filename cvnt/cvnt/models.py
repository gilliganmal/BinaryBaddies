'''
from database import db
from dataclasses import dataclass
from implant_pb2 import * 

# Implants Table
@dataclass
class Implant(db.Model):
    # Implant ID: Create an ID for the implant to distinguish is from others
    implant_id =  db.Column(db.Integer, primary_key = True)
    # Computer Name: What computer did it connect from?
    computer_name = db.Column(db.String)
    # Username: What user are you running as?
    username = db.Column(db.String)
    # GUID: What is the computer's GUID?
    # computer_GUID = db.Column(db.String)
    # Integrity: What privileges do you have?
    # privileges = db.Column(db.String)
    # Connecting IP address: what address did it connect from?
    # connecting_IP_addr = db.Column(db.String)
    # Session Key: After you negotiated a session key, store it per agent
    # session_key = db.Column(db.String)
    # Sleep: How often does the agent check in?
    # sleep_frq = db.Column(db.Integer)
    # Jitter: How random of a check in is it?
    # checkin_frq = db.Column(db.Integer)
    # First Seen: When did the agent first check in
    # first_seen = db.Column(db.String)
    # Last Seen: When was the the last time you saw the agent?
    # last_seen = db.Column(db.String)
    # Expected Check in: When should you expect to see the agent again?
    # expected_checkin = db.Column(db.String)


# MAKE MODEL FUNCTIONS
def make_implant(ri):
    i = Implant(
        implant_id = ri.GUID,
        computer_name = ri.Hostname,
        username = ri.Username)
        # computer_GUID = computer_GUID, 
        # privileges = "",
        # connecting_IP_addr = ip,
        # session_key = "",
        # sleep_frq = 1000,
        # checkin_frq = 1000,
        # first_seen = func.now(),
        # last_seen = func.now())
        # expected_checkin = )
    # print(f'Implant made from {ip}.')
    return i

def register_implant(i):
    print(f'About to db.session.add(implant)')
    db.session.add(i)
    print(f'About to db.session.commit()')
    db.session.commit()
    print(f'Done registering implant.')
    return i

# Tasks Table: Keep track of jobs sent to implants that are in progress/finished
@dataclass 
class Task(db.Model):
    id: int  =  db.Column(db.Integer, primary_key = True)
    task_id: str = db.Column(db.String)
    status: str = db.Column(db.String)
    implant_guid:int   = db.Column(db.Integer)
    task_opcode:str  = db.Column(db.String)
    task_args:str  = db.Column(db.String)


# Task Statuses
STATUS_CREATED  = "implant task created"
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


def get_task_for_implant(implant_guid):
    """
    Get a task for an implant
    """
    i = db.session.query(Implant).filter_by(implant_guid=implant_guid).first()
    if not i:
        print(f"No implant with id {i}")
        return False
    task = (
        db.session.query(Task).filter_by(implant_id=i.id, status=STATUS_CREATED).first()
    )
    if task:
        task.status = STATUS_TASK_RECIEVED
        db.session.add(task)
        db.session.commit()
        print(f"Pulling down {task}")
        return task
    return False


def handle_task_complete(task_id, tr: TaskResponse):
    with db.session() as session:
        try:
            if not tr.TaskGuid:
                return False
            task = session.query(Task).filter_by(task_id=tr.TaskGuid).first()
            if not task:
                print(f"Task {tr} could not be updated as the ID is missing ")
                return False
            task.status = STATUS_TASK_COMPLETE
            task.task_response = tr.Response
            session.add(task)
            session.commit()
            print("Task {tr.TaskGuid} has completed")
        except:
            print(f"Error handling task {tr}")
            task.status = STATUS_TASK_FAILED
            return False
    return True



# Clients Table: Keep track of operators connected to the C2 via the client
@dataclass
class Client(db.Model):
    id: int = db.Column(db.Integer, primary_key = True)


def make_client(id):
    c = Client(
        id = id,
        username = username,
        password = password
    )
    return c
'''
