/*****************************************/
#include <Servo.h> //bibliothéque servo (déjà présente sur arduino)
Servo Servomoteur; //créer un objet servo
const int Pin_LED= 10;
const int Pin_Jack = 9;
const int Pin_Servo = 11;
const int Position_initiale=120;
const int Position_finale=180;

unsigned long dt1=7.6*0.8;    //temps avant plage d'ouverture para en sec
unsigned long dt2=7.6*1.2;    //temps ouverture para obligatoire en sec 
unsigned long dt1_millis;
unsigned long dt2_millis;
unsigned long t1_millis;
unsigned long t2_millis;
unsigned long t0;


void Ouverture(){
  Servomoteur.write(Position_finale);
  digitalWrite(Pin_LED, HIGH);
}

void setup() {
  Servomoteur.attach(Pin_Servo);   // Attache le servomoteur au PinServo
  Servomoteur.write(Position_initiale);          // Met le servomoteur en position
  dt1_millis=dt1*1000; 
  dt2_millis=dt2*1000;
  Serial.begin(19200);
  pinMode(Pin_Jack,INPUT_PULLUP);              //set as INPUT
  pinMode(Pin_LED,OUTPUT);
  digitalWrite(Pin_LED, HIGH);
}

void loop() {
  if(LOW==digitalRead(Pin_Jack)){
    t0 = millis();                            // prend T0
    Serial.print('1');
    digitalWrite(Pin_LED, LOW);
    t1_millis = dt1_millis+t0;
    t2_millis = dt2_millis+t0;
    while(millis()<=t1_millis && LOW==digitalRead(Pin_Jack)){
      Serial.print('1');
    }
    while(millis()<=t2_millis && LOW==digitalRead(Pin_Jack)){
      Serial.print('2');
      if(Serial.available() && Serial.read()=='1' && LOW==digitalRead(Pin_Jack)){
        Serial.print('3');
        Ouverture();
        while(1){
          Serial.print('3');
        }
      }
    }
    Serial.print('4');
    Ouverture();
    while(1){
      Serial.print('4');
    }
  }
  else
  {
    Serial.print('0');
  }
}
