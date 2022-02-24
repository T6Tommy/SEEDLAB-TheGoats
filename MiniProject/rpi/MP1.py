 #Aaron Hsu
#MiniProject

#All of the imported modules
import numpy as np
import cv2 as cv
from picamera import PiCamera
from time import sleep
from matplotlib import pyplot as plt

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
    lower = np.array([100,150,10])
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
    #Displays live video
    cv.imshow('frame',res)

    N = 0
    S = 0
    W = 0
    E = 0

    h = height / 2
    location = 0
    
    #Prints out the angle or "No Marker Found" if no object
    if xy[0] == 0 and xy[1] == 0:
        print("No Marker Found")
    elif angle < 0:
        E = 1
        if y > h:
            S = 1
        elif y < h:
            N = 1
    elif angle > 0:
        W = 1
        if y > h:
            S = 1
        elif y < h:
            N = 1

    if N == 1 and E == 1:
        print("Marker is at position 0")
    elif N == 1 and W == 1:
        print("Marker is at position pi/2")
    elif S == 1 and W == 1:
        print("Marker is at position pi")
    elif S == 1 and E == 1:
        print("Marker is at position 3pi/2")
    else:
        None
    
    #Displays live video
    cv.imshow('frame',res)
    #Prints out the angle or "No Marker Found" if no object
    if xy[0] == 0 and xy[1] == 0:
        print("No Marker Found")
    else:
        print(angle)
    #To end live video
    if cv.waitKey(1) & 0xFF == ord('q'):
        break
    
cap.release()
cv.destroyAllWindows()
