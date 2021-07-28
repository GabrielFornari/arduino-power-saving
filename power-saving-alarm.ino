


#include <avr/sleep.h>
#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include "LowPower.h"

#define SQW_PIN 2   // connect this pin to DS3231 INT/SQW pin
#define PW_PIN 8



void setup () 
{
  Serial.begin(9600);
  
  // set led built
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // turn on RTC
  pinMode(PW_PIN, OUTPUT);
  digitalWrite(PW_PIN, HIGH);

  // configure an interrupt on the falling edge from the SQW pin
  pinMode(SQW_PIN, INPUT_PULLUP);
  
  initializeRTC();
  

  Serial.println("Starting...");
  
  //delay(1000);
  //digitalWrite(PW_PIN, LOW);
}

void loop()
{ 
  
  //digitalClockDisplay();
  //Serial.flush();
  //delay(1000);

  blink();

  
  RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 1);  // daydate parameter should be between 1 and 7
  RTC.alarm(ALARM_2);                   // ensure RTC interrupt flag is cleared
  RTC.alarmInterrupt(ALARM_2, true);

  //digitalWrite(PW_PIN, LOW);
  //delay(1000);
  
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
  
  attachInterrupt(digitalPinToInterrupt(SQW_PIN), wakeUp, FALLING);
  
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



void blink()
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
  delay(500);
}

void digitalClockDisplay()
{
    time_t rtcTime = RTC.get();
    
    Serial.print(hour(rtcTime));
    printDigits(minute(rtcTime));
    printDigits(second(rtcTime));
    Serial.print(' ');
    Serial.print(day(rtcTime));
    Serial.print(' ');
    Serial.print(month(rtcTime));
    Serial.print(' ');
    Serial.print(year(rtcTime));
    Serial.println();
}
void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(":");
    if(digits < 10)
        Serial.print("0");
    Serial.print(digits);
}


//-------------------------------------------------------------------------------
// Description here!
//-------------------------------------------------------------------------------
void initializeRTC()
{
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);

  // set Alarm 1 to occur once per second
  //RTC.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0, 0);
  // clear the alarm flag
  //RTC.alarm(ALARM_1);
  
}
