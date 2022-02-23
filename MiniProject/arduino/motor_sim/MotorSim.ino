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

int period = 8;
double time_now = 0;

void loop() {
  time_now = (((int)millis())) / 1000.0;
  old_theta = theta;

  if(millis() > 2000) {
    analogWrite(PWM_Pin,0);
  }

  if (millis() > 1000 & toggle == true) { // after 1 second
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

  if(millis() > 1000 && millis() < 2000) {
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
  
  
  while(millis() < time_now + period) {
    // This sets the duty cycle
  }
}
