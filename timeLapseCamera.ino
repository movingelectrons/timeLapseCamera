//(C)2017 Jerome Stonebridge
#include <Keypad.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Time.h>
#include <TimeLib.h> // WE need this header
#include <Wire.h>
#include <DS1307RTC.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(12, 11, 10, 9, 8);
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#define shutterPin A0

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};

byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
tmElements_t tm;

void setup(){
  Serial.begin(9600); //!!must be disabled for the 3rd collum of keys to work
  pinMode(13, OUTPUT); 
  digitalWrite(13, HIGH);
  pinMode(shutterPin, OUTPUT); 
  digitalWrite(shutterPin, HIGH);  
  display.begin();
  display.setContrast(45);
  display.display(); // show splashscreen
  delay(750);
  display.clearDisplay();
  display.setRotation(2);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.clearDisplay();   // clears the screen and buffer
  display.setCursor(0,0);
  display.println("Total frames");
  display.setCursor(0,10);
  display.println("(max 32,767)");
  display.setCursor(0,40);
  display.println("then press *");
  display.display();

  bool parse=false;
  bool config=false;
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
  
  delay(250);
}
void(* resetFunc) (void) = 0; //declare reset function @ address 0

int input1i = 0;
int input1array[] = {-1, -1, -1, -1, -1};
int input1 = 0; //total frames
int input2i = 0;
int input2array[] = {-1, -1, -1, -1, -1};
int input2 = 0; //delay inbetween frames (in minutes)
bool startCapture = false;
char customKey;
char exitKey = 42; //*
int currentFrame = 1;

void loop(){
  customKey = customKeypad.getKey();
  if(startCapture == false){
    if (customKey && input1 == 0){
      if(customKey!=exitKey){
        input1array[input1i] = (int)customKey - 48;
        input1i = input1i+1;
        display.setCursor(input1i*6,20);
        display.print(customKey);
        display.display();
      }else if(customKey==exitKey){
        input1 = totalArray(input1array);
        display.clearDisplay();  
        display.setCursor(0,0);
        display.print("Frames ");
        display.print(input1);
        display.setCursor(0,10);
        display.println("Minutes delay");
        display.setCursor(0,20);
        display.println("(max 32,767)");
        display.setCursor(0,40);
        display.println("then press *");
        display.display();
      }
    }else if (customKey && input2 == 0 && input1 != 0){
      if(customKey!=exitKey){
        input2array[input2i] = (int)customKey - 48;
        input2i = input2i+1;
        display.setCursor(input2i*6,30);
        display.print(customKey);
        display.display();
      }else if(customKey==exitKey){
        input2 = totalArray(input2array);
        display.clearDisplay();  
        display.setCursor(0,0);
        display.print("Frames ");
        display.print(input1);
        display.setCursor(0,10);
        display.print("Delay ");
        display.print(input2);
        display.setCursor(0,30);
        display.println("Press * to");
        display.setCursor(0,40);
        display.println(" start!");
        display.display();
      }
    }else if (customKey && input2 != 0 && input1 != 0){
      if(customKey==exitKey){
        startCapture = true;
      }
    }
  }else if(currentFrame <= input1){
    display.clearDisplay();  
    display.setCursor(0,0);
    display.print("Frames ");
    display.print(input1);
    display.setCursor(0,10);
    display.print("Delay ");
    display.print(input2);
    display.setCursor(0,30);
    display.print("On frame ");
    display.println(currentFrame);
    display.display();
      
    digitalWrite(shutterPin, LOW);   
    delay(50);                  
    digitalWrite(shutterPin, HIGH);  
      
    currentFrame++;
    if(currentFrame <= input1){
      unsigned long startMillis = millis();
      while (millis() - startMillis < (input2*1000));
    }
  }else{ //done capturing frames
    if(customKey==exitKey){
      resetFunc();  //call reset
      delay(100);
    }
    display.clearDisplay();  
    display.setCursor(0,0);
    display.print("Frames ");
    display.print(input1);
    display.setCursor(0,10);
    display.print("Delay ");
    display.print(input2);
    display.setCursor(0,40);
    display.print("Hold * to reset");
    display.display();
    delay(500);
    completedScreenBlink();
    delay(500);
  }
}

int totalArray(int mArray[]){
  int i0, i1, i2, i3, i4 = -1;
  for(int i = 0; i < 5; i++){
    Serial.println(mArray[i]);
    if(i == 0){
      i0 = mArray[i];
    }else if(i == 1){
      i1 = mArray[i];
    }else if(i == 2){
      i2 = mArray[i];
    }else if(i == 3){
      i3 = mArray[i];
    }else if(i == 4){
      i4 = mArray[i];
    }
  }
  Serial.println(i0);
  Serial.println(i1);
  Serial.println(i2);
  Serial.println(i3);
  Serial.println(i4);
  if((i0==-1)&&(i1==-1)&&(i2==-1)&&(i3==-1)&&(i4==-1)){
    //throw error
  }else if((i1==-1)&&(i2==-1)&&(i3==-1)&&(i4==-1)){
    return i0; //only one digit was entered, so return it
  }else if((i2==-1)&&(i3==-1)&&(i4==-1)){
    int retval = (i0*10) + i1;
    return retval;
  }else if((i3==-1)&&(i4==-1)){
    int retval = (i0*100) + (i1*10) + i2;
    return retval;
  }else if(i4==-1){
    int retval = (i0*1000) + (i1*100) + (i2*10) + i3;
    return retval;
  }else{
    int retval = (i0*10000) + (i1*1000) + (i2*100) + (i3*10) + i4;
    return retval;
  }
}

//to give appearance that COMPLETED is blinking..
void completedScreenBlink(){
  display.clearDisplay();  
  display.setCursor(0,0);
  display.print("Frames ");
  display.print(input1);
  display.setCursor(0,10);
  display.print("Delay ");
  display.print(input2);
  display.setCursor(0,30);
  display.print("COMPLETED");
  display.setCursor(0,40);
  display.print("Hold * to reset");
  display.display();
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
