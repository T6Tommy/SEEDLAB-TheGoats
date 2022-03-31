#Aaron Hsu
#Demo2

#All of the imported modules
import numpy as np
import cv2 as cv
import math
#import i2c
from picamera import PiCamera
from time import sleep


sleep(2)
PiCamera.shutter_speed= PiCamera.exposure_speed
PiCamera.exposure_mode = 'off'
g = PiCamera.awb_gains
PiCamera.awb_mode = 'off'
PiCamera.awb_gains = g

#Start live video
cap = cv.VideoCapture(0)
#Checks to see if camera is working
if not cap.isOpened():
    print("Cannot open camera")
    exit()

#While Loop to read each frame
while True:
    ret, img = cap.read() #Reads Frame

    #If frame cannot be read
    if not ret:
        print("Cannot read frame")
        break
    #Scales frame
    width = int(img.shape[1]/2)
    height = int(img.shape[0]/2)
    new = (width, height)
    img = cv.resize(img, new) #New resized frame (smaller)
    #Converts frame from BGR format to HSV
    hsv = cv.cvtColor(img,cv.COLOR_BGR2HSV)
    #Uppper and Lower bounds for color detection
    lower = np.array([96,150,50])
    upper = np.array([108,255,255])
    mask = cv.inRange(hsv, lower, upper) #Mask for color detection
    #Frame to display only yellow objects and black everywhere else
    res = cv.bitwise_and(img, img, mask=mask)
    #Kernel for the filters
    kernel = np.ones((5,5),np.uint8)
    #Saving and then reading frame in grayscale
    img2 = cv.imwrite('/home/pi/Desktop/Demo1.jpg', res)
    img2 = cv.imread('/home/pi/Desktop/Demo1.jpg',0)
    #Uses 'opening' and 'closing' effect on frame
    img3 = cv.morphologyEx(img2, cv.MORPH_OPEN, kernel)
    img3 = cv.morphologyEx(img3, cv.MORPH_CLOSE, kernel)
    #Threshold to make frame binary
    ret,thresh1 = cv.threshold(img3,1,255,cv.THRESH_BINARY)
    nonzero = cv.findNonZero(thresh1)
    #Takes the mean of the array for coordinates
    xy = cv.mean(nonzero)
    #Equations to find angle
    angle = 53 / 2
    w = width / 2
    w1 = w - xy[0]
    ratio = w1 / w
    angle = angle * ratio
    radians = math.radians(angle)

    #x,y coordinates
    x = round(xy[0],2)
    y = round(xy[1],2)
    #Displays live video
    cv.imshow('frame',img3)
    #Prints out the angle or "No Marker Found" if no object
    if xy[0] == 0 and xy[1] == 0:
        print("No Marker Found")
        #i2c.command(i2c.CMD_NULL, 0)
    else:
        print(radians)
        #i2c.command(i2c.CMD_TURN, radians)


    #Displays live video
    cv.imshow('frame',res)

    #To end live video
    if cv.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv.destroyAllWindows()
