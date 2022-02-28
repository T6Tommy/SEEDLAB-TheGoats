Mini Project Final Arduino Code.
Team 7 - The Goats
Spring 2022

This is our final code for the arduino that integrates into all the other subsystems.

In order for this code to work:
- The arduino uno must have the pololu motor drive shield attached, and that needs to be attached to
    a battery and our motor.
- The motor encoder pins must be attached to 5V, GND, and PINS 3&5
- The arduino must be attached to the raspberry-pi so that it can receive and send the positional data.

This code will move the motor to the specific theta that is sent to it, and will keep the motor at that position
when it is moved. The arduino camera related 4 on-screen quadrants to to different theta positions, and sent the 
number 0-4 to the arduino that allowed it to move to the specifiec theat value. This was implemented using a PI
controller that uses the calculated ki and kp values from our experiment to regulate the voltage being sent to 
the motor. This code also implements the High performance encoder with 4X counting and very efficient interrupt 
routines.
