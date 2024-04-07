from flask import Flask, request
from flask import Flask, render_template, redirect, url_for
from flask_bootstrap import Bootstrap5
import secrets
from flask_wtf import FlaskForm, CSRFProtect
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
from client_pb2 import Command

msg = Command()
app = Flask(__name__)

app.secret_key = 'tO$&!|0wkamvVia0?n$NqIRVWOG'

# Bootstrap-Flask requires this line
bootstrap = Bootstrap5(app)
# Flask-WTF requires this line
csrf = CSRFProtect(app)

foo = secrets.token_urlsafe(16)
app.secret_key = foo


class Terminal(FlaskForm):
    cmd = StringField('=> ', validators=[DataRequired(), Length(1, 400)])

@app.route('/', methods=['GET', 'POST'])
def index():
    whole = ""
    form = Terminal()
    if form.validate_on_submit():
        whole = form.cmd.data
        firstword, leftoverstring = whole.split(' ', 1)
        msg.cmd = firstword
        msg.args = leftoverstring
        print('Command received successfully!')
    return render_template('index.html', form=form, cmd=whole)

if __name__ == '__main__':
    app.run(debug=True)
