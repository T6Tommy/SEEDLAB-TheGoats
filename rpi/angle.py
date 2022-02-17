#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 16 11:39:39 2022

angle_codes is a dictionary that has a corresponding code to send to the
Arduino for angles from -360 to 360 degrees.

@author: paul
"""

from smbus2 import SMBus

angle_codes = {
    -90: 0x04,
    -180:0x03,
    -270: 0x02,
    -360: 0x01,
    0: 0x01,
    90: 0x02,
    180: 0x03,
    270: 0x04,
    360: 0x01,
    }

#bus = SMBus(1)

def SendRecieve (angle):
    if(-360 <= angle < 0):
        angle += 360
    if(0 <= angle <= 360):
            angle_code = angle/90
            angle_code = round(angle_code)+1
            print ("Sending command %i to turn the wheel to %i " % (angle_code, angle))
            #bus.write_byte_data(0x04, 0,angle_codes(angle))
            return 5
    else: print ("Please enter an angle from -360 to 360 degrees")

SendRecieve(95)