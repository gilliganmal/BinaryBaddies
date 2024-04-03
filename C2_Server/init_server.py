# init_server.py

from flask import Flask 
from C2_Server.database import db

def build_app():
    app = Flask(__name__)
    app.config.from_mapping(
            SQLALCHEMY_DATABASE_URI = 'postgresql://speedrun:gofast@localhost:5432/src2'
#"sqlite:///c2.db"  
        
    )
    app.register_blueprint(admin)
    app.register_blueprint(rpc)
    db.init_app(app)
    return app 


def init_db():
    db.drop_all()
    db.create_all()