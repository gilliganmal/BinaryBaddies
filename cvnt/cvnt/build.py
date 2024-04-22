

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
    # app.config.from_mapping(SQLALCHEMY_ECHO = True)

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

        #make_dummy_task("155.33.133.56")
        #make_dummy_task("35.174.183.10")
        #make_dummy_task("10.110.137.189")

    return app