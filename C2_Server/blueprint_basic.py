# basic blueprint

from flask import Blueprint


basic = Blueprint("basic", __name__)

# home route that returns below text when root url is accessed
@basic.route("/")
def root_url():
   return "This is home base..."