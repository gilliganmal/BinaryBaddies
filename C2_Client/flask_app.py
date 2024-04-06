from flask import Flask, request
from flask import Flask, render_template, redirect, url_for
from flask_bootstrap import Bootstrap5
import secrets
from flask_wtf import FlaskForm, CSRFProtect
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
import requests

app = Flask(__name__)

app.secret_key = 'tO$&!|0wkamvVia0?n$NqIRVWOG'

# Bootstrap-Flask requires this line
bootstrap = Bootstrap5(app)
# Flask-WTF requires this line
csrf = CSRFProtect(app)

foo = secrets.token_urlsafe(16)
app.secret_key = foo

class Terminal(FlaskForm):
    cmd = StringField('=> ', validators=[DataRequired(), Length(3, 400)])

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        print(request.form)  # Print out the form data
    term = Terminal()
    cmd = term.cmd.data
    return render_template('index.html', form=term, cmd=cmd)



if __name__ == '__main__':
    app.run(debug=True)
