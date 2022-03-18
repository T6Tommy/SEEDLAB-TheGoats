These 2 files were used in Demo 1.

The arduino Uno must be connected to the 2 motors that spin the wheels, pins 2&11 for the left encoder, pins 3&5 for the right encoder, the batter, and the battery controller.

For the Forward code, the robot will move forward in a straight line for the specified inches that are entered into the desired_pos variable. The code will check to see if it is on course by comparing the distancce travelled in both wheels and if one if further than the other, it will increase the PWM output of the opposite wheele.

For the Forward & Spin code, the robot will first spin a specified theat, set by the variable derires_theta, and then it will travel straight forward 3 feet. The desired theta refrences the center of the robot, and is calculated with the radius, and wheele distances factored in. Moving forward operates the same as above. We implemented a low leve finite state machine with if statements so that it will first spin, and then move forward. 
