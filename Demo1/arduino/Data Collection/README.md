These 2 files were creates to get the data from the robot running to obtain the transfer functions.

The arduino Uno must be connected to the 2 motors that spin the wheels, pins 2&11 for the left encoder, pins 3&5 for the right encoder, the batter, and the battery controller.

We simply send a fixed volatge for a certian amount of time to the motors and record how far the robot has travelled. This is accomplished with the PWM output on the arduino Uno, and the encoder calculations with the encoder.h library.
