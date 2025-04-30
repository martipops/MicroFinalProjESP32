# Intrusion Detection System
## Build Instructions
1. Make sure PlatformIO (on vscode) and Node.js is downloaded
2. Clone the repo
3. Rename `include/secrets.h.example` to `secrets.h` and change the WIFI SSID and Password to your local wifi network. If the ESP32 cannot connect it will host an access point with the information from `AP_SSID` and `AP_PASSWORD`
4. Run the setup commands:
```bash
# Build the web interface
cd microfinal-web
npm i; npm run build
cd ..

# Convert the html into a byte array for use in main.cpp
python convert_html.py --nobuild 
```
5. Build and Upload the PlatformIO project.

## Project Information
This project uses an ESP32 and 3 sensors to create a motion detection system.

We used 3 sensors to detect intrusion:
1. MQ-8 Gas Sensor Module​ (Detects hydrogen)
2. HC-SR501 PIR Motion Detector​ (Detects movement)
3. KY-037 Microphone Module (Detects sound)

For the user interface, we host an HTTP server that renders the web-app. This web app contains line graphs of the sound and gas sensor, and also displays a binary indicator for motion detection.

Each of the sensors have a 'danger' threshold that is configurable via the frontend. If the sensor data goes past this threshold, the affected graph will highlight red, indicating a 'danger' level.

The data received is also downloadable as a CSV file.

### Backend Overview
The backend primarily does two things in a continuous loop:
1. Handles incoming HTTP Clients
2. Refreshes sensor data into a buffer

For the HTTP server, there are three routes:
1. The root route (`/`) sends the html bytes through a GET request. This is the primary basis for the frontend. 
2. The `/read` route returns JSON formatted data, containing the buffers for each sensor. This includes all the necessary information to be handled by the frontend. The frontend contains a timer that calls this route in a loop, to continuously refresh information.
3. The `/api/interval` route allows the user to change the rate at which data is refreshed, in milleseconds.

When refreshing the data, we do two things:
1. If its time to refresh the data (based on the interval variable), data is shifted in the array down 1 index to make room for the new retrieved data.
2. To retrieve the data from the sensors, `digitalRead` and `analogRead` are called for each of the pins on the ESP32 in which the respective sensors are connected. This new data goes in the first index of each sensor buffer.


### Frontend Overview

#### Compilation
To create the frontend, I used Node.js + Vue to create the local frontend, and then compiled it into one single `index.html` using the `vite-plugin-singlefile` node package. Using this HTML file, I created a python script that converts the plaintext HTML into a compressed `gzip` bytestream, and placed it in a header file to be used in the `main.cpp` as a single variable containing the HTML data.

#### Code Overview
For the charts, I used the Node.js package `JSCharts` to render the graphs shown. The rest is basic TypeScript and styled HTML with a little bit of Vue magic. As mentioned above, a timer is set to refresh the data from the buffer in the backend. Everything is updated dynamically thanks to the Vue.js ecosystem.