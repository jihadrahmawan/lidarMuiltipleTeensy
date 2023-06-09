/*
This example shows how to set up and read multiple VL53L1X sensors connected to
the same I2C bus. Each sensor needs to have its XSHUT pin connected to a
different Arduino pin, and you should change sensorCount and the xshutPins array
below to match your setup.

For more information, see ST's application note AN4846 ("Using multiple VL53L0X
in a single design"). The principles described there apply to the VL53L1X as
well.
*/

#include <Wire.h>
#include "VL53L1X.h"

const uint8_t sensorCount = 8;
const uint8_t xshutPins[sensorCount] = { 26, 27, 28, 29, 30, 31, 32, 33}; //kiri tengah, arah depan, kanan belakang, kiri belakang, kiri depan, kanan depan, kanan tengah, gripper
volatile uint16_t distances_mm[sensorCount];
const int max_distance = 2000;  //mm
volatile int counter = 0;

VL53L1X sensors[sensorCount];
VL53L1X sensorbelakang;

void setup()
{
  while (!Serial) {}
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C
  Wire2.begin();
  Wire2.setClock(400000); // use 400 kHz I2C
  

  //sensorbelakang.
 // 

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);

    sensors[i].setTimeout(500);
    if (!sensors[i].init())
    {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.println(i);
      while (1);
    }

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(0x2A + i);

    sensors[i].startContinuous(50);
  }
  
  sensorbelakang.setBus(&Wire2); //sensor belakang pakai i2c 2
  sensorbelakang.setTimeout(500);
  if (!sensorbelakang.init())
    {
      while (1){
      Serial.print("Failed to detect and initialize sensor belakang");
      Serial.println();
        delay(500);
      }
    }
  sensorbelakang.startContinuous(50);
  delay(5000);
}

void loop()
{
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    Serial.print(sensors[i].read());
    if (sensors[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    Serial.print('\t');
  }
  
  Serial.println();
  Serial.print("lidar belakang = ");
  Serial.println(sensorbelakang.read());

}
