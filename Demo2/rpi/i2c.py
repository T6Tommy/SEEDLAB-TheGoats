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
import struct
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

from smbus import SMBus

MOTOR_ADDR = 0x04

lcd_columns = 16
lcd_rows = 2

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

def Send(register, message):
    try:
        print("Message: %d" % message)
        smb.write_byte_data(0x04, register, message) # Send the angle code.
    except IOError: # If there's no I2C let the user know:
        print("Cannot communicate with Arduino.")
try:
    lcd = character_lcd.Character_LCD_RGB_I2C(ada, lcd_columns, lcd_rows)
    lcd.color = [100, 0, 100] # Set the color to purple
except ValueError:
    print("No LCD detected.")

def PrintLCD (message):
    try:
        lcd.clear();
        if message == isOffscreen:
            lcd.message = "Tape \n offscreen"
        if message == isRight:
            lcd.message = "Tape is to\n the right"
        if message == isLeft:
            lcd.message = "Tape is to\n the left"
        if message == isExiting:
            lcd.message = "Tape is\nleaving view"
    except NameError:
        nullvar = 0

# Sends the arduino the command to move forward or backward a certain distance
# in metes.
def command (register, value):
    #print("%i, %i" %(register,value))
    message = 0
    if (register == 1):
        message = list(bytearray(struct.pack("f", value))) # convert float to a list of bytes
    #print(["0x%02x" % b for b in message])
    try:
        smb.write_block_data(MOTOR_ADDR, register, message) # send the command
    except IOError:
        print("Failed to communicate with motor controller.") # i2c failure
