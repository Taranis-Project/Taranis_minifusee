#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SD.h>
File myFile;
Adafruit_BME280 bme;

// I2Cdev et MPU6050 devoient être installer comme librairies, ou autre comme les fichiers .cpp/.h 
// toutes les classes doivent être dans le chemin d'accés de votre projet
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // pas necessaire si utilisation de fichiers MotionApps 

// la classe par default I2C adresse est 0x68
// l'addresses specific I2C sera adopté comme parametre ici
// AD0 low = 0x68 (adresse par défaut pour SparkFun breakout et InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- utilisé pour AD0 haut

// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
#define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
// #define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
// #define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
// #define OUTPUT_READABLE_WORLDACCEL

// uncomment "OUTPUT_TEAPOT" if you want output that matches the
// format used for the InvenSense teapot demo
// #define OUTPUT_TEAPOT

// MPU control/status vars
bool dmpReady = false;  // mis à vrai si DMP initiatlisation a été réussit
uint8_t mpuIntStatus;   // garde le status actuel du bit d'interruption dupuis le MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

const int PinOUTLED = 8;
const int PinINCom1 = 14; 
const int PinINCom2 = 15;
const int PinOUTdeclenchement = 16;
double moypres;
double presmin = 1013.25;
const double facteurDeDescente = 0.95;
const double alpha = 0.9;
double pression[10];
double numerateur = 0;
double denominateur = 0;
unsigned long t0;

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}
// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  pinMode(PinOUTLED,OUTPUT);//initialize the pin of the LED as an output
  pinMode(PinOUTdeclenchement,OUTPUT);
  pinMode(PinINCom1,INPUT);//set pin for the Jack as INPUT
  pinMode(PinINCom1,INPUT);
 
  Wire.begin();
  Serial.begin(2000000); //on peut changer la vitesse en fonction du projet
  Serial.println("début d'initialisation");
  if (!SD.begin(4)) {
    Serial.println("initialisation raté!");
    while (1);

  } // renvoie un message d'erreur si l'intitialisation a raté
 
  if (!bme.begin()) {
   Serial.println(F("impossible de trouver le capteur BME280 , vérifier le cablage!"));
   while (1) delay(10);
   
  } //renvoie un message d'erreur si le BME280 n'est pas detecté

  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  
  TWBR = 12; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialisation des communication en série
  while (!Serial); // wait for Leonardo enumeration, others continue immediately

  // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
  // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
  // the baud timing being too misaligned with processor ticks. You must use
  // 38400 or slower in these cases, or use some kind of external separate
  // crystal solution for the UART timer.

  // initialize device
  Serial.println(F("intitialisation des systémes I2C..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Test de connections des systémes..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection réussie") : F("MPU6050 échec de connection"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  }

  for (int i = 0; 9; i++)
  {
    pression[i] = bme.readPressure() / 100.0F;
  }
  
  Serial.println("LED allumé");
  digitalWrite(PinOUTLED, HIGH);
  while (digitalRead(PinINCom1) != LOW && digitalRead(PinINCom2) != HIGH);
  Serial.println("Départ");
  digitalWrite(PinOUTLED,LOW);

  t0 = millis();
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {

  myFile = SD.open("DATA.txt", FILE_WRITE); //Pas de nom trop long (place limité)

  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.print(temp());
    digitalWrite(PinOUTLED, LOW);
    Serial.print(F(":"));
    myFile.print(F(":"));
    Serial.print(millis()-t0);
    Serial.print("time,");
    myFile.print(millis()-t0);
    myFile.print("time,");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.print("Pres,");
    myFile.print(bme.readPressure() / 100.0F);
    myFile.print("Pres,");
    Serial.print(bme.readTemperature());
    Serial.print("temp,");
    myFile.print(bme.readTemperature());
    myFile.print("temp,");

    for (int i = 9; 1; i--)
  {
    pression[i] = pression[i-1];
  }
  pression[0] = bme.readPressure() / 100.0F;

    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
      // other program behavior stuff here
      // .
      // .
      // .
      // if you are really paranoid you can frequently test in between other
      // stuff to see if mpuInterrupt is true, and if so, "break;" from the
      // while() loop to immediately process the MPU data
      // .
      // .
      // .
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 2000000) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      Serial.println("FIFO overflow!");
      myFile.println(",overflow");
      digitalWrite(PinOUTLED, HIGH);

      // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
      
      
      // read a packet from FIFO
      mpu.getFIFOBytes(fifoBuffer, packetSize);

      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;

#ifdef OUTPUT_READABLE_QUATERNION
      // display quaternion values in easy matrix form: w x y z
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      Serial.print(q.w);
      Serial.print("w,");
      myFile.print(q.w);
      myFile.print("w,"); 
      Serial.print(q.x);
      Serial.print("x,");
      myFile.print(q.x);
      myFile.print("x,"); 
      Serial.print(q.y);
      Serial.print("y,");
      myFile.print(q.y);
      myFile.print("y,"); 
      Serial.print(q.z);
      Serial.print("z,");
      myFile.print(q.z);
      myFile.print("z,");
#endif
/*
#ifdef OUTPUT_READABLE_EULER
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetEuler(euler, &q);
      myFile.print(euler[0] * 180 / M_PI);
      myFile.print("eu1,");
      Serial.print(euler[0] * 180 / M_PI);
      Serial.print("eu1,");
      myFile.print(euler[1] * 180 / M_PI);
      myFile.print("eu2,");
      Serial.print(euler[1] * 180 / M_PI);
      Serial.print("eu2,");
      myFile.print(euler[2] * 180 / M_PI);
      myFile.print("eu3,");
      Serial.print(euler[2] * 180 / M_PI);
      Serial.print("eu3,");
#endif
*/
#ifdef OUTPUT_READABLE_YAWPITCHROLL
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      myFile.print(ypr[0] * 180 / M_PI);
      myFile.print("yaw,");
      myFile.print(ypr[1] * 180 / M_PI);
      myFile.print("pitch,");
      myFile.print(ypr[2] * 180 / M_PI);
      myFile.println("roll,");
      Serial.print(ypr[0] * 180 / M_PI);
      Serial.print("yaw,");
      Serial.print(ypr[1] * 180 / M_PI);
      Serial.print("pitch,");
      Serial.print(ypr[2] * 180 / M_PI);
      Serial.print("roll,");
#endif
/*
#ifdef OUTPUT_READABLE_REALACCEL
      // display real acceleration, adjusted to remove gravity
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetAccel(&aa, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
      Serial.print("areal\t");
      Serial.print(aaReal.x);
      Serial.print("\t");
      Serial.print(aaReal.y);
      Serial.print("\t");
      Serial.println(aaReal.z);
      myFile.println("areal\t");
      myFile.println(aaReal.x);
      myFile.println("\t");
      myFile.println(aaReal.y);
      myFile.println("\t");
      myFile.println(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
      // display initial world-frame acceleration, adjusted to remove gravity
      // and rotated based on known orientation from quaternion
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetAccel(&aa, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
      mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
      Serial.print("aworld\t");
      Serial.print(aaWorld.x);
      Serial.print("\t");
      Serial.print(aaWorld.y);
      Serial.print("\t");
      Serial.println(aaWorld.z);
      myFile.println("aworld\t");
      myFile.println(aaWorld.x);
      myFile.println("\t");
      myFile.println(aaWorld.y);
      myFile.println("\t");
      myFile.println(aaWorld.z);
#endif

#ifdef OUTPUT_TEAPOT
      // display quaternion values in InvenSense Teapot demo format:
      teapotPacket[2] = fifoBuffer[0];
      teapotPacket[3] = fifoBuffer[1];
      teapotPacket[4] = fifoBuffer[4];
      teapotPacket[5] = fifoBuffer[5];
      teapotPacket[6] = fifoBuffer[8];
      teapotPacket[7] = fifoBuffer[9];
      teapotPacket[8] = fifoBuffer[12];
      teapotPacket[9] = fifoBuffer[13];
      Serial.write(teapotPacket, 14);
      teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
#endif
*/

for (int k = 0; 9; k++)
  {
    numerateur = numerateur + pression[k]*pow(alpha,k);
    denominateur = denominateur + pow(alpha,k);
  }
  moypres = numerateur/denominateur;

    if(presmin > moypres) {
      presmin = moypres;
    }
    else if(moypres >= presmin*facteurDeDescente) {
      digitalWrite(PinOUTdeclenchement, HIGH);
    }
    if(digitalRead(PinINCom1) == LOW && digitalRead(PinINCom2) == HIGH){
      Serial.print("Départ!");
      Serial.print(digitalRead(PinINCom1));
      Serial.print(digitalRead(PinINCom2));
    } else if (digitalRead(PinINCom1) == HIGH && digitalRead(PinINCom2) == LOW ) {
      Serial.print("OuvertureAuto!");
      Serial.print(digitalRead(PinINCom1));
      Serial.print(digitalRead(PinINCom2));
    } else if (digitalRead(PinINCom1) == LOW && digitalRead(PinINCom2) == LOW){
      Serial.print("OuvertureTemps!");
      Serial.print(digitalRead(PinINCom1));
      Serial.print(digitalRead(PinINCom2));
    } else if (digitalRead(PinINCom1) == HIGH && digitalRead(PinINCom2) == HIGH){
      Serial.print("erreur depart!");
      Serial.print(digitalRead(PinINCom1));
      Serial.print(digitalRead(PinINCom2));
    } else {
      Serial.print("erreur com!");
    }
    Serial.println();
    }
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("erreur d'ouverture de 'DATA'");
    digitalWrite(PinOUTLED, LOW);
  }
}
