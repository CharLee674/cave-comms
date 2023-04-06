#include <Arduino.h>
#include <CMSIS_DSP.h>
#include <math.h>

// Pin Definition
#define TX_PIN PA0

#define BIT_LEN 8

void transmit(String msg) {
  // Loop through characters of the message
  for (auto &c : msg){
    // print character (for monitoring purposes)
    Serial.print(c);
    Serial.print("\n"); 

    // Loop through each bit of the character & set the pin value
    for (int i = 0; i < BIT_LEN; i++) {
      // Set pin to bit value
      digitalWrite(TX_PIN, c & 0b00000001);
      Serial.print(c & 0b00000001, BIN);
      delayMicroseconds(100);
      // Set pin to low for pulse shut down time
      digitalWrite(TX_PIN, LOW);
      delay(5);
      // bit shift
      c = c >> 1;
    }
    Serial.print("\n"); 
  }
  delay(1000);
}

void setup() {
  pinMode(TX_PIN, OUTPUT);
  Serial1.begin(115200);
  // put your setup code here, to run once:
}

void loop() { 
  transmit("t");
}