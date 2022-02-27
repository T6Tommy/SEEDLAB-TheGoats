# SEED Lab Group 7 - The Goats

# Mini Project

## EENG 350 B, Spring 2022

### Team Members:

- Josie Dudden - Simulation/Control

- Aaron Hsu - Computer Vision

- Paul Sampson - System Integration

- Thomas MacDougall - Localization

## 

This repository is for the Mini Project and is seperated into two main folders, "arduino," which contains arduino sketches, and "rpi," which contains python code to be run on the Raspberry Pi.

The two Arduino sketches are as follows:

- **"Running.ino"** - Contains the control system code and I2C handler method.  The control system occurs within the main Loop, while the I2C handler is in an ISR method called "recieveData."  The control system is a Proportional-Integral loop that takes in a rotary encoder value and regulates the average voltage level for a motor driver via the ATMEGA's built-in PWM controller.  The I2C handler is capable of taking in up to 32 bytes of data and stores the data in a byte array called "message."  There is also a method called "sendData" that was not used for the Mini Project.

- **"motor_sim.ino"** -  This sketch outputs a step response (time-delayed by 1 second) with a magnitude of 7 volts (PWM average) to the motor driver.  The sketch then monitors the resulting encoder data and records it onto the Serial console.

The second file is "rpi" for the 2 files of Python code used on the Raspberry Pi:

- **"MP1.py"** - This is the Python code for the Computer Vision Marker Detection Scheme.

- **"i2c.py"** - Takes in a quadrant (via four boolean values, N, S, E, W) and sends a code to the arduino via I2C which corresponds to an angle associated with the quadrant.  The angle is also displayed on the LCD display via the AdaFruit I2C LCD library.


