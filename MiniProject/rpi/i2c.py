#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 16 11:39:39 2022

Description:    i2c.py takes a floating point angle input (radians) and sends a
                corresponding code to an arduino peripheral via I2C. The resulting
                angle is displayed on an LCD display.

@author: Paul Sampson
"""

import time
import board
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

from smbus import SMBus

lcd_columns = 16
lcd_rows = 2

pi = 3.141592653589793

ada = board.I2C()
smb = SMBus(1) #initialize i2c

def MoveWheel (N, S, E, W):
    lcd.clear();
    lcd.color = [100, 0, 100] # Set the color to purple

    message = 0

    if(N and W):
        message = 1
        lcd.message = "0"
    if(N and E):
        message = 2
        lcd.message = "pi/2"
    if(S and E):
        message = 3
        lcd.message = "pi"
    if(S and W):
        message = 4
        lcd.message = "3pi/2"
    try:
       smb.write_byte_data(0x04, 0, message) # Send the angle code.
       status = smb.read_byte_data(0x04, 0) # Recieve status code from arduino.
       if (status is 0): #status 0 corresponds to a successful turn
           print("The wheel has successfully turned with message %i" % message)
       else: # Any other value corresponds to some error we don't know about:
           print("An unspecified error was encountered while turning with message %i " % message)
    except IOError: # If there's no I2C let the user know:
        print("Cannot communicate with Arduino.")
    else: print ("Please enter an angle from -2 pi to 2 pi radians")

'''
def MoveWheel (angle):
    lcd.clear();
    lcd.color = [100, 0, 100] # Set the color to purple
    lcd.message = str(angle)

    if(-(2*pi) <= angle < 0): # Add 2 pi to negative angles
        angle += (2*pi)
    if(0 <= angle <= 2*pi): # Only accept angles from 0 to 2pi radians.
            angle_code = angle/(pi/2) # Divide by pi/2 to get a value from 0-4
            angle_code = round(angle_code)+1 # Add 1 and round up for values 1-5
            if(angle_code > 4):             # 5 corresponds to 360 = 0 degrees
                angle_code = 0
            # Let user know what we're doing:
            print ("Sending command %i to turn the wheel to %f " % (angle_code, angle))
            try:
                smb.write_byte_data(0x04, 0, angle_code) # Send the angle code.
                status = smb.read_byte_data(0x04, 0) # Recieve status code from arduino.
                if (status is 0): #status 0 corresponds to a successful turn
                    print("The wheel has successfully turned to %f." % angle)
                else: # Any other value corresponds to some error we don't know about:
                    print("An unspecified error was encountered while turning to %f" % angle)
            except IOError: # If there's no I2C let the user know:
                print("Cannot communicate with Arduino.")
    else: print ("Please enter an angle from -2 pi to 2 pi radians")
'''

lcd = character_lcd.Character_LCD_RGB_I2C(ada, lcd_columns, lcd_rows)

def main():
    while(True):
        try: # Prompt user input
            N = int(input("N"))
            S = int(input("S"))
            E = int(input("E"))
            W = int(input("W"))
        except TypeError: # The user did not enter an integer!
            print ("Invalid integer value!")
            continue
        # Prompt arduino to move the wheel to the specified angle:
        MoveWheel(N, S, E, W)

if __name__ == "__main__":
        main()
