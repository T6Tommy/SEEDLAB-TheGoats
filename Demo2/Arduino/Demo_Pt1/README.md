This is the code for the first part of Demo 2.

The Arduino Uno must be connected to the 2 motors that spin the wheels, pins 2 & 11 for the left encoder, pins 3 & 5 for the right encoder, the battery, and the battery controller. Also, communication must be set up via I2C so that the raspberry pi can send the flags so that the robot knows where it is in relation to the tape.

Our Finite State Machince works via 5 states: Tape not visible, Tape is to the left, Tape is centered, Tape is to the right, robot hit start of tape. Basically the raspberry pi will determine where the tape is and adjust its angle unil it is centered and move forward. At this point it will see if its on the center, if not, it will adjust until it is centered again and continue to move forward. The robot does all this until the tape hits the bottom of the screen where it will then continue to move forward for a little bit more to compensate for the viewing agle of the camera, and then it will stop.
