#include <LoRa_E32.h>
/*
 * LoRa E32-TTL-100
 * Write on serial to transfer a message to other device
 * https://www.mischianti.org
 *
 * E32-TTL-100----- Arduino UNO
 * M0         ----- GND
 * M1         ----- GND
 * TX         ----- PIN 2 (PullUP)
 * RX         ----- PIN 3 (PullUP & Voltage divider)
 * AUX        ----- Not connected
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */

#include "Arduino.h"
#include "LoRa_E32.h"

UART_BPS_RATE bpsRate = UART_BPS_RATE_9600;
  LoRa_E32 e32ttl100(2,3,bpsRate); // e32 TX e32 RX
void setup() {
  Serial.begin(115200);

  e32ttl100.begin();
}
 
void loop() {
  //Serial.println(millis());
  ResponseContainer rc = e32ttl100.receiveMessageUntil('\n');
  Serial.println(rc.data);
} 
