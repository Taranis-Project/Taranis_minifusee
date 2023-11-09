#include <Servo.h> //bibliothéque servo (déjà présente sur arduino)
Servo Servomoteur; //créer un objet servo
const int Pin_Servo = 11;
const int Position_initiale=180;
const int Position_finale=120;

void setup() {
  Servomoteur.attach(Pin_Servo);   // Attache le servomoteur au PinServo
  Servomoteur.write(Position_initiale);          // Met le servomoteur en position
  delay(1000);
  Servomoteur.write(Position_finale); 
}

void loop() {}
