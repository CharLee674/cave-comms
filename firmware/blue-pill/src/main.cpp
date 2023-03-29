#include <Arduino.h>
#include <CMSIS_DSP.h>
#include <math.h>

// HardwareTimer using STM32Duino API: https://github.com/stm32duino/Arduino_Core_STM32/wiki/HardwareTimer-library

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  //turn LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
}