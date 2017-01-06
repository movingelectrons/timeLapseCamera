//Jerome Stonebridge
#include <Keypad.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

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

void setup(){
  //Serial.begin(9600); //must be disabled for the 3rd collum of keys to work
  pinMode(shutterPin, OUTPUT); 
  digitalWrite(shutterPin, LOW);  
  display.begin();
  display.setContrast(60);
  display.display(); // show splashscreen
  delay(750);
  display.clearDisplay();   // clears the screen and buffer
  display.setRotation(2);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Total frames");
  display.setCursor(0,10);
  display.println("(max 32,767)");
  display.setCursor(0,40);
  display.println("then press *");
  display.display();
  delay(250);
}

int input1i = 0;
int input1array[] = {0, 0, 0, 0, 0};
int input1 = 0;
int input2i = 0;
int input2array[] = {0, 0, 0, 0, 0};
int input2 = 0;
bool startCapture = false;
char customKey;
char exitKey = 42; //*
int currentFrame = 0;

void loop(){
  if(startCapture == false){
  customKey = customKeypad.getKey();
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
      display.println("Seconds delay");
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
  }else if(currentFrame < input1){
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
      
      digitalWrite(shutterPin, HIGH);   
      delay(50);                  
      digitalWrite(shutterPin, LOW);  
      
      currentFrame++;
      unsigned long startMillis = millis();
      while (millis() - startMillis < (input2*1000)); //TODO put in real delay..
  }else{
      display.clearDisplay();  
      display.setCursor(0,0);
      display.print("Frames ");
      display.print(input1);
      display.setCursor(0,10);
      display.print("Delay ");
      display.print(input2);
      display.display();
      delay(500);
      completedScreenBlink();
      delay(500);
  }
}

int totalArray(int mArray[]){
  int i0, i1, i2, i3, i4 = 0;
  for(int i = 0; i < 5; i++){
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
  if(i0 == 0){
    //? throw error
  }else if((i1==0)&&(i2==0)&&(i3==0)&&(i4==0)){
    return i0;
  }else if((i2==0)&&(i3==0)&&(i4==0)){
    int retval = (i0*10) + i1;
    return retval;
  }else if((i3==0)&&(i4==0)){
    int retval = (i0*100) + (i1*10) + i2;
    return retval;
  }else if(i4==0){
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
  display.display();
}

