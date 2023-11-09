  #include "LoRa_E32.h"
  UART_BPS_RATE bpsRate = UART_BPS_RATE_19200;
  LoRa_E32 e32ttl100(2,3,4,bpsRate); // e32 TX e32 RX

  
unsigned long delai;

void setup() {
  pinMode(12,OUTPUT);
  digitalWrite(12,HIGH);
  e32ttl100.begin();
  Serial.begin(115200);
delai = millis;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long t=millis();
  char frame[59];
  snprintf(frame, sizeof(frame), "t%lup1834s32g-14z-14958h-27y444j-4x-16054c29\n",t);
  if (millis() - delai >= 70)
  {
  ResponseStatus rs = e32ttl100.sendMessage(frame);
  Serial.println(rs.getResponseDescription());
  delai = millis();
  }
  Serial.println(frame);
  delay(20);
  Serial.println(millis()-t);
}
