# SEED Lab Group 7 - The Goats

# Demo 2

## EENG 350 B, Spring 2022

### Raspberry Pi: Python Code

In this folder, "rpi", there are three sets of Python code used on the Raspberry Pi.

The first code is labeled, "Final.py". This Python code is the marker detection scheme used with the Pi Camera to detect the 1" blue tape marker.
This Python code also sends flags using the i2c code to the Arduino of where the blue tape is on the screen for stopping, turning, et cetera. 

The second code is labeled, "i2c.py". This Python code takes in both a variable address and a value to be written at that address.
It attempts to send it to an Arduino peripheral at i2c address 0x04.
