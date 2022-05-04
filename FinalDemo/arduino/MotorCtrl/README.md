# MotorCtrl Arduino Sketch
## Directory Contents:
###MotorCtrl.ino

MotorCtrl.ino contains the main arduino functions needed to recieve I2C instructions and control two motors with encoder feedback.  This includes the state machine, I2C ISRs, and setup and loop methods.

### TurnCtrl.cpp and TurnCtrl.h

These files implement a discrete-time proportional controller intended to turn the robot to a given angular position.

### VelCtrl.cpp and VelCtrl.h

These files implement a discrete-time proportional-integral controller that keeps the robot moving at a set linear velocity.

### constants.h

Contains the constants needed for many of the functions in the sketch.
