

from flask import Flask

from cvnt.database import db

from cvnt.blueprint_admin import admin
from cvnt.blueprint_basic import basic
from cvnt.blueprint_client import client
from cvnt.blueprint_rpc import rpc
from cvnt.blueprint_explorer import explorer
from flask_bootstrap import Bootstrap
from cvnt.tables import *
import secrets

from flask_wtf import FlaskForm, CSRFProtect
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length

from cvnt.db_operations import make_dummy_task

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
    # app.register_blueprint(explorer)
    csrf.exempt(rpc)

    db.init_app(app)
    with app.app_context():
        db.drop_all()
        db.create_all()
        db.session.commit()
        print('[+] DATABASE INITIALIZED')

    '''
    im = Implant(
        implant_id = "Blah Blah",
        computer_name = "Ya Ya",
        username = "username",
        ip_addr = "10.10.10.1",
        checkin_frq = 1000,
        latitude = 133.3,
        longitude = 22.0
    )
    with app.app_context():
        db.session.add(im)
        db.session.commit()
    '''
    return app