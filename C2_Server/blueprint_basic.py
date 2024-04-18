# basic blueprint

from flask import Blueprint
from flask import Flask, render_template


basic = Blueprint("basic", __name__)

# home route that returns below text when root url is accessed
@basic.route("/")
def root_url():
   return render_template('landing.html')