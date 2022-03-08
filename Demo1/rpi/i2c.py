#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 16 11:39:39 2022

Description:    i2c.py takes a floating point angle input (radians) and sends a
                corresponding code to an arduino peripheral via I2C. The resulting
                angle is displayed on an LCD display.

@author: Paul Sampson
"""
from enum import Enum
import time
import board
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
import struct

from smbus import SMBus

lcd_columns = 16
lcd_rows = 2

#ada = board.I2C()
smb = SMBus(1) #initialize i2c

MOTOR_ADDR = 0x04

# Motor command directory. values are to be used in the i2c register field:
CMD_FDBK = 0x01
CMD_TURN = 0x02

# Sends the arduino the command to move forward or backward a certain distance
# in meters.
def command (register, value):
    message = list(struct.pack("f", value)) # convert float to a list of bytes
    print ([hex(x) for x in message]) # Print out the bytes in hexadecimal
    try:
        smb.write_block_data(MOTOR_ADDR, register, message) # send the command
    except IOError:
        print("Failed to communicate with motor controller.") # i2c failure

#lcd = character_lcd.Character_LCD_RGB_I2C(ada, lcd_columns, lcd_rows)

def main():
    while(True):
        while(True):
            try: # Prompt user input
                angle = float(input("Angle: "))
                break
            except ValueError: # The user did not enter a float!
                print ("Invalid floating-point value!")
                continue
        while (True):
            try: # Prompt user input
                distance = float(input("Distance: "))
                break
            except ValueError: # The user did not enter a float!
                print ("Invalid floating-point value!")
                continue

        # Prompt arduino to move the wheel to the specified angle:
        print("Sending command for turning %f radians:" % angle)
        command(CMD_TURN, angle)
        time.sleep(1)
        print("Sending command for moving %f meters:" % distance)
        command(CMD_FDBK, distance)

if __name__ == "__main__":
        main()
