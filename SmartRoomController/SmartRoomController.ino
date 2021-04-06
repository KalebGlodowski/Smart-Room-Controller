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

// *** Declaring all constants below ***

const int encoderPin_A = 14;      //encoder Pin A
const int encoderPin_B = 15;      //encoder Pin B
const int encoderPin_Switch = 21; //encoder Pin Switch 

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

const int ultrasonicPin_Ping = 8; //Ultrasonic Pin OUT
const int ultrasonicPin_Echo = 16;//Ultrasonic Pin IN

// *** Variables below ***

bool BMEstatus;                   //BME | true/false variable set to BME's start status whether it is on or not.

int currentTime;                  //set to millis(), this is the time the code has been running

int sonicRunTime;                 //Ultrasonic | the time different between sonicTime and currentTime
int sonicTime;                    //Ultrasonic | a timestamp given after going through the ultrasonic IF statement

// *** Defining objects for the header files below ***

OneButton button1 (encoderPin_Switch, true, true);                        //for Button
Encoder myEnc (encoderPin_A, encoderPin_B);                               //for Encoder
Adafruit_SSD1306 display(screen_Width, screen_Height, &Wire, OLED_Reset); //for OLED
Adafruit_NeoPixel pixel (pixel_Count, pixel_Pin, NEO_GRB + NEO_KHZ800);   //for NeoPixels
Adafruit_BME280 bme;                                                      // for BME


void setup() {

  pinMode (ultrasonicPin_Ping, OUTPUT);                     //Ultrasonic Sensor | sending a signal out
  pinMode (ultrasonicPin_Echo, INPUT);                      //Ultrasonic Sensor | recieving a signal back
  
  Serial.begin(9600);

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

  Serial.printf("Finished setup.\n");
}

void loop() {

  currentTime = millis();
  isCatThere();
  delay(100);
  
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

int pMeterToBright() {
//potentiometer read to brightness function
}

void executeDisplay() { //this function is to display 
  display.setCursor(0,0);
  display.display(); //displaying  
}
void _clearDisplay() {
  display.clearDisplay();
  display.display();
}

void hueFlash() {
//hueflash function to flash hues on and then off
}

void printIP() {
  Serial.printf("My IP address: ");
  for (byte thisByte = 0; thisByte < 3; thisByte++) {
    Serial.printf("%i.",Ethernet.localIP()[thisByte]);
  }
  Serial.printf("%i\n",Ethernet.localIP()[3]);
}

bool isCatThere() {
   
  long duration, inches;
  pinMode(ultrasonicPin_Ping, OUTPUT);
  digitalWrite(ultrasonicPin_Ping, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicPin_Ping, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicPin_Ping, LOW);
  pinMode(ultrasonicPin_Echo, INPUT);
  duration = pulseIn(ultrasonicPin_Echo, HIGH);
  inches = microsecondsToInches(duration);

  Serial.printf("in %i\n", inches);
  if (inches > 1 && inches < 3) {               //Need "inch" less than distance to the wall and greater then the distance of the ultrasonic sensor to the cat litter box
    Serial.printf("The cat is there!\n");
    return true;
  }
  else {
    Serial.printf("The cat is not there. :( \n");
    return false;
  }    
    
//  sonicRunTime = currentTime - sonicTime;
//  ultrasonicSensing();
//   if (in > 1 && inch < 2 && sonicRunTime > 500) {
//   Serial.printf("Ultrasonic Sensor: Reading an object...");
//   digitalRead(ultrasonicPin_Echo);
//    Serial.printf("%i\n", digitalRead(ultrasonicPin_Echo));
//    sonicTime = currentTime; 
}

long microsecondsToInches(long microseconds) {            //Ultrasonic Sensor | calculations found: https://www.tutorialspoint.com/arduino/arduino_ultrasonic_sensor.htm
   return microseconds / 74 / 2;                          
}
