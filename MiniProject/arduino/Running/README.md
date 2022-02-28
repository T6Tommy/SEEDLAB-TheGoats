Mini Project Final Arduino Code.
Team 7 - The Goats
Spring 2022

Below is our final code for the Arduino that integrates into all the other subsystems. 

In order for this code to work: 
- The Arduino uno must have the pololu motor drive shield attached, and that needs to be attached to a battery and our motor. 
- The motor encoder pins must be attached to 5V, GND, and PINS 3&5 
- The Arduino must be attached to the raspberry-pi so that it can receive and send the positional data. 

This code will move the motor to the specific theta that is sent to it and will keep the motor at that position when it is moved. The arduino camera related 4 on-screen quadrants to different theta positions and sent the number 0-4 to the Arduino allowing it to move to the specific theta value. This was implemented using a PI controller that uses the calculated ki and p values from our experiment to regulate the voltage being sent to the motor. This code also implements the High-performance encoder with 4X counting and very efficient interrupt routines. 
