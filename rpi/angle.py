#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 16 11:39:39 2022

Description:    angle.py takes an integer angle input (degrees) and sends a corresponding code
                to an arduino peripheral via I2C.

@author: Paul Sampson
"""

from smbus import SMBus

bus = SMBus(1) #initialize i2c

def MoveWheel (angle):
    if(-360 <= angle < 0): # Add 360 degrees to negative angles
        angle += 360
    if(0 <= angle <= 360): # Only accept angles from 0 to 360 degrees.
            angle_code = angle/90 # Divide by 90 to get a value from 0-4
            angle_code = round(angle_code)+1 # Add 1 and round up for values 1-5
            if(angle_code > 4):             # 5 corresponds to 360 = 0 degrees
                angle_code = 0
            # Let user know what we're doing:
            print ("Sending command %i to turn the wheel to %i " % (angle_code, angle))
            try:
                bus.write_byte_data(0x04, 0, angle_code) # Send the angle code.
                status = bus.read_byte_data(0x04, 0) # Recieve status code from arduino.
                if (status is 0): #status 0 corresponds to a successful turn
                    print("The wheel has successfully turned to %i." % angle)
                else: # Any other value corresponds to some error we don't know about:
                    print("An unspecified error was encountered while turning to %i" % angle)
            except IOError: # If there's no I2C let the user know:
                print("Cannot communicate with Arduino.")
    else: print ("Please enter an angle from -360 to 360 degrees")

def main():
    while(True):
        try: # Prompt user input
            angle = int(input("Enter an angle from -360 to 360 degrees to be sent to the Arduino:"))
        except TypeError: # The user did not enter an integer!
            print ("Invalid integer value!")
            continue
        # Prompt arduino to move the wheel to the specified angle:
        MoveWheel(angle)

if __name__ == "__main__":
        main()
