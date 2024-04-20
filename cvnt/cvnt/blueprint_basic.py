# basic blueprint

from flask import Flask, render_template, request, redirect, url_for, flash, Blueprint
from werkzeug.security import generate_password_hash
from flask_login import LoginManager 
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length
from flask_wtf import FlaskForm


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
         #flash('You are logged in')
         return redirect(url_for('client.index'))
      else:
         flash('Thank you! You have succesfully signed up for email alerts. See you soon ...')
         return render_template('landing.html', form=form)  
   return render_template('landing.html', form=form) 
