const int Pin_LED = 8;

//------------------SD---------------------
#include <SPI.h>
#include <SD.h>
File donnees;
const int Pin_CS = 10;
//----------------MPU+I2C------------------
#include <Wire.h>
#include <I2Cdev.h>
#include "MPU6050_6Axis_MotionApps20.h"
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
//------------------BME--------------------
#include "SparkFunBME280.h"
BME280 bme;
//-----------------TELEM-------------------
#include "LoRa_E32.h"
UART_BPS_RATE bpsRate = UART_BPS_RATE_19200;
LoRa_E32 e32ttl100(14, 7,15,bpsRate); // e32 TX e32 RX

float presmin;
const double facteurDeDescente = 1.000005;

void setup() {
  pinMode(Pin_LED, OUTPUT);
  //-----------------------Serial-----------------------
  Serial.begin(19200);
  if (!Serial) {
    while (1) {
      digitalWrite(Pin_LED, HIGH);
      delay(100);
      digitalWrite(Pin_LED, LOW);
      delay(100);
    }
  }
  Serial.print("test");
  //------------------------BME--------------------
  Wire.begin();
  bme.setI2CAddress(0x76);   //adresse du capteur de pression BMP280
  bme.beginI2C();
  if (!bme.begin()) {
    Serial.println("Echec connextion BME");
    while (1) {
      digitalWrite(Pin_LED, HIGH);
      delay(100);
      digitalWrite(Pin_LED, LOW);
      delay(100);
    }
  }
  Serial.println("2");
  //-------------------------SD------------------------
  if (!SD.begin(Pin_CS)) {
    Serial.println("Echec connextion SD");
    while (1) {
      digitalWrite(Pin_LED, HIGH);
      delay(100);
      digitalWrite(Pin_LED, LOW);
      delay(100);
    }
  }
  Serial.println("3");
  //------------------------MPU+I2C--------------------
  Wire.begin();
  
  mpu.initialize();

  mpu.dmpInitialize();            //initialise la connexion au calculateur du MPU6050
  
  if (!mpu.testConnection()) {
    Serial.println("Echec connextion MPU");
    while (1) {
      digitalWrite(Pin_LED, HIGH);
      delay(100);
      digitalWrite(Pin_LED, LOW);
      delay(100);
    }
  }
  
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);  //Configure le capteur à 16g
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);   //Configure le capteur à 250 deg/sec
  mpu.setDLPFMode(6);
  mpu.setDHPFMode(1);

  mpu.setXAccelOffset(-704);     //1G az=2043
  mpu.setYAccelOffset(1822);     //1G ay=2044        
  mpu.setZAccelOffset(656);    //1G ax=2077
  mpu.setXGyroOffset(60);
  mpu.setYGyroOffset(8);
  mpu.setZGyroOffset(11);
  //-----------------------TELEM--------------------
  pinMode(16,OUTPUT);
  digitalWrite(16,HIGH);
  e32ttl100.begin();
  
  digitalWrite(Pin_LED, HIGH);
}

void loop() {
  byte s;
  unsigned long t = millis();
  unsigned long delai;
  int16_t p = bme.readFloatPressure() - 100000;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  while(Serial.available()){
    s = Serial.read();
  }
  byte c = bme.readTempC();
  char frame[59];
  snprintf(frame, sizeof(frame), "t%lup%ds%dg%dz%dh%dy%dj%dx%dc%d\n", t, p, s, gx, ax, -gy, -ay, -gz, -az, c);
                                      // temps pression serial gory z acc z gyro y acc y gyro x acc x température
  donnees = SD.open("donnees.txt", FILE_WRITE);
  donnees.println(frame);
  donnees.close();
  if (p <= presmin) {
    presmin = p;
  } 
  else if (s == '2' && p>=presmin*facteurDeDescente) {
    Serial.print('1');
  }
  if (millis() - delai >= 150){
  ResponseStatus rs = e32ttl100.sendMessage(frame);
  delai = millis();
  }
}
