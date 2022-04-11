#Aaron Hsu
#Demo2

#All of the imported modules
import numpy as np
import cv2 as cv
import math
import i2c
import time
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

Turn = 0
Turn_old = 0

timer = 0
timer_old = 0

#While Loop to read each frame
while True:
    ret, img = cap.read() #Reads Frame

    #If frame cannot be read
    if not ret:
        print("Cannot read frame")
        break
    #Scales frame
    width = int(img.shape[1]/1.5)
    height = int(img.shape[0]/1.5)
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
    #print(angle)

    
    #x,y coordinates
    #x = round(xy[0],2)
    #y = round(xy[1],2)
    #Displays live video
    cv.imshow('frame',res)

    #stop = 9
    Go = 9
    
    #Different states for each Flag
    if xy[0] == 0 and xy[1] == 0:
        #print("Searching...")
        Turn= 1 #Continue turning till blue tape is found
    elif angle > 0.75:
        #print("Turning Left")
        Turn = 2 #Continue turning left because tape is found
    elif angle < -0.75:
        #print("Turning Right")
        Turn = 4 #Tape is somehow on the right of camera
    else:
        #print("Tape is centered")
        Turn = 3 #Tape should be centered ahead

    if xy[0] == 0 and xy[1] == 0:
        Max = 0
    else:
        Y,X = np.nonzero(thresh1)
        Max = np.amax(Y,0)      #Finds the front edge of tape

    if Max >= 319:
        Turn = 5  #This means tape is at end of camera, STOP
        Go = 0     #This means tape is still on camera, GO
    else:
                   #This means tape is not at end, GO
        Go = 1     #This means tape is not on camera, STOP

    #Sends Turn Flag
    if(Turn_old != Turn):
        #print(Turn)
        i2c.Send(0, Turn)
        Turn_old = Turn

    #timer for sending angle
    timer = time.perf_counter()
    if (timer > timer_old+0.1):
       i2c.command(1, radians)
       i2c.Send(2, Go)
    timer_old = timer


    #print(stop)
    #print(Turn)
    print(Go)
    #To end live video
    if cv.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv.destroyAllWindows()
