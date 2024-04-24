from cvnt.blueprint_client import *
from cvnt.blueprint_terminal import *
from flask import Flask, Blueprint, session, url_for
from flask import render_template_string, render_template
from flask import redirect
from flask import request
from cvnt.db_operations import *
import os
import subprocess
import shutil
from flask_wtf import FlaskForm
from wtforms import HiddenField

explorer = Blueprint('explorer', __name__, template_folder='templates', static_folder='static')

class Menu(FlaskForm):
    selected_implant = SelectField('Select Implant', choices=[])


# handle root route
@explorer.route('/explorer')
def root():
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    implant_id = "slayyy"
    implants = get_list()
    menu = Menu()
    menu.selected_implant.choices = implants
    form = Terminal()
    if menu.validate_on_submit():
        form.cmd.data = 'dir'
        try:
            implant_id = menu.selected_implant.data
            #file_list = new(implant_id, form)
        except AttributeError:
            implant_id = "slayyy"
    return render_template('explorer.html', current_working_directory='.', form=Menu,
         file_list=analyze_input(form, implant_id), implants=implants, implant_id=implant_id) 

# handle 'cd' command
@explorer.route('/cd')
def cd():
    # run 'level up' command
    os.chdir(request.args.get('path'))
    
    # redirect to file manager
    return redirect('/explorer')

# view text files
@explorer.route('/new', methods=['POST'])
def new(implant_id, form):
    # get the file content
    form.cmd.data = 'dir'
    reposense = analyze_input(form, implant_id)
    return reposense
    
# view text files
@explorer.route('/view')
def view():
    # get the file content
    with open(request.args.get('file')) as f:
        return f.read().replace('\n', '<br>')
