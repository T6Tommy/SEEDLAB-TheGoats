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

#
# 0x00  Tape is off screen
isOffscreen = 0x00

# 0x01  Tape is on screen, to the right
isRight = 0x01

# 0x02  Tape is approximately centered on the screen
isCentered = 0x02

# 0x03  Tape is on screen, to the left
isLeft = 0x03

# 0x04  Tape has exited the lower-peripheral vision
isExiting = 0x04

def Send(message):
    
    try:
       smb.write_byte_data(0x04, 0, message) # Send the angle code.
    except IOError: # If there's no I2C let the user know:
        print("Cannot communicate with Arduino.")

lcd = character_lcd.Character_LCD_RGB_I2C(ada, lcd_columns, lcd_rows)
lcd.color = [100, 0, 100] # Set the color to purple


def PrintLCD (message):
    lcd.clear();    
    if message == isOffscreen:
        lcd.message = "Tape \n offscreen"
    if message == isRight:
        lcd.message = "Tape is to\n the right"
    if message == isLeft:
        lcd.message = "Tape is to\n the left"
    if message == isExiting:
        lcd.message = "Tape is\nleaving view"


def main():
    while(True):
        try: # Prompt user input
            message = int(input("Please enter a message (0-4)"))
        except TypeError: # The user did not enter an integer!
            print ("Invalid integer value!")
            continue
        # Prompt arduino to move the wheel to the specified angle:
        Send(message)
        PrintLCD(message)

if __name__ == "__main__":
        main()
