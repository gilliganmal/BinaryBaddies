# init_server.py

from flask import Flask 
# from database import db
from blueprint_admin import admin
# from blueprint_rpc import rpc


def build_app():
    app = Flask(__name__)
    
    # basic database connection URL = dialect://username:password@host:port/database
    # TODO add a "SECRET_KEY" to app configuration
    app.config.from_mapping(SQLALCHEMY_DATABASE_URI = 'postgresql://user:pass@localhost:6666/c2.db')

    app.register_blueprint(admin)
    # app.register_blueprint(rpc)

    # db.init_app(app)

    return app 

'''
def init_db():
    db.drop_all()
    db.create_all()
'''

if __name__ == '__main__':  
   app = build_app()
   app.run()