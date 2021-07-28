


#include <avr/sleep.h>

#define SQW_PIN 2   // connect this pin to DS3231 INT/SQW pin


void setup () 
{
  Serial.begin(9600);

  // set led built
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // configure an interrupt on the falling edge from the SQW pin
  pinMode(SQW_PIN, INPUT_PULLUP);
  
  
  sleep();
}


void loop()
{

  // blink
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  
  sleep();
  
}

void wakeUp()
{
  sleep_disable();
  detachInterrupt(0);
}


void sleep() 
{
  // turn off analog pins
  for (byte iPin = A0; iPin <= A5; iPin++)
  {
    pinMode(iPin, OUTPUT);
    digitalWrite(iPin, LOW);
  }
  
  // disable ADC
  ADCSRA = 0;  
  
  noInterrupts();           // timed sequence follows
  sleep_enable();
  
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, FALLING);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  
  // disabe BOD before going to sleep
  cli();
  sleep_bod_disable();
  sei();
  
  interrupts();             // guarantees next instruction executed
  sleep_cpu();              // sleep within 3 clock cycles of above
}
