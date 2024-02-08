#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int stopWatch_init = 0;
const int stopWatch_run = 1;
const int stopWatch_result = 2;
const int lcd_ON = 3;
const int lcd_OFF = 4;
const int countDown_init = 5;
const int countDown_run = 6;
int countDownSecond;
int state;

boolean lcdStatus = true;
boolean previousState = 0;

unsigned long startTime = 0;   // Variable to store the start time
unsigned long elapsedTime = 0; // Variable to store the elapsed time
boolean buttonRelease = true;
boolean buttonRelease_1 = true;
boolean arrived = false;
void displayTime(unsigned long time);

void setup()
{
  Serial.begin(115200);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  lcd.init(); // initialize the lcd
  lcd.backlight();
  state = stopWatch_init;
}

void loop()
{
  if (state == stopWatch_init)
  {
    previousState = 0;
    startTime = millis();
    lcd.setCursor(0, 0);
    lcd.print("=====Timer======");
    lcd.setCursor(4, 1);
    lcd.print("0:00:000");
    digitalWrite(D5, HIGH);
    if (digitalRead(D3) == 0 && buttonRelease == true)
    {
      digitalWrite(D7, HIGH);
      digitalWrite(D5, LOW);
      state = stopWatch_run;
      buttonRelease = false;
    }
    else if (digitalRead(D4) == 0 && buttonRelease_1 == true)
    {
      if (lcdStatus == true)
      {
        unsigned long pressTime = millis();
        while (digitalRead(D4) == 0)
        {
          if (millis() - pressTime >= 2000)
          {
            buttonRelease_1 = false;
            state = countDown_init;
            lcd.clear();
            return;
          }
          delay(10);
        }
        state = lcd_OFF;
      }
      else if (lcdStatus == false)
      {
        unsigned long pressTime = millis();
        while (digitalRead(D4) == 0)
        {
          if (millis() - pressTime >= 2000)
          {
            buttonRelease_1 = false;
            state = countDown_init;
            lcd.clear();
            return;
          }
          delay(10);
        }
        state = lcd_ON;
      }
    }
    else if (digitalRead(D3) == 1)
    {
      buttonRelease = true;
    }
    if (digitalRead(D4) == 1)
    {
      buttonRelease_1 = true;
    }
  }
  else if (state == stopWatch_run)
  {
    displayTime(millis() - startTime);
    if (digitalRead(D3) == 0 && buttonRelease == true)
    {
      digitalWrite(D7, LOW);
      digitalWrite(D6, HIGH);
      state = stopWatch_result;
      elapsedTime = millis() - startTime;

      buttonRelease = false;
    }
    else if (digitalRead(D3) == 1)
    {
      buttonRelease = true;
    }
  }
  else if (state == stopWatch_result)
  {
    if (digitalRead(D3) == 0 && buttonRelease == true)
    {
      startTime = millis() - elapsedTime;
      unsigned long pressTime = millis();
      while (digitalRead(D3) == 0)
      {
        if (millis() - pressTime >= 1000)
        {
          digitalWrite(D6, LOW);
          state = stopWatch_init;
          buttonRelease = false;
          return;
        }
        delay(10);
      }
      digitalWrite(D7, HIGH);
      digitalWrite(D6, LOW);
      state = stopWatch_run;
      buttonRelease = false;
    }
    else if (digitalRead(D3) == 1)
    {
      buttonRelease = true;
    }
  }
  else if (state == lcd_ON)
  {
    lcd.backlight();
    lcdStatus = true;
    buttonRelease_1 = false;
    if (digitalRead(D4) == 1 && previousState == 0)
    {
      state = stopWatch_init;
      buttonRelease_1 = true;
    }
    else if (digitalRead(D4) == 1 && previousState == 1)
    {
      state = countDown_init;
      buttonRelease_1 = true;
    }
  }
  else if (state == lcd_OFF)
  {
    lcd.noBacklight();
    lcdStatus = false;
    buttonRelease_1 = false;
    if (digitalRead(D4) == 1 && previousState == 0)
    {
      state = stopWatch_init;
      buttonRelease_1 = true;
    }
    else if (digitalRead(D4) == 1 && previousState == 1)
    {
      state = countDown_init;
      buttonRelease_1 = true;
    }
  }
  else if (state == countDown_init)
  {
    lcd.setCursor(0, 0);
    lcd.print("SetCountDownTime");
    lcd.setCursor(7, 1);
    lcd.print(countDownSecond);
    previousState = 1;
    if (digitalRead(D3) == 0 && buttonRelease == true)
    {
      unsigned long pressTime = millis();
      while (digitalRead(D3) == 0)
      {
        if (millis() - pressTime >= 1000)
        {
          pressTime = millis();
          arrived = false;
          state = countDown_run;
          return;
        }
        delay(10);
      }
      // lcd.setCursor(0,0);
      countDownSecond += 1;
      lcd.setCursor(7, 1);
      lcd.print(countDownSecond);
    }
    else if (digitalRead(D4) == 0 && buttonRelease_1 == true)
    {
      if (lcdStatus == true)
      {
        unsigned long pressTime = millis();
        while (digitalRead(D4) == 0)
        {
          if (millis() - pressTime >= 2000)
          {
            buttonRelease_1 = false;
            state = stopWatch_init;
            lcd.clear();
            countDownSecond = 0;
            return;
          }
          delay(10);
        }
        state = lcd_OFF;
      }
      else if (lcdStatus == false)
      {
        unsigned long pressTime = millis();
        while (digitalRead(D4) == 0)
        {
          if (millis() - pressTime >= 2000)
          {
            buttonRelease_1 = false;
            state = stopWatch_init;
            lcd.clear();
            countDownSecond = 0;
            return;
          }
          delay(10);
        }
        state = lcd_ON;
      }
    }
    if (digitalRead(D4) == 1)
    {
      buttonRelease_1 = true;
    }
    if (digitalRead(D3) == 1)
    {
      buttonRelease = true;
    }
  }
  else if (state == countDown_run)
  {
    unsigned long r = millis();
    while (buttonRelease == true)
    {
      if (digitalRead(D3) == 0 && countDownSecond > 0 && arrived == true)
      {
        lcd.setCursor(0, 0);
        lcd.print(" PauseCountDown ");
        buttonRelease = false;
      }
      else if (millis() - r >= 1000)
      {
        r = millis();
        if (countDownSecond > 0)
        {
          countDownSecond--;
          lcd.setCursor(0, 0);
          lcd.print(" CountDownTime  ");
          lcd.setCursor(7, 1);
          lcd.print(countDownSecond);
        }
        else if (countDownSecond == 0)
        {
          lcd.setCursor(11, 1);
          lcd.print("Done");
          delay(200);
          lcd.clear();
          state = countDown_init;
          return;
        }
      }
      else if (digitalRead(D3) == 1)
      {
        buttonRelease = true;
        arrived = true;
      }
      delay(10);
    }
    if (digitalRead(D3) == 0 && buttonRelease == false && arrived == false)
    {
      buttonRelease = true;
    }
    else if (digitalRead(D3) == 1)
    {
      arrived = false;
    }
  }
}

void displayTime(unsigned long time)
{
  unsigned long milliseconds = time % 1000;    // Calculate milliseconds
  unsigned long seconds = (time / 1000) % 60;  // Calculate seconds
  unsigned long minutes = (time / 60000) % 60; // Calculate minutes

  lcd.setCursor(4, 1);

  // Print the elapsed time in the format "min:sec:millis"
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10)
  {
    lcd.print('0'); // Add leading zero for single-digit seconds
  }
  lcd.print(seconds);
  lcd.print(":");
  if (milliseconds < 100)
  {
    lcd.print('0'); // Add leading zero for two-digit milliseconds
  }
  if (milliseconds < 10)
  {
    lcd.print('0'); // Add leading zero for single-digit milliseconds
  }
  lcd.print(milliseconds);
}
