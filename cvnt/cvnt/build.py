

from flask import Flask

from database import db

from blueprint_admin import admin
from blueprint_basic import basic
from blueprint_client import client
from blueprint_rpc import rpc

from flask_bootstrap import Bootstrap

import secrets

from flask_wtf import FlaskForm, CSRFProtect
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length

SECRET_KEY = 'tO$&!|0wkamvVia0?n$NqIRVWOG'

def build_app():
    app = Flask(__name__)
    
    app.secret_key = SECRET_KEY

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

    csrf.exempt(rpc)

    db.init_app(app)

    with app.app_context():
        db.drop_all()
        db.create_all()
        db.session.commit()
        print('DATABASE INITIALIZED')
    return app

