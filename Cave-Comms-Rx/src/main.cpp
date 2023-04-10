#include <Arduino.h>

#define RX_PIN PB5 // receive pin

// Decoding constants
#define BIT_PERIOD 20100
#define OFF_TIME 800000
#define BUFFER_TIME 1000
#define BYTE_LEN 8

// decode incoming message
void decode() {
  int start_time = micros();

  int curr_time = start_time;
  int prev_time = curr_time;
  int end = 0; // end marker after 2 bytes of consecutive zeros
  int prev_state = 1;
  int curr_state = 1;
  int num_zeros = 0;
  String msg = "";
  
  // Reads message until 2 bytes full of zeros passes
  while (!end) {
    // delay(1);
    curr_state = digitalRead(RX_PIN);
    if (!prev_state && curr_state && (micros() - curr_time) > BUFFER_TIME) { // Rising edge detection
      // Determining time between rising edges (aka number of zero bits)
      prev_time = curr_time;
      curr_time = micros();
      num_zeros = (curr_time - prev_time)/BIT_PERIOD;
      
      // Appending new data to msg string
      for (int i = 0; i < num_zeros; i++) {
        msg = "0" + msg;
      }
      msg = "1" + msg;
      delay(1);
    } 

    // Checking if message has finished transmitting
    else if (!prev_state && !curr_state && (micros() - curr_time) > OFF_TIME ) {
      // Adding extra zeros 
      int extra_zeros = BYTE_LEN - msg.length() % BYTE_LEN;
      for (int i = 0; i < extra_zeros; i++) {
        msg = "0" + msg;
      }
      end = 1;
    }
    prev_state = curr_state;
  }
  // Serial.print(msg);
  // Serial.print("\n");
  // Now that bits are found, decode into ascii string
  String decoded_msg = "";

  // split string into different bytes and loop through them but backwards lol
  int num_bytes = msg.length() / BYTE_LEN;
  for (int i = 0; i < num_bytes; i++) {
    String byte = msg.substring(msg.length() - BYTE_LEN*(1+i), msg.length() - BYTE_LEN*(i));
    // Iterate through all the bits -> construct bit representation of character
    char character = 0B0;
    for (int i = 0; i < byte.length(); i++)  {
      character = character | (byte[i] - '0'); 
      if (i < byte.length() - 1) {
        character = character << 1;
      }
    }
    // Append new character to output string
    decoded_msg += character;
  }
  int end_time = micros();
  Serial.print(decoded_msg);
  Serial.print("\n");
  Serial.print("Message length: ");
  Serial.print(decoded_msg.length());
  Serial.print(" characters. \n");

  Serial.print("Communication time: ");
  Serial.print((end_time - start_time) / 1000000);
  Serial.print(" seconds. \n");

  Serial.print("Bit rate: ");
  Serial.print(decoded_msg.length()*8 / ((end_time - start_time) / 1000000));
  Serial.print(" bits/second. \n \n");

}

void setup() {
  pinMode(RX_PIN, INPUT);
  Serial1.begin(115200); 
}

void loop() {
  // Searching for start bit
  
  if (digitalRead(RX_PIN)) {
    decode();
  }
}