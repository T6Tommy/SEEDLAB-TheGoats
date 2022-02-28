#include "DualMC33926MotorShield.h"

DualMC33926MotorShield md;

#include <Encoder.h> // enable the high precision encoder library
Encoder myEnc(3, 5);

int PWM_Pin = 9; // PWM wave outputs to pin 9

void setup() {
  Serial.begin(250000);
  pinMode(PWM_Pin, OUTPUT); // Set the PWM output 
  analogWrite(PWM_Pin,0);
  md.init();
}

// Encoder Variables
long oldPosition  = -999;
long newPosition = 0;
const double delta = 0.001963495; // This is [(2*pi) / 3200]

// Output Variables
double theta = 0; // initalizes the current theta var.
double old_theta = 0; // initalizes the old theta var.
double theta_vel = 0;
double voltage = 0;
double presentVoltage = 8.0;

// Other required variables
int period = 8;
boolean toggle = true;
double time_now = 0;

void loop() {
  time_now = (((int)millis())) / 1000.0;
  old_theta = theta;

  if(millis() > 2000) { // Stop the motor running after 2s
    analogWrite(PWM_Pin,0);
  }

  if (millis() > 1000 & toggle == true) { // Supply the motor 3V after 1 second
    toggle = false;
    voltage = 3;
    // 32 is approx. 1V out of 8V 
    analogWrite(PWM_Pin,((float)(voltage/presentVoltage))*255);  // PWM wave outputs to pin 11
  }
  
  // Encoder Part
  newPosition = myEnc.read();

  if (newPosition != oldPosition) { // if the wheel has moved
    if(newPosition > oldPosition) { // if going forward
      theta += (delta*(newPosition-oldPosition));               // increads theta by delta
    }
    else {                          // if going backwards
      theta = (theta - (delta*(oldPosition-newPosition)));      // decrease theta by delta
    }
    oldPosition = newPosition;      // sets new position for next loop
  }
  theta_vel = -1 * ((theta - old_theta) / (0.008));

  if(millis() > 900 && millis() < 2500) { // output data between .9s and 2.5s
    Serial.print(time_now,3);
    Serial.print('\t');
    Serial.print(voltage);
    Serial.print('\t');
    Serial.print(theta_vel,8);
    Serial.print('\t');
    Serial.print(newPosition);
    Serial.print('\t');
    Serial.println(theta);
  }
  
  
  while(millis() < time_now + period) { // set the loop to run at period speed of 8ms
    // This sets the duty cycle
  }
}
