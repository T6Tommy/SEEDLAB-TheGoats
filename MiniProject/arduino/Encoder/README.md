Rotary Encoder for Pololua Motor
Group 7 - The Goats
Spring 2022

This is the code that was initally used to implement the new encoder library that is defined as
High performance with 4X counting and very efficient interrupt routines. 

Required to run the Code:
- Pololua motor encoder Pins attached to the arduino uno 5V, GND, and PINS 3 & 4.
- Button attached to 5V, GND, and PIN 8

This code was used to get a theta output based off of the motor spinning.
The motor has 3200 counts per rotation, which we relate to a delta value that is >3200/(2pi)<
We take this value and add/sumtract it to the theta value every time the encoder updates its'
position.
When pressed, the button will reset the theta value back to 0.
