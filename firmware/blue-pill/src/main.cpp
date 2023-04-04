#include <Arduino.h>
#include <CMSIS_DSP.h>
#include <math.h>

#define AIN_PIN PA0	// analog read pin
#define AOUT_PIN PA1   // analog out pin
#define AOUT_PIN_PWM PA_1 // analog out pin for PWM (same pin but different
                      	// data type - this is confusing!)

// Variable to store the value of the analog in pin.
volatile uint16_t value = 0; //ADC is 12bit

// Variable to show the program is running
uint32_t loop_counter = 0;

/**
 * @brief Initialize the analog in and out pins. We will use the analog out
 *    	pin as PWM. Also initialize the OLED display.
 * @param none
 * @retval none
 */
void setup()
{
  //Initialize analog in pin
  pinMode(AIN_PIN, INPUT);
  pinMode(AOUT_PIN, OUTPUT);
}

/**
 * @brief Read the value of the analog-in pin and set the timing for the LED
 *    	blink period depending on this value.
 * @param none
 * @retval none
 */
void loop()
{
  //Output a PWM signal
  uint32_t frequency_Hz = 490000;
  uint32_t dutycycle = 50;
  
  pwm_start(AOUT_PIN_PWM, frequency_Hz, dutycycle,
        	TimerCompareFormat_t::PERCENT_COMPARE_FORMAT);
  //Read the PWM signal as analog in
  value = analogRead(AIN_PIN);

  delay(500);
}
