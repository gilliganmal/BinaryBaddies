

from flask import Flask

from database import db

from blueprint_admin import admin
from blueprint_basic import basic
from blueprint_client import client
from blueprint_rpc import rpc


def build_app():
    app = Flask(__name__)

    app.config.from_mapping(
            SQLALCHEMY_DATABASE_URI="postgresql://baddie:pass@localhost:5432/c2"
    )
    
    #app.register_blueprint(basic)
    #app.register_blueprint(client)
    #app.register_blueprint(admin)
    app.register_blueprint(rpc)

    db.init_app(app)

    with app.app_context():
        db.drop_all()
        db.create_all()
        db.session.commit()
        print('SLDKJFLKSJDFLJSLDJFJSDLFJLS')
    return app

