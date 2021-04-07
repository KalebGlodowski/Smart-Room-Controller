/*
 *  Project:      Smart Room Controller Midterm
 *  Description:  See readme for overview.
 *  Authors:      Kaleb Glodowski
 *  Date:         06-APR-2021
 */
// *** All included headers below ***
#include <SPI.h>              //header file that helps with many aspects
#include <mac.h>              //used with ethernet
#include <Ethernet.h>         //ethernet
#include <hue.h>              //for HUE lights | custom header from Brian
#include <Adafruit_GFX.h>     //for OLED
#include <Adafruit_SSD1306.h> //for OLED
#include <OneButton.h>        //button
#include <Encoder.h>          //for turning encoder
#include <Wire.h>             //for BME
#include <Adafruit_Sensor.h>  //for BME
#include <Adafruit_BME280.h>  //for BME
#include <Adafruit_NeoPixel.h>//for NexPixels
#include "colors.h"           //for NexPixels | custom colors from Brian
#include <Keypad.h>           //for Keypad
#include <PWMServo.h>         //for Keypad
#include "wemo.h"             //for Wemo

// *** Declaring all constants below ***

const int encoderPin_A = 14;      //encoder Pin A
const int encoderPin_B = 15;      //encoder Pin B
const int encoderPin_Switch = 21; //encoder Pin Switch 
const int encoderPin_G = 22;      //encoder Pin Green
const int encoderPin_R = 23;      //encoder Pin Red

const int pixel_Pin = 17;         //NeoPixels Pin
const int pixel_Count = 16;       //NeoPixels, total amount of NeoPixels

const int  keypad_Rows = 4;       //Keypad amount of rows
const int  keypad_Cols = 4;       //Keypad amount of columns
const char keypad_PassCode = 9496;//Keypad passcode to gain access
const int  keypad_PassLength = 4; //Keypad passcode character length

const int screen_Width = 128;     //OLED display width, in pixels
const int screen_Height = 64;     //OLED display height, in pixels
const int OLED_Reset = -1;        //OLED reset to synchronize with teensy reset
const int OLED_Address = 0x3C;    //OLED| this is OLED address

const int BME_Address = 0x76;     //BME | this is BME address
const char degree = 248;          //BME | degree symbol char

const int ultrasonicPin_Ping = 8; //Ultrasonic Pin OUT
const int ultrasonicPin_Echo = 16;//Ultrasonic Pin IN

const int  selectWemo = 3;        //selects the 3rd Wemo within the Wemo header file. This is the only one I have.

const int potentiometerPin = 20;  //Potentiometer Pin IN

// *** Variables below ***

bool BMEstatus;                   //BME | true/false variable set to BME's start status whether it is on or not.
int   tempC;                      //BME | celsius temperature
float tempF;                      //BME | fahrenheit temperature converted from celsius
int tempTime;                     //BME | timestamp when displaying temperature
int lastTemp;                     //BME | last temperature reading

int currentTime;                  //set to millis(), this is the time the code has been running

int lastSonicTime;                //Ultrasonic | a timestamp given after going through the ultrasonic IF statement
long duration;                    //Ultrasonic | duration
long inches;                      //Ultrasonic | inches from sensor

bool wemoOn;                      //Wemo | Is the Wemo on?
int wemoOnTime;                   //Wemo | Timestamp when the Weemo was turned on

int potRead;                      //Potentiometer readings

int hueBright;                    //Hue | light brightness level converted from potentiometer read
int lastHueBright;                //Hue | last brightness
int hueColor;                     //Hue | selected hue color
int hueLight;                     //Hue | current hue light selected
bool hueStatus;                   //Hue | is hue light on?
int saveBright;                   //Hue | saved brightness for hueflash
int saveColor;                    //Hue | saved color for hueflash
int saveCurrentLight;             //Hue | saved current selected light for hueflash
int saveHueStatus;                //Hue | save hue on/off status
int i;                            //Hue | used in the hue for loop to light all 3 lights

// *** Defining objects for the header files below ***

OneButton button1 (encoderPin_Switch, true, true);                        //for Button
Encoder myEnc (encoderPin_A, encoderPin_B);                               //for Encoder
Adafruit_SSD1306 display(screen_Width, screen_Height, &Wire, OLED_Reset); //for OLED
Adafruit_NeoPixel pixel (pixel_Count, pixel_Pin, NEO_GRB + NEO_KHZ800);   //for NeoPixels
Adafruit_BME280 bme;                                                      //for BME
Wemo myWemo;                                                              //for Wemo

void setup() {

  pinMode (ultrasonicPin_Ping, OUTPUT);                     //Ultrasonic Sensor | sending a signal out
  pinMode (ultrasonicPin_Echo, INPUT);                      //Ultrasonic Sensor | recieving a signal back
  pinMode (encoderPin_R,OUTPUT);                            //Encoder Red LED   | outputting a command from the Teensy to the board
  pinMode (encoderPin_G,OUTPUT);                            //Encoder Green LED | ''
  pinMode (potentiometerPin, INPUT);                        //Potentiometer     | recieving a reading of the position of the potentiometer
  Serial.begin(9600);

  //Below are necessary includes to ensure Wemo works  
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
  button1.attachClick(oneClick);                           //Button | single click
  button1.attachDoubleClick(doubleClick);                  //Button | double click
  button1.attachLongPressStart(longPress);                 //Button | long click
  button1.setClickTicks(250);                              //Button | knows the difference between a single click and a double click
  button1.setPressTicks(1000);                             //Button | how long to be considered a long press  

  delay(100);
  Serial.printf("Starting OLED and BME ...\n");  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_Address)) { //REQUIRED LINE FOR OLED TO BEGIN WITH HEX ADDRESS OF OLED, CHECKS IF WORKING
    Serial.printf("Error beginning display.\n");
    return;
  }
  else {
    Serial.printf("Display has started successfully.\n");
  }
  display.setTextSize(2);                                   //OLED | Draw "2"X-scale text
  display.setTextColor(SSD1306_WHITE);                      //OLED | setting text color  
  
  BMEstatus = bme.begin(BME_Address);                       //REQUIRED LINE TO BEGIN BME WITH HEX ADDRESS OF BME, CHECK IF WORKING BELOW
  if (BMEstatus == false) {
    Serial.printf("BME280 not detected. Check BME address and wiring.\n");
  }
  else {
    Serial.printf("BME280 has been detected.\n");    
  }

  pixel.begin();                                            //REQUIRED LINE TO BEGIN PIXELS
  pixel.show();

  Serial.printf("Detecting ethernet link ...\n");
  Ethernet.begin(mac);                                      //REQUIRED LINE TO BEGIN ETHERNET LINK
  printIP();
  Serial.printf("LinkStatus: %i  \n",Ethernet.linkStatus());
  
  myWemo.switchOFF(selectWemo);                             //Ensuring myWemo is off
  delay(1000);
  wemoOn = false;                                           //Establishing wemoOn is false
  hueStatus = false;                                        //Ensuring hue lights are turned off
  for (i=1; i <= 3; i++) {
    setHue(i, hueStatus, hueColor, hueBright, 255);
  }    
  Serial.printf("Finished setup.\n");
}

void loop() {

  currentTime = millis();
  
  if (isCatThere() == true && wemoOn == false) {                //turns on Wemo if cat is there
    airFreshenerOn();
  }
  if ((currentTime - wemoOnTime) > 10000 && wemoOn == true) {   //turns off Wemo after 10 seconds of being on
    airFreshenerOff();  
  }
  roomTempDetect();


}

//******* ALL USER INPUT FUNCTIONS SHOULD HAVE AN "IF UNLOCKED" IMMEDIATELY FOLLOWING*******

void oneClick() {
//one click function to turn current light on/off 
}

void doubleClick() {
//double click function to manually turn on/off Weemo outlet
}

void longPress() {
//long click function to change current light
}

void roomTempDetect() {
  
  tempC = bme.readTemperature(); //reading temp in celsius
  tempF = ((tempC * 9/5) + 32);  //converting to fahrenheit
  if ((currentTime - tempTime) > 10000 && tempF != lastTemp) {
    Serial.printf("Room temperature is currently %f%c fahrenheit.\n", tempF, degree);   //Serial.print does not display degree character properly | this is normal.
    pixel.clear();
    pixel.show();
    if (tempF >= 70) {
      Serial.printf("Temperature is great than or equal to 70 degrees. Lighting pixels to red.\n");
      pixel.fill(red, 0, pixel_Count);
      pixel.setBrightness(30);
      pixel.show();
      hueFlash(red);
    }
    if (tempF < 65) {
      Serial.printf("Temperature is less than 65 degrees. Lighting pixels to blue.\n");
      pixel.fill(blue, 0, pixel_Count);
      pixel.setBrightness(30);
      pixel.show();
      hueFlash(blue);      
    }
    if (tempF >= 65 && tempF < 70) {
      Serial.printf("Temperature is greater than or equal to 65 and less than 70. Lighting pixels to yellow.\n");      
      pixel.fill(maize, 0, pixel_Count);
      pixel.setBrightness(30);
      pixel.show();
      hueFlash(yellow);
    }
    lastTemp = tempF;
    tempTime = millis();    
  }
}

void hueFlash(int tempColor) {
  saveBright = hueBright;
  saveColor = hueColor;
//  saveCurrentLight = hueLight;
//  saveHueOn = hueStatus;
  if (tempColor == red) {
    hueBright = 255;
    hueColor = HueRed;
    for (i=1; i <= 3; i++) {
      setHue(i, true, hueColor, hueBright, 255);
    }           
  }
  if (tempColor == yellow) {
    hueBright = 255;
    hueColor = HueYellow;
    for (i=1; i <= 3; i++) {
      setHue(i, true, hueColor, hueBright, 255);
    }          
  }    
  if (tempColor == blue) {
    hueBright = 255;
    hueColor = HueBlue;
    for (i=1; i <= 3; i++) {
      setHue(i, true, hueColor, hueBright, 255);
    }          
  }
  hueColor = saveColor;
  hueBright= saveBright;
  delay(2000);
  setHue(hueLight, hueStatus, hueColor, hueBright, 255);   //reverting back to previous hue settings
}

int pMeterToBright() {                                     //converts potentiometer readings to brightness for the Hue Light

  potRead = analogRead(potentiometerPin);
  hueBright = map(potRead,0,1024,0,255);
  if (hueBright != lastHueBright) {                               //ensures only printing brightness when it changes
    Serial.printf("Brightness changed to: %i.\n", hueBright);
    lastHueBright = hueBright; 
  }
  return hueBright;
}

void executeDisplay() {                                    //this function is to display 
  display.setCursor(0,0);
  display.display(); //displaying  
}

void _clearDisplay() {                                     //this function clears display
  display.clearDisplay();
  display.display();
}

void airFreshenerOn() {
  myWemo.switchON(selectWemo);    //Commenting out all Wemo functionality due to an error in the header file on line 23 completely stopping all code.
  delay(100);
  wemoOnTime = millis();
  wemoOn = true; 
  digitalWrite(encoderPin_R, LOW);              //Encoder | turning red LED off
  digitalWrite(encoderPin_G, HIGH);             //Encoder | turning green LED on
  Serial.printf("Wemo has turned on.\n");
  hueStatus = true;
  hueColor = HueViolet;
  hueBright = 100;
  for (i=1; i <= 3; i++) {
    setHue(i, hueStatus, hueColor, hueBright, 255);
  }   
  Serial.printf("Turning on the lights for the cat.");
  
}

void airFreshenerOff() {
  myWemo.switchOFF(selectWemo);  //Commenting out all Wemo functionality due to an error in the header file on line 23 completely stopping all code.
  delay(100);
  wemoOn = false;
  digitalWrite(encoderPin_R, HIGH);              //Encoder | turning red LED on
  digitalWrite(encoderPin_G, LOW);               //Encoder | turning green LED off  
  Serial.printf("Wemo has turned off.\n");
  hueStatus = false;
  for (i=1; i <= 3; i++) {
    setHue(i, hueStatus, hueColor, hueBright, 255);
  }  
  Serial.printf("Turning the lights off.\n");    
}

void printIP() {
  Serial.printf("My IP address: ");
  for (byte thisByte = 0; thisByte < 3; thisByte++) {
    Serial.printf("%i.",Ethernet.localIP()[thisByte]);
  }
  Serial.printf("%i\n",Ethernet.localIP()[3]);
}

bool isCatThere() {                                  //this function checks if cat is passing by the ultrasonic sensor                            

  static bool Status;                                //Local variable to determine true/false of the bool function
  digitalWrite(ultrasonicPin_Ping, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicPin_Ping, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicPin_Ping, LOW);
  duration = pulseIn(ultrasonicPin_Echo, HIGH);
  inches = microsecondsToInches(duration);
  if ((currentTime - lastSonicTime) > 1000) {       //Only prints if the cat is there every X miliseconds
    if (inches < 3) {                               //Need "inch" less than distance to the wall
      if (Status != true) {                         //included to only send the Serial.print one time when status changes
      Serial.printf("The cat is there!\n");
      Serial.printf("Setting lastSonicTime to: %i\n", lastSonicTime);      
      Serial.printf("Reading inches: %i\n", inches);
      }
      Status = true;
    }
    else {
      if (Status != false) {                        //included to only send the Serial.print one time when status changes
      Serial.printf("The cat is not there. :( \n");
      Serial.printf("Setting lastSonicTime to: %i\n", lastSonicTime); 
      Serial.printf("Reading inches: %i\n", inches);             
      }
      Status = false;
    }
    lastSonicTime = millis();
  }
  return Status;
}          

long microsecondsToInches(long microseconds) {            //Ultrasonic Sensor | calculations found: https://www.tutorialspoint.com/arduino/arduino_ultrasonic_sensor.htm
   return microseconds / 74 / 2;                          
}
