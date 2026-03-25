"""
Flask Documentation:     https://flask.palletsprojects.com/
Jinja2 Documentation:    https://jinja.palletsprojects.com/
Werkzeug Documentation:  https://werkzeug.palletsprojects.com/
This file creates your application.
"""

# from crypt import methods
import site 

from app import app, Config,  mongo, Mqtt
from flask import render_template, request, jsonify, send_file, redirect, make_response, send_from_directory 
from markupsafe import escape
from json import dumps, loads 
from werkzeug.utils import secure_filename
from datetime import datetime,timedelta, timezone
from os import getcwd
from os.path import join, exists
from time import time, ctime
from math import floor
 



#####################################
#   Routing for your application    #
#####################################

@app.route('/api/station/units', methods=['POST'])
def set_units():
    '''SETS THE UNITS FOR THE STATION. THIS SHOULD BE CALLED ONCE WHEN THE STATION IS FIRST SET UP.'''
    if request.method == "POST":
        '''Add your code here to complete this route'''
        try:
            data = request.get_json()
            print(data)
            if data:
                result = mongo.setUnits(data)
                if result:
                    return jsonify({"status": "success"})
                else:
                    return jsonify({"status": "failure"})
        except Exception as e:
            print(f"set_units error: f{str(e)}")
            return jsonify({"status": "error", "message": str(e)})
    # INVALID REQUEST METHOD
    return jsonify({"status": "invalid method"}), 405


# @app.route('/api/climo/get/<start>/<end>', methods=['GET']) 
# def get_all(start,end):   
#     '''RETURNS ALL THE DATA FROM THE DATABASE THAT EXIST IN BETWEEN THE START AND END TIMESTAMPS'''
   
#     if request.method == "GET":
#         '''Add your code here to complete this route'''
#         try:
#             START = escape(start)
#             END = escape(end)
#             data = mongo.getAllInRange(START, END)
#             if data:
#                 return jsonify({"status": "found", "data":data})
                
#         except Exception as e:
#             print(f"get_data error: f{str(e)}")
#     # FILE DATA NOT EXIST
#     return jsonify({"status":"not found","data":[]})


# @app.route('/api/mmar/temperature/<start>/<end>', methods=['GET']) 
# def get_temperature_mmar(start,end):   
#     '''RETURNS MIN, MAX, AVG AND RANGE FOR TEMPERATURE. THAT FALLS WITHIN THE START AND END DATE RANGE'''
   
#     if request.method == "GET": 
#         '''Add your code here to complete this route'''

#         try:
#             START = escape(start)
#             END = escape(end)
#             data = mongo.temperatureMMAR(START, END)
#             if data:
#                 return jsonify({"status": "found", "data":data})
            
#         except Exception as e:
#             print(f"get_temperature_mmar error: f{str(e)}")

#     # FILE DATA NOT EXIST
#     return jsonify({"status":"not found","data":[]})


# @app.route('/api/mmar/humidity/<start>/<end>', methods=['GET']) 
# def get_humidity_mmar(start,end):   
#     '''RETURNS MIN, MAX, AVG AND RANGE FOR HUMIDITY. THAT FALLS WITHIN THE START AND END DATE RANGE'''
   
#     if request.method == "GET": 
#         '''Add your code here to complete this route'''

#         try:
#             START = escape(start)
#             END = escape(end)
#             data = mongo.humidityMMAR(START, END)
#             if data:
#                 return jsonify({"status": "found", "data":data})
            
#         except Exception as e:
#             print(f"get_humidity_mmar error: f{str(e)}")
#     # FILE DATA NOT EXIST
#     return jsonify({"status":"not found","data":[]})


# @app.route('/api/frequency/<variable>/<start>/<end>', methods=['GET']) 
# def get_freq_distro(variable,start,end):   
#     '''RETURNS FREQUENCY DISTRIBUTION FOR SPECIFIED VARIABLE'''
   
#     if request.method == "GET": 
#         '''Add your code here to complete this route'''     

#         try:
#             VARIABLE = escape(variable)
#             START = escape(start)
#             END = escape(end)
#             data = mongo.frequencyDistro(VARIABLE,START, END)
#             if data:
#                 return jsonify({"status": "found", "data":data})
            
#         except Exception as e:
#             print(f"get_frequency error: f{str(e)}")       

#     # FILE DATA NOT EXIST
#     return jsonify({"status":"not found","data":[]})


##############################################################
# The functions below should be applicable to all Flask apps. #
###############################################################

@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also tell the browser not to cache the rendered page. If we wanted
    to we could change max-age to 600 seconds which would be 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response

@app.errorhandler(405)
def page_not_found(error):
    """Custom 404 page."""    
    return jsonify({"status": 404}), 404



