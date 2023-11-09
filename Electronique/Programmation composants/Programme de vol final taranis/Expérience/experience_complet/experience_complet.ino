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

int seuil = 10;
float pression[5] = {0,0,0,0,0};
unsigned long delai;
void setup() {
  pinMode(Pin_LED, OUTPUT);
  //-----------------------Serial-----------------------
  Serial.begin(19200);
  Serial.setTimeout(1);
  if (!Serial) {
    while (1) {
      digitalWrite(Pin_LED, HIGH);
      delay(100);
      digitalWrite(Pin_LED, LOW);
      delay(100);
    }
  }
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
  donnees = SD.open("donnees.txt", O_CREAT | O_APPEND | O_WRITE);
  donnees.println("départ:");
  donnees.close();
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
  mpu.setZAccelOffset(619);    //1G ax=2077
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
  int16_t p = bme.readFloatPressure() - 100000;
  
  for (int i = 4; i>=1; i--)
  {
    pression[i] = pression[i-1];
  }
  pression[0] = p;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  while(Serial.available()){
    const int BUFFER_SIZE = 64;
    char buf[BUFFER_SIZE];
    int rlen = Serial.readBytes(buf, BUFFER_SIZE);
    s = buf[rlen-2];
  }
  byte c = bme.readTempC();
  char frame[59];
  snprintf(frame, sizeof(frame), "t%lup%ds%dg%dz%dh%dy%dj%dx%dc%d\n", t, p, s, gx, ax, -gy, -ay, -gz, -az, c);
                                      // temps pression serial gyro z acc z gyro y acc y gyro x acc x température
  donnees = SD.open("donnees.txt", O_CREAT | O_APPEND | O_WRITE);
  donnees.println(frame);
  donnees.flush(); //si problème SD, essaye d'enlever les //
  donnees.close();
  
  float max1 = pression[0];
  for (int j = 1;j < 5; j++)
  {
    if(max1 < pression[j])
    {
        max1 = pression[j];
    }
  }

  float min1 = pression[0];
  for (int j = 1;j < 5; j++)
  {
    if(max1 > pression[j])
    {
        min1 = pression[j];
    }
  }

  if (s == '2' && max1 - min1 < seuil ) {
    Serial.print('1');
  }
  if (millis() - delai >= 70)
  {
  e32ttl100.sendMessage(frame);
  delai = millis();
  }
  
}
