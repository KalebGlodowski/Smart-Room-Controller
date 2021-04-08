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
const int  passLength = 4;        //Keypad passcode character length
const char lockKey = '#';         //Keypad button to lcok the keypad 

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

char hexaKeys [keypad_Rows] [keypad_Cols] = {     //Keypad | array for the buttons
  { '1', '2', '3', 'A'},
  { '4', '5', '6', 'B'},
  { '7', '8', '9', 'C'},
  { '*', '0', '#', 'D'}
};
char keyCode[passLength] = {'9', '4', '9', '6'};  //Keypad | passcode
byte rowPins[keypad_Rows] = {0, 1, 2, 3};         //Keypad | pins being used for the rows
byte colPins[keypad_Cols] = {4, 5, 6, 7};         //Keypad | pins being used for the columns
char keypadInput;                                 //Keypad | user input to the keypad
char s;                                           //Keypad | "s" is the spot the user is inputing a key for. ie: 0 is spot 1, 1 is spot 2, ect.
char keySequence[passLength];                     //Keypad | this is the code that the user has entered so far, will only go on to the nex character if correct
bool resetPosition;                               //Keypad | resetting the "s", reverting the user back to s=0, also known as position 1
bool keyState;                                    //Keypad | state of whether key is locked or unlocked
bool isLocked;                                    //Keypad | necessary to not have OLED spamming/flickering. This will be set true when locked, and turned false when unlocked.
  
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
bool hueStatus;                   //Hue | is hue light on?
int saveBright;                   //Hue | saved brightness for hueflash
int saveColor;                    //Hue | saved color for hueflash
bool needHueRestore;              //Hue | restores to saved hue settings
int i;                            //Hue | used in the hue for loop to light all 3 lights
int previousColor;                //Hue | last hue color
int hueColorSelect[] = 
{HueOrange, HueYellow, HueViolet};//Hue | 3 colors to switch between utilizing the longPress
int c;                            //Hue | c is the color within the hueSelect, can be between 0 and 2
int hueSaturation;                //Hue | saturation level which is set manually via turning the encoder

bool singleClickState;            //Button | a state that is changed with a single click
bool doubleClickState;            //Button | a state that is changed with a double click
bool longPressState;              //Button | a state that is changed with a long press

int lastPosition;                 //Encoder | remembering last position of encoder
int encPosition;                  //Encoder | current position of encoder

// *** Defining objects for the header files below ***

OneButton button1 (encoderPin_Switch, true, true);                                //for Button
Encoder myEnc (encoderPin_A, encoderPin_B);                                       //for Encoder
Adafruit_SSD1306 display(screen_Width, screen_Height, &Wire, OLED_Reset);         //for OLED
Adafruit_NeoPixel pixel (pixel_Count, pixel_Pin, NEO_GRB + NEO_KHZ800);           //for NeoPixels
Adafruit_BME280 bme;                                                              //for BME
Wemo myWemo;                                                                      //for Wemo
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, keypad_Rows, keypad_Cols); //for Keypad

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
  _clearDisplay();
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
  wemoOn = false;                                           //Ensuring wemoOn is false
  hueStatus = false;                                        //Ensuring hue lights are turned off
  for (i=1; i <= 3; i++) {
    setHue(i, hueStatus, hueColor, hueBright, 255);
  }    
  Serial.printf("Finished setup.\n");
}

void loop() {

  button1.tick();                                               //required for button to work
  currentTime = millis();  
  if (isCatThere() == true && wemoOn == false) {                //turns on Wemo if cat is there
    airFreshenerOn();
  }
  if ((currentTime - wemoOnTime) > 10000 && wemoOn == true) {   //turns off Wemo after 10 seconds of being on
    airFreshenerOff();  
  }
  roomTempDetect();
  isKeyUnlocked();
  encoderTurn();  
}

//******* ALL USER INPUT FUNCTIONS HAVE AN "IF UNLOCKED" PRIOR TO ANY CODE ||| KEYPAD MUST BE USED TO UNLOCK FIRST*******

void oneClick() {                                         //manually turn on/off the hue lights
  if (isKeyUnlocked() == true) {
    singleClickState = !singleClickState;
    Serial.printf("Button has been clicked a single time.\n");
    if (singleClickState == true) {
      Serial.printf("Single click on state.\n");    
      hueStatus = true;
      for (i=1; i <= 3; i++) {
        setHue(i, hueStatus, hueColorSelect[c], hueBright, hueSaturation);
      }    
    }
    if (singleClickState == false) {
      Serial.printf("Single click off state.\n");    
      hueStatus = false;
      for (i=1; i <= 3; i++) {
        setHue(i, hueStatus, hueColorSelect[c], hueBright, hueSaturation);
      }    
    }    
  }
}

void doubleClick() {                                      //manually turn on/off the airfreshener + violet hue lights
  if (isKeyUnlocked() == true) {  
    Serial.printf("Button has been double clicked.\n");  
    doubleClickState = !doubleClickState;
    if (doubleClickState == true) {
      Serial.printf("Double click on state.\n");
      airFreshenerOn();
    }
    if (doubleClickState == false) {
      Serial.printf("Double click off state.\n");
      airFreshenerOff();
    }
  }
}

void longPress() {                                        //manually change the color of the hue lights
  if (isKeyUnlocked() == true) {
    //make something sweet happen on the OLED screen! But make sure it reverts back to whatever it was displaying last time
    Serial.printf("Button has been long pressed.\n");    
    longPressState = !longPressState;
    if (longPressState == true) {
      c = c + 1;   //c represents the spot in the array for hueColorSelect
      if (c > 2) { //c can only be 0, 1, or 2
        c = 0;
      }      
    }
  }
}

void encoderTurn() {                                      //manually change the saturation levels of the hue
  if (isKeyUnlocked() == true) {
    encPosition = myEnc.read();
    if (lastPosition != encPosition) {
      Serial.printf("The current encoder positin is: %i.\n", encPosition);
      if (encPosition < 0) {
        encPosition = 0;
        Serial.printf("encPosition being set to 0. Do not try to turn below 0.");
      }
      if (encPosition >= 128) {
        encPosition = 127;
        Serial.printf("encPosition being set to 127. Do not try to go beyond 127.");
      }
      hueSaturation = map(encPosition,0,127,0,255);
      Serial.printf("The current hue saturation is: %i.\n", hueSaturation);
      lastPosition = encPosition;
    }  
  }
}

bool isKeyUnlocked () {                                   //keypad lock and unlock function
  keypadInput = customKeypad.getKey();  //establishes user input  
  if (resetPosition == true) {
    s = 0;
  }
  if (keypadInput == lockKey && keyState == true) {                             //this gives the keypad a functionality of being re-locked after being locked
    keyState = false;
    keySequence[3] = NULL; //resetting the keySequence
    keySequence[2] = NULL;
    keySequence[1] = NULL;
    keySequence[0] = NULL;
    resetPosition = true; //resetting position back to 1
    Serial.printf("Key # pressed. Locking keypad.\n");
    Serial.printf("To access user input features, please re-enter the passcode.\n");
    return false;
  }
  if (keypadInput && keyState == false) {                                       //this is the main keycode if function that compares user input char to keyCode
    Serial.printf("Key %c has been pressed.\n", keypadInput);
    Serial.printf("Currently entering character in the %i position.\n", (s+1)); //s+1 because first place would show a 0 and we want the user to see a 1
    if (keyCode[s] == keypadInput) {
      Serial.printf("Position %i key unlocked.\n", (s+1));
      keySequence[s] = keypadInput;
      resetPosition = false;
    }
    else {
      resetPosition = true;
      Serial.printf("Password incorrect for position %i. Resetting.\n", (s+1));
    }
    s = s+1;
  }
  if (keySequence[3] == keyCode[3]) {
    if (keyState == false) {
      Serial.printf("Password has been entered corrrect. User input is unlocked.\n");
      _clearDisplay();
      display.setTextSize(1); // Draw 1X-scale text      
      display.printf("All user input is currently:\n");
      display.setTextSize(3); // Draw 3X-scale text
      display.printf("UNLOCKED!");
      executeDisplay(); 
      keyState = true; //this is put in place to keep the above line from spamming
      isLocked = false;
    }
    return true;  //returns a true for the isKeyLocked function
  }
  else {
    if (isLocked != true) {
     _clearDisplay();
    display.setTextSize(1); // Draw 1X-scale text      
    display.printf("All user input is currently:\n");
    display.setTextSize(3); // Draw 3X-scale text
    display.printf("LOCKED!");
    executeDisplay();
    isLocked = true;     
    }
    
    return false; //returns a false for the isKeyLocked function
  }
}

void roomTempDetect() {                                   //detects the temperature in the room and lights the NeoPixels accordingly
  
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

void hueFlash(int tempColor) {                            //Flashes the hue lights when the temperature changes
  saveBright = hueBright;
  saveColor = hueColor;
  if (tempColor == red && tempColor != previousColor) {   //put in tempColor != previousColor because we do not want to flash unless changing temperature colors
    hueBright = 255;
    hueColor = HueRed;
    for (i=1; i <= 3; i++) {
      setHue(i, true, hueColor, hueBright, 255);
    }  
    previousColor = tempColor;
    Serial.printf("Flashing red.\n");
    needHueRestore = true;               
  }
  if (tempColor == yellow && tempColor != previousColor) {
    hueBright = 255;
    hueColor = HueYellow;
    for (i=1; i <= 3; i++) {
      setHue(i, true, hueColor, hueBright, 255);
    }
    previousColor = tempColor;
    Serial.printf("Flashing yellow.\n");   
    needHueRestore = true;                       
  }    
  if (tempColor == blue && tempColor != previousColor) {
    hueBright = 255;
    hueColor = HueBlue;
    for (i=1; i <= 3; i++) {
      setHue(i, true, hueColor, hueBright, 255);
    } 
    previousColor = tempColor;   
    Serial.printf("Flashing blue.\n");
    needHueRestore = true;                
  }
  if (needHueRestore == true) {
    hueColor = saveColor;
    hueBright= saveBright;
    delay(3000);
    for (i=1; i <= 3; i++) {  
    setHue(i, hueStatus, hueColor, hueBright, 255);   //reverting hue lights back to previous hue settings    
    }
    Serial.printf("Hue lights have been restored to settings prior to flash.\n");
  }
}

int pMeterToBright() {                                    //converts potentiometer readings to brightness for the Hue Light
  if (isKeyUnlocked() == true) {
    potRead = analogRead(potentiometerPin);
    hueBright = map(potRead,0,1024,0,255);
    if (hueBright != lastHueBright) {                               //ensures only printing brightness when it changes
      Serial.printf("Brightness changed to: %i.\n", hueBright);
      lastHueBright = hueBright; 
    }
    return hueBright;
  }
}

void executeDisplay() {                                   //this function is to display 
  display.setCursor(0,0);
  display.display(); //displaying  
}

void _clearDisplay() {                                    //this function clears display
  display.clearDisplay();
  display.display();
}

void airFreshenerOn() {                                   //turns on the Weemo outlet and hue lights
  myWemo.switchON(selectWemo);    //Commenting out all Wemo functionality due to an error in the header file on line 23 completely stopping all code.
  delay(100);
  wemoOnTime = millis();
  wemoOn = true; 
  digitalWrite(encoderPin_R, LOW);              //Encoder | turning red LED off
  digitalWrite(encoderPin_G, HIGH);             //Encoder | turning green LED on
  Serial.printf("Wemo has turned on.\n");
  hueStatus = true;
  c = 2; //this is Violet in the hueColorSelect array
  hueBright = 80;
  for (i=1; i <= 3; i++) {
    setHue(i, hueStatus, hueColorSelect[c], hueBright, 255);
  }   
  Serial.printf("Turning on the lights for the cat.");
  
}

void airFreshenerOff() {                                  //turns off Weemo outlet and hue lights
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

void printIP() {                                          //ensures ethernet is working properly and prints ip
  Serial.printf("My IP address: ");
  for (byte thisByte = 0; thisByte < 3; thisByte++) {
    Serial.printf("%i.",Ethernet.localIP()[thisByte]);
  }
  Serial.printf("%i\n",Ethernet.localIP()[3]);
}

bool isCatThere() {                                       //this function checks if cat is passing by the ultrasonic sensor                            

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
