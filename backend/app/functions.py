 #!/usr/bin/python3


#################################################################################################################################################
#                                                    CLASSES CONTAINING ALL THE APP FUNCTIONS                                                                                                    #
#################################################################################################################################################

class DB:

    def __init__(self,Config):

        from math import floor
        from os import getcwd
        from os.path import join
        from json import loads, dumps, dump
        from datetime import timedelta, datetime, timezone 
        from pymongo import MongoClient , errors, ReturnDocument
        from urllib import parse
        from urllib.request import  urlopen 
        from bson.objectid import ObjectId  
       
      
        self.Config                         = Config
        self.getcwd                         = getcwd
        self.join                           = join 
        self.floor                      	= floor 
        self.loads                      	= loads
        self.dumps                      	= dumps
        self.dump                       	= dump  
        self.datetime                       = datetime
        self.ObjectId                       = ObjectId 
        self.server			                = Config.DB_SERVER
        self.port			                = Config.DB_PORT
        self.username                   	= parse.quote_plus(Config.DB_USERNAME)
        self.password                   	= parse.quote_plus(Config.DB_PASSWORD)
        self.remoteMongo                	= MongoClient
        self.ReturnDocument                 = ReturnDocument
        self.PyMongoError               	= errors.PyMongoError
        self.BulkWriteError             	= errors.BulkWriteError  
        self.tls                            = False # MUST SET TO TRUE IN PRODUCTION

    def __del__(self):
            # Delete class instance to free resources
            pass
    
    def set_mqtt(self, Mqtt):
        self.mqtt = Mqtt
 
    ####################
    # DATABASE UTIL FUNCTIONS  #
    ####################
    
    def addUpdate(self,data):
        '''ADD A NEW STORAGE LOCATION TO COLLECTION'''
        try:
            remotedb 	= self.remoteMongo('mongodb://%s:%s@%s:%s' % (self.username, self.password,self.server,self.port), tls=self.tls)
            #validate data here before inserting into database
            required_fields = ["id", "timestamp"]
            sensor_data = ["moisture", "humidity", "temperature", "pressure", "altitude", "heat_index"]
            if not all(field in data for field in required_fields):
                raise ValueError(f"Missing required fields. Expected: {required_fields}")
            if not any(field in data for field in sensor_data):
                raise ValueError(f"At least one sensor data field is required. Expected at least one of: {sensor_data}")
            if not isinstance(data["timestamp"], int) or data["timestamp"] <= 0:
                raise ValueError("Invalid timestamp")
            if not all(isinstance(data[field], (int, float)) for field in ["moisture", "humidity", "temperature", "pressure", "altitude", "heat_index"]):
                raise ValueError("Invalid data types for numeric fields")
            result      = remotedb.ELET2415.station.insert_one(data)
        except Exception as e:
            msg = str(e)
            if "duplicate" not in msg:
                print("addUpdate error ",msg)
            return False
        else:                  
            return True
        
    def setUnits(self,data):
        '''SETS THE UNITS FOR THE STATION. THIS SHOULD BE CALLED ONCE WHEN THE STATION IS FIRST SET UP.'''
        try:
            self.mqtt.publish("620172690_sub", self.dumps(data)) # PUBLISH UNITS TO MQTT TOPIC. THIS SHOULD BE SUBSCRIBED TO BY THE HARDWARE TO SET THE UNITS ON THE HARDWARE SIDE
        except Exception as e:
            msg = str(e)
            print("setUnits error ",msg)
            return False
        else:                  
            return True
        

    def frequencyDistro(self,variable,start, end):
        '''RETURNS THE FREQUENCY DISTROBUTION FOR A SPECIFIED VARIABLE WITHIN THE START AND END DATE RANGE'''
        try:
            remotedb 	= self.remoteMongo('mongodb://%s:%s@%s:%s' % (self.username, self.password,self.server,self.port), tls=self.tls)
            result      = list(remotedb.ELET2415.station.aggregate([{"$match": {"timestamp": {"$gte": int(start), "$lte": int(end)}}}, {"$bucket": {"groupBy": "$" + variable, "boundaries": [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100], "default": "outliers", "output": {"count": {"$sum": 1}}}}]))
        except Exception as e:
            msg = str(e)
            print("frequencyDistro error ",msg)            
        else:                  
            return result

    def getAggregatedStats(self, start, end):
        '''RETURNS MIN, MAX, AVG FOR ALL SENSOR FIELDS WITHIN THE TIME RANGE'''
        try:
            remotedb = self.remoteMongo('mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            result = list(remotedb.ELET2415.station.aggregate([
                {"$match": {"timestamp": {"$gte": int(start), "$lte": int(end)}}},
                {"$group": {
                    "_id": None,
                    "avgTemp": {"$avg": "$temperature"},
                    "maxTemp": {"$max": "$temperature"},
                    "minTemp": {"$min": "$temperature"},
                    "avgHumidity": {"$avg": "$humidity"},
                    "maxHumidity": {"$max": "$humidity"},
                    "minHumidity": {"$min": "$humidity"},
                    "avgMoisture": {"$avg": "$moisture"},
                    "maxMoisture": {"$max": "$moisture"},
                    "minMoisture": {"$min": "$moisture"},
                    "avgPressure": {"$avg": "$pressure"},
                    "maxPressure": {"$max": "$pressure"},
                    "minPressure": {"$min": "$pressure"},
                    "avgAltitude": {"$avg": "$altitude"},
                    "maxAltitude": {"$max": "$altitude"},
                    "minAltitude": {"$min": "$altitude"},
                    "avgHeatIndex": {"$avg": "$heat_index"},
                    "maxHeatIndex": {"$max": "$heat_index"},
                    "minHeatIndex": {"$min": "$heat_index"},
                    "count": {"$sum": 1}
                }}
            ]))
        except Exception as e:
            msg = str(e)
            print("getAggregatedStats error ", msg)
        else:
            return result if result else [{}]
        return [{}]

    def getTrendLines(self, start, end, granularity="hourly"):
        '''RETURNS HOURLY OR DAILY TREND LINES FOR ALL SENSOR FIELDS'''
        try:
            remotedb = self.remoteMongo('mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            bucket_size = 3600 if granularity == "hourly" else 86400
            result = list(remotedb.ELET2415.station.aggregate([
                {"$match": {"timestamp": {"$gte": int(start), "$lte": int(end)}}},
                {"$group": {
                    "_id": {
                        "$multiply": [
                            {"$subtract": ["$timestamp", {"$mod": ["$timestamp", bucket_size]}]},
                            1000
                        ]
                    },
                    "avgTemp": {"$avg": "$temperature"},
                    "avgHeatIndex": {"$avg": "$heat_index"},
                    "avgHumidity": {"$avg": "$humidity"},
                    "avgMoisture": {"$avg": "$moisture"},
                    "avgPressure": {"$avg": "$pressure"},
                    "avgAltitude": {"$avg": "$altitude"}
                }},
                {"$sort": {"_id": 1}}
            ]))
        except Exception as e:
            msg = str(e)
            print("getTrendLines error ", msg)
        else:
            return result if result else []
        return []

    def getFrequencyDistributionBucketed(self, variable, start, end):
        '''RETURNS FREQUENCY DISTRIBUTION WITH APPROPRIATE BOUNDARIES FOR EACH VARIABLE'''
        boundaries = {
            "temperature": [10, 15, 18, 21, 24, 27, 30, 33, 36, 40],
            "heat_index": [10, 15, 18, 21, 24, 27, 30, 33, 36, 40],
            "humidity": [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100],
            "moisture": [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100],
            "pressure": [99000, 100000, 101000, 101325, 102000, 103000],
            "altitude": [0, 5, 10, 15, 20, 25, 30, 40, 50]
        }
        try:
            remotedb = self.remoteMongo('mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            bucket_boundaries = boundaries.get(variable, [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100])
            result = list(remotedb.ELET2415.station.aggregate([
                {"$match": {"timestamp": {"$gte": int(start), "$lte": int(end)}}},
                {"$bucket": {
                    "groupBy": "$" + variable,
                    "boundaries": bucket_boundaries,
                    "default": "Other",
                    "output": {"count": {"$sum": 1}}
                }}
            ]))
        except Exception as e:
            msg = str(e)
            print("getFrequencyDistributionBucketed error ", msg)
        else:
            return result if result else []
        return []

    def getScatterPlotData(self, start, end, limit=2000):
        '''RETURNS RAW DATA FOR SCATTER PLOT ANALYSIS'''
        try:
            remotedb = self.remoteMongo('mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            result = list(remotedb.ELET2415.station.find(
                {"timestamp": {"$gte": int(start), "$lte": int(end)}},
                {"temperature": 1, "humidity": 1, "moisture": 1, "heat_index": 1, "pressure": 1, "altitude": 1, "_id": 0}
            ).limit(limit))
        except Exception as e:
            msg = str(e)
            print("getScatterPlotData error ", msg)
        else:
            return result if result else []
        return []

    def getHeatStressEvents(self, start, end, threshold=32):
        '''RETURNS HEAT STRESS EVENT COUNT AND TIMESTAMPS WHERE HEAT_INDEX > THRESHOLD'''
        try:
            remotedb = self.remoteMongo('mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            # Get count
            count_result = list(remotedb.ELET2415.station.aggregate([
                {"$match": {
                    "timestamp": {"$gte": int(start), "$lte": int(end)},
                    "heat_index": {"$gt": threshold}
                }},
                {"$count": "heat_stress_readings"}
            ]))
            event_count = count_result[0]["heat_stress_readings"] if count_result else 0
            
            # Get events
            events = list(remotedb.ELET2415.station.find(
                {"timestamp": {"$gte": int(start), "$lte": int(end)}, "heat_index": {"$gt": threshold}},
                {"timestamp": 1, "heat_index": 1, "_id": 0}
            ).sort("timestamp", 1))
        except Exception as e:
            msg = str(e)
            print("getHeatStressEvents error ", msg)
        else:
            return {"count": event_count, "events": events if events else []}
        return {"count": 0, "events": []}
        
 



def main():
    from config import Config
    from time import time, ctime, sleep
    from math import floor
    from datetime import datetime, timedelta
    one = DB(Config)
 
 
    start = time() 
    end = time()
    print(f"completed in: {end - start} seconds")
    
if __name__ == '__main__':
    main()


    
