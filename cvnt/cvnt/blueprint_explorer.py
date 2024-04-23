from cvnt.blueprint_client import client
from flask import Flask, Blueprint, session, url_for
from flask import render_template_string, render_template
from flask import redirect
from flask import request
import os
import subprocess
import shutil

explorer = Blueprint('explorer', __name__, template_folder='templates', static_folder='static')

# handle root route
@explorer.route('/explorer')
def root():
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    return render_template( 'explorer.html', current_working_directory=os.getcwd(),
         file_list=subprocess.check_output('ls', shell=True).decode('utf-8').split('\n')) # use 'dir' command on Windows
    
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
    
# view text files
@explorer.route('/view')
def view():
    # get the file content
    with open(request.args.get('file')) as f:
        return f.read().replace('\n', '<br>')
