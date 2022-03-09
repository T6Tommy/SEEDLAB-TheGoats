 #Aaron Hsu
#Demo1

#All of the imported modules
import numpy as np
import cv2 as cv
import math
import glob
#import i2c
from picamera import PiCamera
from time import sleep
from matplotlib import pyplot as plt

sleep(2)
PiCamera.shutter_speed= PiCamera.exposure_speed
PiCamera.exposure_mode = 'off'
g = PiCamera.awb_gains
PiCamera.awb_mode = 'off'
PiCamera.awb_gains = g

# termination criteria
criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

objp = np.zeros((6*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

objpoints = []
imgpoints = []
images = glob.glob('/home/pi/Chessboard/*.jpg')

for fname in images:
    img = cv.imread(fname)
    gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)

    ret, corners = cv.findChessboardCorners(gray,(7,6),None)

    if ret == True:
        objpoints.append(objp)

        corners2 = cv.cornerSubPix(gray, corners,(11,11),(-1,-1), criteria)
        imgpoints.append(objp)

        cv.drawChessboardCorners(img, (7,6), corners2, ret)
        cv.imshow('img', img)
        cv.waitKey(500)
cv.destroyAllWindows()

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
    lower = np.array([75,150,0])
    upper = np.array([150,255,255])
    mask = cv.inRange(hsv, lower, upper) #Mask for color detection
    #Frame to display only yellow objects and black everywhere else
    res = cv.bitwise_and(img, img, mask=mask)
    #Kernel for the filters
    kernel = np.ones((5,5),np.uint8)
    #Saving and then reading frame in grayscale
    img2 = cv.imwrite('/home/pi/Desktop/mini1.jpg', res)
    img2 = cv.imread('/home/pi/Desktop/mini1.jpg',0)
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
    else:
        print(radians)


    #i2c.MoveWheel(N,S,E,W)

    #Displays live video
    cv.imshow('frame',res)

    #To end live video
    if cv.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv.destroyAllWindows()
