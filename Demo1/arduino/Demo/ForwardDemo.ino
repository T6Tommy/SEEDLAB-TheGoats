#include <Wire.h>
#include <Encoder.h>
Encoder myEncR(3, 5);
Encoder myEncL(2, 11);

// pin 7 is H bridge for R
// pin 8 is H bridge for L

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

// Variables 
const double delta = 0.001963495; // This is [(2*pi) / 3200]
const double new_delta = 0.00582569;
const double radius = 2.967; // constant for radius if wheels
const double betw = 5.23; // constant for distance between wheels
const double presentVoltage = 7.5;
double time_now = 0;
double time_after = 0;

// running
double voltage = 0;
double voltageL = 0;
double voltageR = 0;
double PWM_value = 0;

//encoder
long oldPositionL  = 0;
long oldPositionR  = 0;
long newPositionL = 0;
long newPositionR = 0;

// distance 
double current_pos = 0;
double desired_pos = 84; // in inches 
double old_pos = 0;
double changeR = 0;
double changeL = 0;
double error = 0;

// PI Controller 
const double KpF = 3;
const double KiF = 0;
double I_pastF = 0;
double IF = 0;


void loop() {
  time_now = (((int)millis())) / 1000.0;
  old_pos = current_pos;

  // PI Controller
  error = desired_pos - current_pos;
  IF = I_pastF+(error*0.008);
  I_pastF = IF;
  voltage = (KpF * error) + (KiF *IF);
  PWM_value = ((voltage/presentVoltage))*255;
  if (PWM_value > 50) {
    PWM_value = 50;
  }
  else if (PWM_value < -50) {
    PWM_value = -50;
  }
  if (PWM_value >= 0) {
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  }
  else if (PWM_value < 0) {
    digitalWrite(8, HIGH);
    digitalWrite(7, HIGH);
  }
  if (changeL > changeR) {
    analogWrite(PWM_PinR, abs(PWM_value)+14);
    analogWrite(PWM_PinL, abs(PWM_value));
  }
  else if (changeR > changeL) {
    analogWrite(PWM_PinL, abs(PWM_value)+5);
    analogWrite(PWM_PinR, abs(PWM_value));
  }
  else {
    analogWrite(PWM_PinR, abs(PWM_value)+4);
    analogWrite(PWM_PinL, abs(PWM_value));
  }
  Serial.print(PWM_value);
  Serial.print('\t');
  

  // Encoder -----------------------------------------------------------------
  newPositionL = myEncL.read();
  newPositionR = myEncR.read();
  if (newPositionL != oldPositionL) { // if the wheel has moved
    changeL = (newPositionL - oldPositionL) * new_delta;
  }
  if (newPositionR != oldPositionR) { // if the wheel has moved
    changeR = (newPositionR - oldPositionR) * new_delta;
  }
  current_pos = old_pos + ((changeL + changeR) / 2);
  oldPositionR = newPositionR;
  oldPositionL = newPositionL;
  Serial.print(error);
  Serial.print("\t");
  Serial.print(newPositionL);
  Serial.print("\t");
  Serial.print(newPositionR);
  Serial.print("\t");
  Serial.print(changeL);
  Serial.print("\t");
  Serial.print(changeR);
  Serial.print("\t");
  Serial.println(current_pos);

  time_after = (((int)millis())) / 1000.0;
  delay(8 - (.001*(time_after - time_now)));
}
