# Kaleb's Smart Room Controller

## Overview
The purpose of this project is to increase automation within Blink (my cat)'s living environment.

1. Turn on the air freshener and hue lights when the cat is detected entering her litter box
1. Detect the temperature of the cat's room and light up LED's on the board accordingly.
		-flash hue lights when the temperature changes enough to change LED colors
1. 3D model a button knob for the encoder using this link: https://www.sparkfun.com/products/15141
		-click on the features tab in the link
1. Full user input to adjust hue lights
		-Change brightness via potentiometer
		-Change active light via encoder
		-Turn on active light with a single click
		-Change active light color with a long press
1. Keypad that takes in a passcode input
		-locks all user inputs when code is not entered
		-unlocks when correct passcode is entered
		-keypad can be re-locked via the '#' key

1. Display lock/unlock status on OLED


### The controller will have the ability to be manually controlled ontop of the automatic triggers.

## Details

This project uses the following components:

* Phillips hue lights
* Breadboard
* Wires
* Teensy 3.2 microcontroller
* Ultrasonic Sensor (the 2x giant silver circles) | Sends signal, encounters obstacle, then comes back and calculates distance

Can look into more parts: https://www.elegoo.com/products/elegoo-37-in-1-sensor-kit?_pos=1&_sid=ba0552267&_ss=r

## Summary

Automated hue lights, automated air-freshener through a Wemo outlet and fully adjustable hue lights through user input on the board.
