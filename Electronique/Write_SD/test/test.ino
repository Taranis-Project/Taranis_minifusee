/*
  SD card datalogger

 This example shows how to log data from three analog sensors
 to an SD card using the SD library.

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void loop() {
  // make a string for assembling the data to log:
  
  unsigned long t=millis();
  char frame[59];
  snprintf(frame, sizeof(frame), "t%lup1834s32g-14z-14958h-27y444j-4x-16054c29\n",t);


  File dataFile = SD.open("datalog.txt", FILE_WRITE);


    dataFile.println(frame);
    dataFile.close();

Serial.println(millis()-t);

  File file = SD.open("WR_TEST2.TXT", O_CREAT | O_APPEND | O_WRITE);

  file.println(frame);
 
  file.flush();  
 
  file.close();

  Serial.println(millis()-t);
}
