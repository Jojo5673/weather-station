from flask import Flask 
from .config import Config

from os import name    
from .functions import DB 
from .mqtt import MQTT  

# Create MongoDB instance to get access to all the functions defined in functions.py
mongo = DB(Config)
Mqtt  = MQTT(mongo)
mongo.set_mqtt(Mqtt) # SET THE MQTT INSTANCE IN THE DB CLASS SO THAT IT CAN BE ACCESSED THROUGH THE DB INSTANCE IN THE ROUTES TO PUBLISH MESSAGES TO THE HARDWARE WHEN NEEDED

app = Flask(__name__)
app.config.from_object(Config) 

from app import views