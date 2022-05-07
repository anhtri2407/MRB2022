#include<Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
TMRpcm audio;

int SD_ChipSelectPin = 10;

int setUpTime = 2700; //15 mins
int maxSetUpTime = 5400; //45 mins
int minSetUpTime = 0; //OverTime

int minute;
int second;

int decreaseButton = 2;
int increaseButton = 4;
int playButton = 7;
int resetButton = 8;
int countPlayButton = 0;

bool decreaseButtonPushed;
bool increaseButtonPushed;
bool playButtonPushed;
bool resetButtonPushed;

int interval = 1000;
unsigned long previousMillis = 0;
unsigned long decreaseSecond = 0;
unsigned long previousDecreaseSecond = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  audio.speakerPin = 9;
  audio.setVolume = 5;
  
  pinMode(decreaseButton, INPUT_PULLUP);
  pinMode(increaseButton, INPUT_PULLUP);
  pinMode(playButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);

  if (!SD.begin(SD_ChipSelectPin)){
    lcd.setCursor(5,0);
    lcd.print("SD Fail!");
    return;
  }
}
  

bool isOverTime(int setUpTime){
  if (setUpTime == 0) return true;
  return false;
}

void displayTime(){
  
  if (setUpTime >= minSetUpTime && setUpTime <= maxSetUpTime) {
  
  minute = setUpTime / 60;
  second = setUpTime % 60;
  
  if (minute < 10){
  lcd.setCursor(6,0);
  lcd.print('0');
  lcd.setCursor(7,0);
  lcd.print(minute);
  }

  else{
    lcd.setCursor(6,0);
    lcd.print(minute);
  }

  if (second < 10){
  lcd.setCursor(9,0);
  lcd.print('0');
  lcd.setCursor(10,0);
  lcd.print(second);
  }

  else{
    lcd.setCursor(9,0);
    lcd.print(second);
  }
  
 
  lcd.setCursor(8,0);
  lcd.print(':');
  }
}

void increaseFiveMinutes(bool increaseButtonPushed){
  if (!increaseButtonPushed && setUpTime <= maxSetUpTime) setUpTime += 300;
}


void decreaseFiveMinutes(bool decreaseButtonPushed){
  if (!decreaseButtonPushed && setUpTime > minSetUpTime) setUpTime -= 300;
}

void resetTime(bool resetButtonPushed){
  if (!resetButtonPushed){
    setUpTime = 2700; //45 mins
    countPlayButton = 0;
  }
}

void noticeFiveMinutesLeft(int setUpTime){
  if (setUpTime == 300){ //5 mins
    audio.play("notice_5_mins_left.wav";
  }
}

void loop() {
  decreaseButtonPushed = digitalRead(decreaseButton);
  increaseButtonPushed = digitalRead(increaseButton);
  playButtonPushed = digitalRead(playButton);
  resetButtonPushed = digitalRead(resetButton);


  if (playButtonPushed == 0) ++countPlayButton;

  if (countPlayButton > 0) {
  
  unsigned long timeNow = millis();

  if ((unsigned long) (timeNow - previousMillis) >= interval){
    ++decreaseSecond;
    previousMillis = timeNow;
    if (decreaseSecond - previousDecreaseSecond >= 1){
    setUpTime -= (decreaseSecond - (decreaseSecond - 1));
    displayTime();
    previousDecreaseSecond = decreaseSecond;
    }
  }
 }
  
  else{
    displayTime();
    }
  
  
  if (!isOverTime(setUpTime)){
    noticeFiveMinutesLeft(setUpTime);
    increaseFiveMinutes(increaseButtonPushed);
    decreaseFiveMinutes(decreaseButtonPushed);
    resetTime(resetButtonPushed);
    delay(80);
  }

  else {
    displayTime();
    int countdown = 10;
    audio.play("alarm.wav");
    while(countdown--){
      lcd.noBacklight();
      delay(100);
      lcd.backlight();
      delay(100);
      resetButtonPushed = digitalRead(resetButton);
      resetTime(resetButtonPushed);
    }
  }

 
}
