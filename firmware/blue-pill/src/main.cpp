#include <Arduino.h>
#include <CMSIS_DSP.h>
#include <math.h>


// HardwareTimer using STM32Duino API: https://github.com/stm32duino/Arduino_Core_STM32/wiki/HardwareTimer-library

#define AOUT_PIN PA1   // analog out pin
#define BOOSTRAP_FREQ 490000 // 490 kHz
#define DUTY_CYCLE 50

#define GATE_OUT_PIN1 PA6
#define GATE_OUT_PIN2 PA7
#define GATE_FREQ 200000 // 200 kHz
#define GATE_SD_PIN PB0
#define TOGGLE_FREQ 5000 // 10 kHz

#define SERIAL_PIN1 PB10
#define SERIAL_PIN2 PA2

void pwm_start_inverted(PinName pin, uint32_t PWM_freq, uint32_t value, TimerCompareFormat_t resolution)
{
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
  HardwareTimer *HT;
  TimerModes_t previousMode;
  uint32_t index = get_timer_index(Instance);
  if (HardwareTimer_Handle[index] == NULL) {
    HardwareTimer_Handle[index]->__this = new HardwareTimer((TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM));
  }

  HT = (HardwareTimer *)(HardwareTimer_Handle[index]->__this);

  uint32_t channel = STM_PIN_CHANNEL(pinmap_function(pin, PinMap_TIM));

  previousMode = HT->getMode(channel);
  if (previousMode != TIMER_OUTPUT_COMPARE_PWM2) {
    HT->setMode(channel, TIMER_OUTPUT_COMPARE_PWM2, pin);
  }
  HT->setOverflow(PWM_freq, HERTZ_FORMAT);
  HT->setCaptureCompare(channel, value, resolution);
  if (previousMode != TIMER_OUTPUT_COMPARE_PWM2) {
    HT->resume();
  }
}

void toggle(PinName pin, uint32_t freq)
{
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
  HardwareTimer *HT;
  TimerModes_t previousMode;
  uint32_t index = get_timer_index(Instance);
  if (HardwareTimer_Handle[index] == NULL) {
    HardwareTimer_Handle[index]->__this = new HardwareTimer((TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM));
  }

  HT = (HardwareTimer *)(HardwareTimer_Handle[index]->__this);

  uint32_t channel = STM_PIN_CHANNEL(pinmap_function(pin, PinMap_TIM));

  previousMode = HT->getMode(channel);
  if (previousMode != TIMER_OUTPUT_COMPARE_TOGGLE) {
    HT->setMode(channel, TIMER_OUTPUT_COMPARE_TOGGLE, pin);
  }
  HT->setOverflow(freq, HERTZ_FORMAT);
  if (previousMode != TIMER_OUTPUT_COMPARE_TOGGLE) {
    HT->resume();
  }
}

void timer_stop(PinName pin){
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
  HardwareTimer *HT;
  uint32_t index = get_timer_index(Instance);
  if (HardwareTimer_Handle[index] == NULL) {
    HardwareTimer_Handle[index]->__this = new HardwareTimer((TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM));
  }

  HT = (HardwareTimer *)(HardwareTimer_Handle[index]->__this);
  if (HT != NULL) {
    delete (HT);
    HT = NULL;
  }
}

HardwareSerial transmitter(SERIAL_PIN1);
HardwareSerial receiver(SERIAL_PIN2);

static uint32_t nbTestOK = 0;
static uint32_t nbTestKO = 0;
void test_uart(int val)
{
  int recval = -1;
  uint32_t error = 0;

  receiver.write(val);
  delay(10);
  while (transmitter.available()) {
    recval = transmitter.read();
  }
  /* Enable receiver to RX*/
  receiver.enableHalfDuplexRx();
  if (val == recval) {
    transmitter.write(val);
    delay(10);

    while (receiver.available()) {
      recval = receiver.read();
    }
    /* Enable transmitter to RX*/
    transmitter.enableHalfDuplexRx();
    if (val == recval) {
      nbTestOK++;
      Serial.print("Exchange: 0x");
      Serial.println(recval, HEX);
    } else {
      error = 2;
    }
  }
  else {
    error = 1;
  }
  if (error) {
    Serial.print("Send: 0x");
    Serial.print(val, HEX);
    Serial.print("\tReceived: 0x");
    Serial.print(recval, HEX);
    Serial.print(" --> KO <--");
    Serial.println(error);
    nbTestKO++;
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  //turn LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(AOUT_PIN, OUTPUT);
  digitalWrite(GATE_SD_PIN, HIGH);
  Serial1.begin(115200); 

  // Start PWM on bootstrap pin
  pwm_start(digitalPinToPinName(AOUT_PIN), BOOSTRAP_FREQ, DUTY_CYCLE,
        	TimerCompareFormat_t::PERCENT_COMPARE_FORMAT);

  // Start PWM on gate pin
  pwm_start(digitalPinToPinName(GATE_OUT_PIN1), GATE_FREQ, 118,
        	TimerCompareFormat_t::RESOLUTION_8B_COMPARE_FORMAT);
  pwm_start_inverted(digitalPinToPinName(GATE_OUT_PIN2), GATE_FREQ, 136,
        	TimerCompareFormat_t::RESOLUTION_8B_COMPARE_FORMAT);

}

void loop() {
  // put your main code here, to run repeatedly:
  //Output a PWM signal
  Serial1.println("Serial Half-Duplex test started...");

  for (uint32_t i = 0; i <= (0xFF); i++) {
    test_uart(i);
  }

  Serial1.println("Serial Half-Duplex test done.\nResults:");
  Serial1.print("OK: ");
  Serial1.println(nbTestOK);
  Serial1.print("KO: ");
  Serial1.println(nbTestKO);
  Serial1.println("Rebooting in 5s...");

  delay(5000);
}
