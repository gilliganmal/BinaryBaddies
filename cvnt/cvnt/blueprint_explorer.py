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
    selected_implant = HiddenField('Selected Implant')


# handle root route
@explorer.route('/explorer')
def root():
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    implants = get_list()
    Menu = Menu()
    form = Terminal
    form.cmd.data = 'dir'
    implant_id = form.selected_implant.data
    return render_template( 'explorer.html', '.', form=Menu, implants=implants,
         file_list=analyze_input(form, implant_id)) 

'''   
# handle 'cd' command
@explorer.route('/cd')
def cd():
    # run 'level up' command
    os.chdir(request.args.get('path'))
    
    # redirect to file manager
    return redirect('/explorer')

# handle 'make directory' command
@explorer.route('/md')
def md():
    # create new folder
    os.mkdir(request.args.get('folder'))
    
    # redirect to fole manager
    return redirect('/explorer')

# handle 'make directory' command
@explorer.route('/rm')
def rm():
    # remove certain directory
    shutil.rmtree(os.getcwd() + '/' + request.args.get('dir'))
    
    # redirect to fole manager
    return redirect('/explorer')
    
''' 
# view text files
@explorer.route('/view')
def view():
    # get the file content
    with open(request.args.get('file')) as f:
        return f.read().replace('\n', '<br>')
