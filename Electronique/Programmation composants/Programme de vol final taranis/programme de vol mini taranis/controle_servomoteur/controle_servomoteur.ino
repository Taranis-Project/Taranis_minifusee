 /* Inclut la lib Servo pour manipuler le servomoteur */
#include <Servo.h>
/* Créer un objet Servo pour contrôler le servomoteur */
Servo Gouverne_profondeur;
Servo Ailerons;
Servo Lacet;
int Aileron = 90;
int Gouverne = 90;
int Lacett = 90;
int DAlt;
int DRoulis;
int DLacet;
void setup() {
  // Attache le servomoteur à la broche D9
  Gouverne_profondeur.attach(2);
  Ailerons.attach(3);
  Lacet.attach(4);
  Serial.begin(9600);
}
void loop() {
  
  if (DAlt > 0 && Gouverne <= 180){
    Gouverne = Gouverne + 1;
  } else if (DAlt < 0 && Gouverne >= 0){
    Gouverne = Gouverne-1;
  }
  Gouverne_profondeur.write(Gouverne);
  Serial.print(Gouverne);
  if ( DRoulis > 0 && Aileron <= 180){
    Aileron = Aileron+1;
  } else if (DRoulis < 0 && Aileron >= 0){
    Aileron = Aileron-1;
  }                                              
  Ailerons.write(Aileron);
  Serial.print(Aileron);
  if (DLacet > 0 && Lacett <= 180){
    Lacett = Lacett+1;
  } else if (DLacet < 0 && Lacett >= 0){
    Lacett = Lacett-1;
  }                                              
  Lacet.write(Lacett);
  Serial.print(Lacett);
}
