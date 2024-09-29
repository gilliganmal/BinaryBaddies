# basic blueprint

from flask import session, render_template, request, redirect, url_for, flash, Blueprint # type: ignore
from wtforms import StringField, SubmitField # type: ignore
from wtforms.validators import DataRequired # type: ignore
from flask_wtf import FlaskForm # type: ignore
from cvnt.blueprint_client import client

basic = Blueprint("basic", __name__)

class Email(FlaskForm):
    email = StringField('Email', validators=[DataRequired()])
    submit = SubmitField('Submit')

# home route that returns below text when root url is accessed
@basic.route("/", methods=['GET', 'POST'])
def login_success():
    form = Email()
    if request.method == 'POST' and  form.validate_on_submit():
        email = form.email.data
        if email == "username.password@gmail.com":
            session['authenticated'] = True
            return redirect('client')
        else:
            flash('Thank you! You have successfully signed up for email alerts. See you soon ...')
            return render_template('landing.html', form=form)  
    return render_template('landing.html', form=form) 

