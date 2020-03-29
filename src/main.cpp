#define __AVR_ATtiny84__

// GOODREAD => http://www.gammon.com.au/i2c

#include <Arduino.h>
#include "TinyWireS.h"
#include "Wire.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// dht vars
const unsigned int dhtpin = 6;
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT_Unified dht(dhtpin, DHTTYPE);

unsigned long previousDhtSensorPolling = 0;
unsigned long dhtSensorPollingInterval = 15000;

uint32_t delayMS;

float temperature = 0.0;
float humidity = 0.0;
// i2c vars
const byte SLAVE_ADDR = 0x64;
const byte NUM_BYTES = 4;
enum {
    CMD_ID = 1,
    CMD_READ_TEMP  = 2,
    CMD_READ_HUMIDITY = 3
  };
char command;

void requestEvent() {
    switch (command)
     {
      case CMD_ID:  
        TinyWireS.write (0x64); 
        break;   // send our ID 
      case CMD_READ_TEMP: {
        byte *tempByte = (byte *)&temperature;
        for (byte i=0; i<NUM_BYTES; i++) {
            TinyWireS.write(tempByte[i]);
        }
        break;
      }
      case CMD_READ_HUMIDITY: {
        byte *humidByte = (byte *)&humidity;
        for (byte i=0; i<NUM_BYTES; i++) {
            TinyWireS.write(humidByte[i]);
        }
        break;
        }
     }
}



void receiveEvent (uint8_t howMany)
{
  command = TinyWireS.read();  // remember command for when we get request
} // end of receiveEvent

void setup() {
  // setup i2c
  command = 0;
  TinyWireS.begin(SLAVE_ADDR);
  TinyWireS.onReceive (receiveEvent);
  TinyWireS.onRequest(requestEvent);
  // setup dht.begin();
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousDhtSensorPolling > dhtSensorPollingInterval) {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    temperature = event.temperature;
    humidity = event.relative_humidity;
  }
}

