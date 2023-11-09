/*****************************************/
#include <Servo.h> //bibliothéque servo (déjà présente sur arduino)
Servo Servomoteur; //créer un objet servo
const int PinLEDVERTE= 6;
const int PinINJack = 9;
const int PinINdeclenchement = 5;
const int PinOUTCom1 = 8;
const int PinOUTCom2 = 4;
unsigned long T1=10; //temps avant plage d'ouverture para en sec
unsigned long T2=20; //temps ouverture para obligatoire en sec
unsigned long T1milis;
unsigned long T2milis;
unsigned long T1millis;
unsigned long T2millis;
unsigned long T0;
int Ouverture(){
  for(int position = 90; position <= 360; position++){
    Servomoteur.write(position);
    delay(15);
  }
  while(1);
}
void setup()
{
  // Attache le servomoteur à la broche D13
  Servomoteur.attach(11);
  for (int position = 0; position <= 90; position++) {
    Servomoteur.write(position);
    delay(15);
  }
  T1milis=T1*1000;
  T2milis=T2*1000;
  Serial.begin(2000000);
  pinMode(PinLEDVERTE,OUTPUT);
  pinMode(PinINJack,INPUT);//set as INPUT
  pinMode(PinINdeclenchement,INPUT);
  pinMode(PinOUTCom1,OUTPUT);
  pinMode(PinOUTCom2,OUTPUT);
  digitalWrite(PinINJack, HIGH);//set as LOW
  digitalWrite(PinINdeclenchement, LOW);
  Serial.println(F("Attente du départ"));
  digitalWrite(PinLEDVERTE, HIGH);
}
/******************************************/
void loop() 
{
  int digitalVal = digitalRead(PinINJack);//Read the value of pin2
  if(LOW == digitalVal)//if tilt switch is not breakover
  {
    digitalWrite(PinLEDVERTE, LOW);
    T0 = millis();// prend T0
    digitalWrite(PinOUTCom1, LOW);//turn the led on
    digitalWrite(PinOUTCom2, HIGH);
    Serial.println(T1milis);
    Serial.println(T2milis);
    T1millis = T1milis+T0;
    T2millis = T2milis+T0;
    Serial.println("départ");
    while(millis() <= T1millis){
      Serial.println(millis()-T0);
      Serial.println(digitalRead(PinINdeclenchement));
    }
    while(millis() <= T2millis){
      Serial.println(F("T1"));
      Serial.println(F("waiting T2"));
      Serial.println(millis()-T0);
      Serial.println(digitalRead(PinINdeclenchement));
      if(HIGH == digitalRead(PinINdeclenchement)){
        digitalWrite(PinOUTCom1, HIGH);
        digitalWrite(PinOUTCom2, LOW);
        Serial.println(F("Ouverture Auto"));
        Ouverture();
      }
    }
    digitalWrite(PinOUTCom1, HIGH);
    digitalWrite(PinOUTCom2, HIGH);
    Serial.println(F("T2 Ouverture temps"));
    Ouverture();
  }
  else ////if tilt switch breakover
  {
    digitalWrite(PinOUTCom1, LOW);
    digitalWrite(PinOUTCom2, LOW);
  }
}
/**********************************************/
