#Aaron Hsu
#SEED Lab
#MiniProject

#All of the imported modules
import numpy as np
import cv2 as cv
import i2c
from picamera import PiCamera
from time import sleep
from matplotlib import pyplot as plt

#Auto White Balance
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
    lower = np.array([100,150,15])
    upper = np.array([140,255,255])
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
    #Finds nonzeros
    nonzero = cv.findNonZero(thresh1)
    #Takes the mean of the array for coordinates
    xy = cv.mean(nonzero)
    #Equations to find angle
    angle = 53 / 2
    w = width / 2
    w1 = w - xy[0]
    ratio = w1 / w
    angle = angle * ratio
    angle = round(angle,2)
    
    #x,y coordinates
    x = round(xy[0],2)
    y = round(xy[1],2)
    
    #Variable for North, South, West, East
    N = 0
    S = 0
    W = 0
    E = 0
    #Variable for the halfway point of frame
    h = height / 2

    #Prints out the angle or "No Marker Found" if no object
    if xy[0] == 0 and xy[1] == 0:
        print("No Marker Found")
    #If the angle is negative, then it is on the East side of frame
    elif angle < 0:
        E = 1
        #If y coordinate is more than h, it is on the South side
        if y > h:
            S = 1
        #If y coordinate is less than h, it is on the North side
        elif y < h:
            N = 1
    #If the angle is positive, then it is on the West side of frame
    elif angle > 0:
        W = 1
        #If y coordinate is more than h, it is on the South side
        if y > h:
            S = 1
        #If y coordinate is less than h, it is on the North side
        elif y < h:
            N = 1
    
    #If North East Quadrant, then position is 0
    if N == 1 and E == 1:
        print("Marker is at position 0")
    #If North West Quadrant, then position is pi/2
    elif N == 1 and W == 1:
        print("Marker is at position pi/2")
    #If South West Quadrant, then position is pi
    elif S == 1 and W == 1:
        print("Marker is at position pi")
    #If South East Quadrant, then position is 3pi/2
    elif S == 1 and E == 1:
        print("Marker is at position 3pi/2")
    else:
        None
    
    #Sends values to i2c code
    i2c.MoveWheel(N,S,E,W)

    #Displays live video
    cv.imshow('frame',res)

    #To end live video
    if cv.waitKey(1) & 0xFF == ord('q'):
        break
      
#Releases capture and closes live video
cap.release()
cv.destroyAllWindows()
