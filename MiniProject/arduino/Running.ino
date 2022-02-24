#include "DualMC33926MotorShield.h"

DualMC33926MotorShield md;

#include <Encoder.h>
Encoder myEnc(3, 4);

int PWM_Pin = 9; // PWM wave outputs to pin 9

void setup() {
  Serial.begin(250000);
  pinMode(PWM_Pin, OUTPUT);
  analogWrite(PWM_Pin,0);
  md.init();
}

long oldPosition  = -999;
long newPosition = 0;
const double delta = 0.001963495; // This is [(2*pi) / 3200]
double theta = 0; // initalizes the current theta var.
double old_theta = 0; // initalizes the old theta var.
double theta_vel = 0;
double voltage = 0;
boolean toggle = true;
double presentVoltage = 8.0;
int PWM_value = 0;

int period = 8;
double time_now = 0;
double time_after = 0;
double desired_theta = 0;
double desired_pos = 0;
double error = 0;
const double Kp = 1.32;
const double Ki = 0.33;
double I_past = 0;
double I = 0;


void loop() {
  
  time_now = (((int)millis())) / 1000.0;
  old_theta = theta;

  // Kp part
  desired_pos = desired_theta * 800;
  error = (desired_pos - newPosition);
  
  // Ki pat
  I = I_past+((error*delta)*0.008);
  I_past = I;
  
  voltage = (Kp * (error * delta)) + (Ki * I);
  
  PWM_value = (voltage/presentVoltage))*255
  
  /// implement directional pin -----------------------------------
  
  if (PWM_value > 255) {
    PWM_value = 255;
  }
  else if (PWM_value < 0) {
    PWM_value = 0;
  }
  
  analogWrite(PWM_Pin, PWM_value);
  
  // Encoder Part
  newPosition = myEnc.read();

  if (newPosition != oldPosition) { // if the wheel has moved
    if(newPosition > oldPosition) { // if going forward
      theta += (delta*((float)(newPosition-oldPosition)));               // increads theta by delta
    }
    else {                          // if going backwards
      theta = (theta - (delta*((float)(oldPosition-newPosition))));      // decrease theta by delta
    }
    oldPosition = newPosition;      // sets new position for next loop
  }
  time_after = (((int)millis())) / 1000.0;

  delay(8 - (.001*(time_after - time_now)));
}
