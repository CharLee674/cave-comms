#include <Arduino.h>

#define RX_PIN PB5 // receive pin

// Decoding constants
#define BIT_PERIOD 5100
#define OFF_TIME 40000
#define BYTE_LEN 8

// decode incoming message
void decode() {

  int curr_time = micros();
  int prev_time = curr_time;
  int end = 0; // end marker after 2 bytes of consecutive zeros
  int prev_state = 1;
  int curr_state = 1;
  int num_zeros = 0;
  String msg = "";
  
  // Reads message until 2 bytes full of zeros passes
  while (!end) {
    curr_state = digitalRead(RX_PIN);
    if (!prev_state && curr_state) { // Rising edge detection
      // Determining time between rising edges (aka number of zero bits)
      prev_time = curr_time;
      curr_time = micros();
      num_zeros = (curr_time - prev_time)/BIT_PERIOD;
      // Appending new data to msg string
      for (int i = 0; i < num_zeros; i++) {
        msg = "0" + msg;
      }
      msg = "1" + msg;
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
  Serial.print(decoded_msg);
  Serial.print("\n");
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