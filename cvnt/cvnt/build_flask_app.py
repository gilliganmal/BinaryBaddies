# init_server.py

from flask import Flask 
from cvnt.database import db
from cvnt.blueprint_basic import basic
from cvnt.blueprint_admin import admin
from cvnt.blueprint_rpc import rpc
from cvnt.blueprint_client import client
from flask import Flask, request
from flask import Flask, render_template, redirect, url_for
from flask_bootstrap import Bootstrap

import secrets
from flask_wtf import FlaskForm, CSRFProtect
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length


def build_app():
    app = Flask(__name__)

    app.secret_key = 'tO$&!|0wkamvVia0?n$NqIRVWOG'

    # Bootstrap-Flask requires this line
    bootstrap = Bootstrap(app)
    
    # Flask-WTF requires this line
    # Packets from IMPLANT to SERVER require a CSRF token
    csrf = CSRFProtect(app)

    # foo = secrets.token_urlsafe(16)
    # app.secret_key = foo

    # basic database connection URL = dialect://username:password@host:port/database
    app.config.from_mapping(SQLALCHEMY_DATABASE_URI = 'postgresql://user:pass@localhost:5000/c2.db')
    app.config.from_mapping(SQLALCHEMY_ECHO = True)

    app.register_blueprint(basic) # CSRF Token sent to client here
    #app.register_blueprint(admin)
    app.register_blueprint(rpc) # CSRF Token sent to implant
    app.register_blueprint(client)

    csrf.exempt(rpc)

    db.init_app(app)


    return app 

def init_db():
    db.drop_all()
    db.create_all()


if __name__ == '__main__':  
   app = build_app()
   app.run(host='0.0.0.0', debug=True)


