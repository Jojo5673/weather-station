# Internet of Things (IoT) Final Project - Weather System

## Description
This project entails designing, implementing, and testing a weather station with remote monitoring capabilities. This system should possess the capability to measure various meteorological variables, such as air temperature, humidity, air pressure, and more. Moreover, the system should facilitate the viewing of these parameters at the physical weather station and remotely from any location through a web-based graphical user interface (GUI). 

The project has two parts: 
### The Embedded System Weather Station:
* Read and process information from diverse sensors, incorporating a minimum set of parameters such as "air temperature," "air pressure," "altitude," "humidity," "heat index," and "soil moisture."
* Process and transmit sensor data through a WiFi connection to section 2 of the system, which involves web-based data collection, processing, storage, and display.

### Web-Based Data Collection, Processing, Storage and Display System:
* Collect sensor data from section 1 of the system, which is the embedded system weather station.
* Process and store the collected data in an online database.
* Provide access to and display the stored data through a user-friendly graphical user interface (GUI) in a user-friendly format.


## Hardware Setup
Download and install [Arduino](https://www.arduino.cc/en/software) IDE. Subsequently, install the following Arduino IDE libraries following the tutorial [here](https://support.arduino.cc/hc/en-us/articles/5145457742236-Add-libraries-to-Arduino-IDE):
1. Adafruit GFX Library by Adafruit
2. Adafruit ILI9341 by Adafruit
3. ArduinoJson by Benoît Blanchon
4. PubSubClient by Nick O’Leary
5. NewPing by Tim Eckel
6. esp32 by Espressif Systems, from the Board Manager tab in the Arduino IDE



## Backend Setup
Always ensure to establish a virtual environment and install the necessary packages from your requirements file if you haven't already done so. Following that, activate your virtual environment and proceed to run your Flask API.

**The commands below must be executed from a command line terminal in the homeautomation/backend/ folder**
### Create a virtual environment

Windows 
```sh
python -m venv env  
```
Linux
```sh
python3 -m venv env  
```
### Activate virtual environment
Windows
```sh
.\env\Scripts\activate 
```
Linux
```sh
source env/bin/activate
```
### Install API requirements in the virtual environment
```sh
pip install -r requirements.txt 
```
### Create **.env** file
Create a **.env** file in the backend/ folder to store the application's environment variables. 
Refer to the lab manual for the specific information that must be added to this file. Subsequently, modify the **FLASK_RUN_HOST** variable found in this file from localhost to the IP address of the computer your backend is running on.

### Start Flask API
Windows
```sh
py run.py 
```
Linux
```sh
python3 run.py
```



## Frontend Setup ( [Vue js](https://vuejs.org/), [Vuetify](https://vuetifyjs.com/en/components/all/), [Vite](https://vitejs.dev/))
### Recommended IDE Setup
[VSCode](https://code.visualstudio.com/) + [Volar](https://marketplace.visualstudio.com/items?itemName=johnsoncodehk.volar) (and disable Vetur) + [TypeScript Vue Plugin (Volar)](https://marketplace.visualstudio.com/items?itemName=johnsoncodehk.vscode-typescript-vue-plugin).

Customize configuration
See [Vite Configuration Reference](https://vitejs.dev/config/).


### In a command line terminal, execute the first commands in the homeautomation/frontend/ folder to initiate the dev server for the initial setup. 
### For all subsequent instances, only run the second command to start the dev server.
### Once development is complete, run the final command to generate production files. Please be aware that the generation of production files is not part of this course.

Project Setup
```sh
npm install
```

Run dev server (Compile and Hot-Reload for Development)
```sh
npm run dev
```

Create a production bundle (Compile and Minify for Production)
```sh
npm run build
```


### Modify **vite.config.js** file
Modify the target in the proxy object found in the **frontend/vite.config.js** file shown in the code block below. Change localhost in the string to the IP address of the computer your backend is running on.
```js
proxy: {
      '^/api*': { 
        target: 'http://localhost:8080/' ,
       changeOrigin: false,
    },   
  }

```