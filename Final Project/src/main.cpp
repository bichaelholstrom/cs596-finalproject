#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Arduino.h>
#include "SparkFunLSM6DSO.h"
#include <WiFi.h>
#include <HttpClient.h>
#include <string>
#include <cstring>

//Task Handlers to run wifi on core 0 and heart rate monitor on core 1
//WiFi breaks heart monitor if both are running on same core
TaskHandle_t Task0;
TaskHandle_t Task1;

// Create a PulseOximeter object
PulseOximeter pox;
#define REPORTING_PERIOD_MS     1000
uint32_t tsLastReport = 0; // Time at which the last beat occurred
float heart_rate;
float spo2;

//accelermatorer and variables
LSM6DSO myIMU;
float x_accel;
float y_accel;
float z_accel;
float temperature;
float x_offset = 0;
float y_offset = 0;
float z_offset = 0;


// Callback routine is executed when a pulse is detected
void onBeatDetected() {
    Serial.println("Beat!");
}

//WIFI variables
const char ssid[] = "";    // your network SSID (name) 
const char pass[] = ""; // your network password (use for WPA, or use as key for WEP)
const char kHostname[] = "3.137.171.85"; // Name of the server we want to connect to
const uint16_t port = 5000; //port we wish to connect to
const int kNetworkTimeout = 30*1000; // Number of milliseconds to wait without receiving any data before we give up
const int kNetworkDelay = 1000; // Number of milliseconds to wait if no data is available before trying again

void Task0Code(void * pvParameters){
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("MAC address: ");
    Serial.println(WiFi.macAddress());

    std::string fall_result = "false";

    for(;;){
        WiFiClient c;
        HttpClient http(c);
        x_accel = myIMU.readFloatAccelX();
        y_accel = myIMU.readFloatAccelX();
        z_accel = myIMU.readFloatAccelX();
        temperature = myIMU.readTempF();

        //construct path for get request format
        std::string path = "/?temp=" + std::to_string(temperature) + "&x=" + std::to_string(x_accel)
        + "&y=" + std::to_string(y_accel) + "&z=" + std::to_string(z_accel)+ "&heart=" + std::to_string(heart_rate);
        int length = path.length();
        char* kPath = new char[length + 1];
        strcpy(kPath, path.c_str()); 
  
        http.get(kHostname, port ,kPath);

        delay(500);
    }
}

void Task1Code(void * pvParameters){
    for(;;){
         // Read from the sensor
        pox.update();

        // Grab the updated heart rate and SpO2 levels
        if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
            heart_rate = pox.getHeartRate();
            spo2 = pox.getSpO2();
            tsLastReport = millis();
        }
    }
}
void setup() {
    Serial.begin(9600);

    Serial.print("Initializing pulse oximeter..");

    // Initialize sensor
    if (!pox.begin()) {
        Serial.println("FAILED");
    } else {
        Serial.println("SUCCESS");
    }

	// Configure sensor to use 7.6mA for LED drive
	pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback routine
    pox.setOnBeatDetectedCallback(onBeatDetected);

    
    //initializing lsm6dso
    if( myIMU.begin() )
        Serial.println("Ready.");
    else { 
        Serial.println("Could not connect to IMU.");
        Serial.println("Freezing");
    }

    if(myIMU.initialize(BASIC_SETTINGS) )
        Serial.println("Loaded Settings.");
    
    xTaskCreatePinnedToCore(
        Task0Code,
        "Task0",
        10000,
        NULL,
        1,
        &Task0,
        0);

    xTaskCreatePinnedToCore(
        Task1Code,
        "Task1",
        10000,
        NULL,
        1,
        &Task1,
        1);
        
}
void loop() {

}