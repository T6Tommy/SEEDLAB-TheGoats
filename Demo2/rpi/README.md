# SEED Lab Group 7 - The Goats

# Demo 2

## EENG 350 B, Spring 2022

### Raspberry Pi: Python Code

In this folder, "rpi", there are three sets of Python code used on the Raspberry Pi.

The first code is labeled, "Demo2.py". This Python code is the marker detection scheme used with the Pi Camera to detect the 1" blue tape marker.
This Python code also sends flags using the i2c code to the Arduino of where the blue tape is on the screen for stopping, turning, etc. This code was used for the first test to stop at the beginning of the tape.

The second code is labeled, "Demo2_2.py". This Python code is the marker detection scheme used with the Pi Camera to detect the 1" blue tape marker.
This Python code also sends flags using the i2c code to the Arduino of where the blue tape is on the screen for stopping, turning, etc. This code was used for the second test to stop at the end of the tape.

The third code is labeled, "i2c.py". This Python code takes in the horizontal angle in radians sent by the "Demo1.py".
It interprets this data to send to the LCD display and attempts to send it to an Arduino peripheral at i2c address 0x04.  The Send function sends commands indicating states and flags that the Arduino needs to respond to.  The message function sends floating-point values for distance or angles, and is not currently used by the Arduino.
