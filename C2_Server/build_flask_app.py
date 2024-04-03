# init_server.py

from flask import Flask 
from C2_Server.database import db
from C2_Server.app_blueprints.admin import admin
from C2_Server.app_blueprints.rpc import rpc


def build_app():
    app = Flask(__name__)
    
    # basic database connection URL = dialect://username:password@host:port/database
    # TODO add a "SECRET_KEY" to app configuration
    app.config.from_mapping(SQLALCHEMY_DATABASE_URI = 'postgresql://user:pass@localhost:6666/c2.db')

    app.register_blueprint(admin)
    app.register_blueprint(rpc)

    db.init_app(app)

    return app 


def init_db():
    db.drop_all()
    db.create_all()