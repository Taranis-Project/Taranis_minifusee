
#include <Wire.h>
#include <I2Cdev.h>
#include "MPU6050_6Axis_MotionApps20.h"
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
const int Pin_LED = 8;

void setup() {  
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(1000000000);       //Ultra Fast Mod à 1Mhz

  
  mpu.initialize();

  mpu.dmpInitialize();            //initialise la connexion au calculateur du MPU6050

  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);  //Configure le capteur à 16g
  Serial.println(mpu.getFullScaleAccelRange());
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);   //Configure le capteur à 250 deg/sec
  Serial.println(mpu.getFullScaleGyroRange());
  mpu.setDLPFMode(6);
  Serial.println(mpu.getDLPFMode());
  mpu.setDHPFMode(1);
  Serial.println(mpu.getDHPFMode());
  
  if (!mpu.testConnection()) {
    Serial.println("Echec connextion MPU");
    while (1) {
      digitalWrite(Pin_LED, HIGH);
      delay(100);
      digitalWrite(Pin_LED, LOW);
      delay(100);
    }
  }
  mpu.setXAccelOffset(-704);     //2043
  mpu.setYAccelOffset(1822);     //2044        
  mpu.setZAccelOffset(656);    //ax=2077
  mpu.setXGyroOffset(60);
  mpu.setYGyroOffset(8);
  mpu.setZGyroOffset(11);
}

void loop() {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    int16_t ax2=-az;
    int16_t ay2=-ay;
    int16_t az2=+ax;
  
    int16_t gx2=-gz;
    int16_t gy2=-gy;
    int16_t gz2=+gx; 
  Serial.println();
  Serial.println("ax:");
  Serial.println(ax2);
  Serial.println("ay:");
  Serial.println(ay2);
  Serial.println("az:");
  Serial.println(az2);
  Serial.println("gx:");
  Serial.println(gx2);
  Serial.println("gy:");
  Serial.println(gy2);
  Serial.println("gz:");
  Serial.println(gz2);
}
