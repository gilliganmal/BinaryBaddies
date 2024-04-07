from flask import Flask, request
from flask import Flask, render_template, redirect, url_for
from flask_bootstrap import Bootstrap
import secrets
from flask_wtf import FlaskForm, CSRFProtect
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
from client_pb2 import Command

msg = Command()
app = Flask(__name__)

app.secret_key = 'tO$&!|0wkamvVia0?n$NqIRVWOG'

# Bootstrap-Flask requires this line
bootstrap = Bootstrap(app)
# Flask-WTF requires this line
csrf = CSRFProtect(app)

foo = secrets.token_urlsafe(16)
app.secret_key = foo


class Terminal(FlaskForm):
    cmd = StringField('=> ', validators=[DataRequired(), Length(1, 400)])

@app.route('/', methods=['GET', 'POST'])
def index():
    form = Terminal()
    whole = None
    error_message = None  # Initialize the error message to None
    if form.validate_on_submit():
        whole = form.cmd.data
        parts = whole.split(' ', 1)
        if len(parts) == 2:
            firstword, leftoverstring = parts
            msg.cmd = firstword
            msg.args = leftoverstring
            print('Command received successfully!')
        else:
            error_message = 'Not enough arguments provided!'  # Set the error message
    return render_template('index.html', form=form, cmd=whole, error_message=error_message)



if __name__ == '__main__':
    app.run(debug=True)
