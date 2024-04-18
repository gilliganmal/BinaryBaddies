from flask import Blueprint, render_template, request, redirect, url_for
from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
import secrets
from client_pb2 import Command

client = Blueprint('client', __name__, template_folder='templates')

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
        if len(parts) == 2:
            firstword, leftoverstring = parts
            msg = Command()
            msg.cmd = firstword
            msg.args = leftoverstring
            print('Slay Baddies your command was received successfully!')
        else:
            error_message = 'Not enough arguments provided!'  # Set the error message
    return render_template('index.html', form=form, cmd=whole, error_message=error_message)
