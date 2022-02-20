// SEED Lab - Mini Project section 4.4
// Thomas MacDougall - Group 7
// Srping 2022

// 3200 counts is a fill rotation
#include <Encoder.h>

Encoder myEnc(3, 4); // Pin a from the motor is attached to digital pin 3, and pin b is attached to digital pin 4

const int buttonPin = 8; // Pin for the button is digital pin 8
int buttonState = 0;   // declaration for button state

void setup() {
  Serial.begin(57600); 
  pinMode(buttonPin, INPUT);
}

long oldPosition  = -999;
const float delta = 0.001963495; // This is [(2*pi) / 3200]
float theta = 0; // initilaze theta

void loop() {
  
  long newPosition = myEnc.read();

  if (newPosition != oldPosition) { // if the wheel has moved
    if(newPosition > oldPosition) { // if going forward
      theta += delta;               // increads theta by delta
    }
    else {                          // if going backwards
      theta = (theta - delta);      // decrease theta by delta
    }
    oldPosition = newPosition;      // sets new position for next loop
    Serial.println(theta);
  }
  
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) { // if button pressed
    theta = 0; // reset theta
  }
}
