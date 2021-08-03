// Before flashing this script, you must set the time (if you didn't do it already).
// Hence, flash the example from the RTCLib library (ds1307 or softrtc).
// After that, the time is set, and if you have a battery it is kept into your Arduino.

// ## IMPORT LIBRARIES and instantiate objects

// for date logging, date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_DS1307 rtc;

// for temp sensor
#include <DHT.h>
#include <Adafruit_Sensor.h>

// for SD reader
#include <SPI.h>
#include <SD.h>

// DHT SENSOR: Initialize DHT sensor for normal 16 mHz Arduino:
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht = DHT(DHTPIN, DHTTYPE);

// SD MODULE: change this to match the CS pin of the SD module
const int chipSelect = 10;

// choose the LEDs pins (we use some free one)
//#define led1 3
//#define led2 4
////digitalWrite(led1, HIGH);


// ## SET USEFUL VARIABLES

// cannot be more than 8 chars by FAT standard
char filename[] = "hhmmss.txt";    // the filename array, I will change it with the date_time

// variables for date and time
char current_date[] = "yyyy/mm/dd";
char current_time[] = "hh:mm:ss";

File myFile;                                // my log file

int id = 1;                                 // to later increase the ID on the log file
int log_frequency = 1000;                   // log frequency in ms



// ## FUNCTIONS

void setup() {    

    // Begin serial communication at a baud rate of 9600
    Serial.begin(9600);

    #ifndef ESP8266
      while (!Serial); // wait for serial port to connect. Needed for native USB
    #endif

    // check if rtc is being recognised
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      abort();
    }
    
    // let's start fetching the date_time to create the unique filename
    DateTime now = rtc.now(); // take the current time

    // create filename == hhmmss.txt (note: the filename cannot be > 8 chars by FAT standard)
    sprintf(filename, "%02d%02d%02d.txt",  now.hour(),now.minute(),now.second());
    
    // Setup DHT sensor
    dht.begin();

    // initialise SD card
    Serial.print("Initializing SD card...");

    if (!SD.begin()) {
      Serial.println("initialization failed!");
      return;
    }
    Serial.println("initialization done.");
    
    // initialise data file with headers
    myFile = SD.open(filename, FILE_WRITE);

    // if the file opened okay, write the headers in it
    if (myFile) {
      Serial.print("Initialising data file...");
      // write the headers in one line comma separated, every time the Arudino starts (ID, Datetime, Temp, Hum, Flag)
      myFile.print("ID");myFile.print(",");
      myFile.print("date");myFile.print(",");
      myFile.print("time");myFile.print(",");
      myFile.print("Temperature");myFile.print(",");
      myFile.println("Humidity");
      
      // close the file:
      myFile.close();
      
      Serial.println("Done, all good!");
      } else {
        Serial.println("SOMETHING WENT WRONG - couldn't initilise the log file on the SD card");
      }    
}

void loop() {

  DateTime now = rtc.now(); // take the current time

  // Read the humidity (in %) from the DHT22 sensor
  float h = dht.readHumidity();
  // Read the temperature (in Celsius) from the DHT22 sensor
  float t = dht.readTemperature();

  // Check if everything's good with the reading from the DHT22:
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // write the headers (our colummns) on the SD card log file
  myFile = SD.open(filename, FILE_WRITE);

  if (myFile) { // if the file opened okay, write to it:
    
    Serial.println("I'm writing"); // for debug purposes

    // create current_date variable
    sprintf(current_date, "%4i/%02d/%02d",  now.year(),now.month(),now.day());
    
    // create current_time variable
    sprintf(current_time, "%02d:%02d:%02d",  now.hour(),now.minute(),now.second());

    // log into file on the SD card
    myFile.print(id);myFile.print(",");
    myFile.print(current_date);myFile.print(",");
    myFile.print(current_time);myFile.print(",");
    myFile.print(t);myFile.print(",");
    myFile.println(h);
    
    // debug on serial
    Serial.print(id);Serial.print(",");
    Serial.print(current_date);Serial.print(",");
    Serial.print(current_time);Serial.print(",");
    Serial.print(t);Serial.print(",");
    Serial.println(h);

    myFile.close(); // close the log file

    id++; // increase the id number
    
    delay(log_frequency); // log data every X second/s
  }
}
