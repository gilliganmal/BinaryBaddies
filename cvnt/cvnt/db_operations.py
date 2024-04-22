# from database import db
from cvnt.tables import *
from cvnt.database import db
from cvnt.implant_pb2 import *
from ip2geotools.databases.noncommercial import DbIpCity# type: ignore

# Task Statuses
STATUS_CREATED  = "implant task created"
STATUS_TASK_RECIEVED = "implant pulled down task"
STATUS_TASK_COMPLETE = "implant successfully compeleted task"
STATUS_TASK_FAILED = "implant failed ot complete task"

def make_implant(ri: RegisterImplant, ip) -> RegisterImplant:
    location = get_location(ip)
    i = Implant(
        implant_id = ri.ImplantID,
        computer_name = ri.ComputerName,
        username = ri.Username,
        ip_addr = ip,
        checkin_frq = 1000,
        latitude = location[0],
        longitude = location[1])
    print(i)
    return i

def register_implant(i):
    db.session.add(i)
    db.session.commit()
    return i

def update_implant_last_seen(iID):
    # Query the Implant table by implant_id
    implant = get_implant_by_id(iID)
    implant.last_seen = func.now()
    db.session.commit()
    return implant

def make_task(target_implantID, tr: TaskRequest): # id, task_id, status, implant_guid, task_opcode, task_args):
    t = Task(
        implant_id = target_implantID,
        task_id = tr.TaskID,
        status = STATUS_CREATED,
        task_opcode = tr.Opcode,
        task_args  = tr.Args
    )
    return t

# returns a list of all implants
def get_list():
    # Query all implants from the Implant table
    all_implants = Implant.query.all()
    # Extract the implant IDs from the list of implants
    implant_ids = [implant.implant_id for implant in all_implants]
    return implant_ids

# gets the longitute and latitude of an implant based on its ip
def get_location(ip):
    res = DbIpCity.get(ip, api_key="free")
    lat = res.latitude
    long = res.longitude
    return [lat, long]

def get_implant_by_id(implant_id):
    # Query the Implant table by implant_id
    implant = db.session.query(Implant).filter_by(implant_id=implant_id).first()
    return implant

def get_task_by_id(task_id):
    # Query the Implant table by implant_id
    implant = db.session.query(Task).filter_by(task_id=task_id).first()
    return implant

def analyze_TaskResponse(tr: TaskResponse):
    if tr is None:
        return ""
    
    print(f'[+] TaskReponse:')
    print(f'[+]    * TaskID: {tr.TaskID}')
    print(f'[+]    * Opcode: {tr.Opcode}')
    print(f'[+]    * Args: {tr.Args}')
    
    # Query the Implant table by implant_id
    task = get_task_by_id(tr.TaskID)
    db.session.commit()

    return ""

def make_dummy_task(ip):
    location = get_location(ip)
    i = Implant(
        implant_id = "Blah Blah",
        computer_name = "Ya Ya",
        username = "username",
        ip_addr = ip,
        checkin_frq = 1000,
        latitude = location[0],
        longitude = location[1])

    r = register_implant(i)