#include <Wire.h>
#include <Encoder.h>
Encoder myEncR(3, 5);
Encoder myEncL(2, 11);

int PWM_PinR = 9; // PWM wave outputs to pin 9 & 10
int PWM_PinL = 10;

void setup() {
  Serial.begin(57600);
  digitalWrite(4, HIGH);
  pinMode(PWM_PinR, OUTPUT);
  pinMode(PWM_PinL, OUTPUT);
  analogWrite(PWM_PinR,0);
  analogWrite(PWM_PinL,0);
}

const double delta = 0.001963495; // This is [(2*pi) / 3200]
const double new_delta = 0.00582569;
const double radius = 2.967; // constant for radius if wheels
const double betw = 10.46; // constant for distance between wheels
double time_now = 0;
double time_after = 0;

long oldPositionL  = 0;
long oldPositionR  = 0;
long newPositionL = 0;
long newPositionR = 0;

double presentVoltage = 7.7;
double voltage = 0;
int PWM_value = 0;
int period = 8;

double error = 0;
double changeL = 0;
double changeR = 0;
double current_pos = 0;
double desired_pos = 0;
double old_pos = 0;

const double KpR = 10;

void loop() {
  if (millis() > 1000) {
    desired_pos = 24;
  }
  time_now = (((int)millis())) / 1000.0;

  error = desired_pos - current_pos;

  voltage = (KpR * error);
  
  PWM_value = ((voltage/presentVoltage))*255;
  if (PWM_value >= 0) {
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  }
  else if (PWM_value < 0) {
    digitalWrite(8, HIGH);
    digitalWrite(7, HIGH);
  }
  if (PWM_value > 255) {
    PWM_value = 255;
  }
  if (PWM_value < -255) {
    PWM_value = -255;
  }
  analogWrite(PWM_PinR, abs(PWM_value));
  analogWrite(PWM_PinL, abs(PWM_value)); 

  // Encoder -----------------------------------------------------------
  newPositionL = myEncL.read();
  newPositionR = myEncR.read();
  if (newPositionL != oldPositionL) { // if the wheel has moved
    changeL = (newPositionL - oldPositionL) * new_delta;
  }
  if (newPositionR != oldPositionR) { // if the wheel has moved
    changeR = (newPositionR - oldPositionR) * new_delta;
  }
  current_pos = old_pos + ((changeL + changeR) / 2);

  Serial.print(changeL);
  Serial.print('\t');
  Serial.print(changeR);
  Serial.print('\t');
  Serial.print(desired_pos);
  Serial.print('\t');
  Serial.print(current_pos);
  Serial.print('\t');
  Serial.println(error);

  time_after = (((int)millis())) / 1000.0;
  delay(8 - (.001*(time_after - time_now)));
}
