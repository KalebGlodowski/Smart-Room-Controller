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
const int pixel_Count = 16;      //NeoPixels, total amount of NeoPixels

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


// *** Defining objects for the header files below ***

OneButton button1 (encoderPin_Switch, true, true);                        //for Button
Encoder myEnc (encoderPin_A, encoderPin_B);                               //for Encoder
Adafruit_SSD1306 display(screen_Width, screen_Height, &Wire, OLED_Reset); //for OLED
Adafruit_NeoPixel pixel (pixel_Count, pixel_Pin, NEO_GRB + NEO_KHZ800);   //for NeoPixels
Adafruit_BME280 bme;                                                      // for BME


void setup() {

  pinMode (ultrasonicPin_Ping, OUTPUT); //Ultrasonic Sensor | sending a signal out
  pinMode (ultrasonicPin_Echo, INPUT);  //Ultrasonic Sensor | recieving a signal back
  
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_Address)) { //REQUIRED LINE FOR OLED TO BEGIN WITH HEX ADDRESS OF OLED, CHECKS IF WORKING
    Serial.printf("Error beginning display.\n");
    return;
  }
    
}

void loop() {


}

//******* ALL USER INPUT FUNCTIONS SHOULD HAVE AN "IF UNLOCKED" IMMEDIATELY FOLLOWING*******

//one click function to turn current light on/off

//double click function to do something maybe?

//long click function to change current light

//potentiometer read to brightness function

//display to led function

//hueflash function to flash hues on and then off
