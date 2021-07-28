/*
  DS3231: Real-Time Clock. SQW & 32kHz Example
  Read more: www.jarzebski.pl/arduino/komponenty/zegar-czasu-rzeczywistego-rtc-ds3231.html
  GIT: https://github.com/jarzebski/Arduino-DS3231
  Web: http://www.jarzebski.pl
  (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <DS3232RTC.h>
#include <Streaming.h>  

boolean state;

long LastRTCMicro;  //variable to store the last RTC Micro time - to be used for looping
long RTCmicro;     //variable to store the RTC Micro time as soon as the Sqwave pin is driven low

const byte sqwPin = 10;    // the pin that the Sqwave attaches to
unsigned int sqwCounter = 0;   // counter for the number of cycles. 
bool sqwState = false;         // current state of the squarewave Pin either HIGH or LOW
bool lastSqwState = false;    // previous state so can see if its changed in the loop

void setup()
{
  Serial.begin(115200);

  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_4096_HZ);

  tmElements_t tm;
  tm.Hour = 00;           // set the RTC to an arbitrary time
  tm.Minute = 00;
  tm.Second = 00;
  tm.Day = 16;
  tm.Month = 9;
  tm.Year = 2017 - 1970;  // tmElements_t.Year is the offset from 1970
  RTC.write(tm);          // set the RTC from the tm structure
  
  delay(800);
  
  // set Alarm 1 to occur once per second
  RTC.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0, 0);
  // clear the alarm flag
  RTC.alarm(ALARM_1);
  
  Serial << millis() << " Start ";
  printDateTime(RTC.get());
  Serial << endl;
  
  
  pinMode(sqwPin, INPUT_PULLUP); //has to be a pullup - without 10mOhm resister arduino will not register the state change
}

void loop()
{
  chkPin();

  // check to see if the alarm flag is set (also resets the flag if set)
  // Nota: se esse trecho do código for comentado, o programa funciona muito bem (~0,5 seg usando a função micros)
  // /*
  if ( RTC.alarm(ALARM_1) )
  {
      Serial << millis() << " ALARM_1 ";
      printDateTime(RTC.get());
      Serial << endl;
      // Serial << sqwCounter;
      // Serial << endl;
  }
  // */
}

void chkPin()
{
  if(sqwCounter == 4095){
    sqwCounter = 0;
    RTCmicro = micros();
    Serial.println((RTCmicro-LastRTCMicro) / 1000) ;
    LastRTCMicro = RTCmicro; 
  }
  
  if(sqwCounter == 0)
    LastRTCMicro = micros();  //grabs the current micro 
    
  sqwState = digitalRead(sqwPin);
  if (sqwState != lastSqwState){
      sqwCounter++;
      lastSqwState = sqwState;
  }
}

void printDateTime(time_t t)
{
    Serial << ((day(t)<10) ? "0" : "") << _DEC(day(t));
    Serial << monthShortStr(month(t)) << _DEC(year(t)) << ' ';
    Serial << ((hour(t)<10) ? "0" : "") << _DEC(hour(t)) << ':';
    Serial << ((minute(t)<10) ? "0" : "") << _DEC(minute(t)) << ':';
    Serial << ((second(t)<10) ? "0" : "") << _DEC(second(t));
}
