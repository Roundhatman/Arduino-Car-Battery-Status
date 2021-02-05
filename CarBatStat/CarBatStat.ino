/*
 ############################################################
 ##                                                        ##
 ##   Battery status check v1.0                            ##
 ##   Written by L.Swarnajith                              ##
 ##   26-03-2020, local time 2150, Alubomulla, Sri Lanka   ##
 ##                                                        ##
 ############################################################
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <JC_Button.h>
#include <Adafruit_INA219.h>

#define Speaker 12

float Low_BAT_level = 6.0, BAT_Voltage = 0, current_mA = 0;
unsigned long Capacity = 0;
byte Hour = 0, Minute = 0, Second = 0;
bool calc = false, Done = false;
 
LiquidCrystal_I2C lcd(0x27,20,4);
Adafruit_INA219 ina219;
Button UP_Button(10, 25, false, true);
 
void setup() {
	
  pinMode(Speaker,OUTPUT);
  
  Wire.begin();
  UP_Button.begin();
  ina219.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.begin(16, 2);
  lcd.print("Battery Capacity");
  lcd.setCursor(0, 1);
  lcd.print("Swarnajith v1.0");
  delay(3000);
  
  lcd.clear();
  lcd.print("Set current");
  lcd.setCursor(0, 1);
  lcd.print("0");
 
}
 
void loop() {
  UP_Button.read();
 
  if (UP_Button.pressedFor(1000) && calc == false)
  {
    digitalWrite(Speaker, HIGH);
    delay(100);
    digitalWrite(Speaker, LOW);
    lcd.clear();
    timerInterrupt();
  }
  else{
    current_mA = GetCurrent();
    lcd.setCursor(0,1);
    lcd.print(String(current_mA) + "mA");
  }
 
}
 
void timerInterrupt() {
  calc = true;
  while (Done == false) {
    Second ++;
    if (Second == 60) {
      Second = 0;
      Minute ++;
      lcd.clear();
    }
    if (Minute == 60) {
      Minute = 0;
      Hour ++;
    }
    lcd.setCursor(0, 0);
    lcd.print(String(Hour) + ":" + String(Minute) + ":" + String(Second));
    lcd.setCursor(9, 0);
    BAT_Voltage = GetVoltage();
    lcd.print("V:" + String(BAT_Voltage));
    lcd.setCursor(0, 1);
    lcd.print("BAT-C: Wait!...");
 
    if (BAT_Voltage < Low_BAT_level)
    {
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      Capacity =  (Hour * 3600) + (Minute * 60) + Second;
      lcd.print("BAT-C:" + String(Capacity) + "mAh");
      Done = true;
      
      digitalWrite(Speaker, HIGH);
      delay(100);
      digitalWrite(Speaker, LOW);
      delay(100);
      digitalWrite(Speaker, HIGH);
      delay(100);
      digitalWrite(Speaker, LOW);
      
      while(1){}
    }
  }
}

int16_t GetCurrent(){
  uint16_t value;
  Wire.beginTransmission(0x40);
    Wire.write(0x05);
    Wire.write((4096 >> 8) & 0xFF);
    Wire.write(4096 & 0xFF);
    Wire.write(0x04);
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(0x40,(float)2);
  value = ((Wire.read() << 8) | Wire.read());

  return (int16_t)value;
}

int16_t GetVoltage(){
  uint16_t value;
  Wire.beginTransmission(0x40);
    Wire.write(0x02);
  Wire.endTransmission(0x40);
  delay(1);
  Wire.requestFrom(0x40,(float)2);
  value = ((Wire.read() << 8) | Wire.read());

  return (int16_t)((value >> 3) * 4);
}
