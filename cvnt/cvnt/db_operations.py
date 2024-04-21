# from database import db
from tables import *
from implant_pb2 import *

def make_implant(ri: RegisterImplant):
    i = Implant(
        implant_id = ri.GUID,
        computer_name = ri.Hostname,
        username = ri.Username)
    return i

def register_implant(i):
    print(i)
    print(f'About to db.session.add(implant)')
    db.session.add(i)
    print(f'About to db.session.commit()')
    db.session.commit()
    print(f'Done registering implant.')
    return i
