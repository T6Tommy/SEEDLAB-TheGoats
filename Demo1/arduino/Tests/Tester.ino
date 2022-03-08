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
const double radius = 2.967; // constant for radius if wheels
const double betw = 10.46; // constant for distance between wheels
double time_now = 0;
double time_after = 0;

double desired_theta = 0;
double current_theta = 0;
double old_theta = 0;

long oldPositionL  = -999;
long oldPositionR  = -999;
long newPositionL = 0;
long newPositionR = 0;

double presentVoltage = 8.0;
double voltage = 0;
int PWM_value = 0;
int period = 8;

double error = 0;
double vel_R = 0;
double vel_L = 0;

const double KpR = 10;
const double KiR = 0;
double I_pastR = 0;
double IR = 0;

void loop() {
  time_now = (((int)millis())) / 1000.0;
  old_theta = current_theta;
  if (millis() >= 1000) {
    desired_theta = 3.14;
  }
  Serial.print(error);
  Serial.print('\t');
  
  IR = I_pastR+(error*0.008);
  I_pastR = IR;
  
  voltage = (KpR * error) + (KiR *IR);
  
  PWM_value = ((voltage/presentVoltage))*255;
  if (PWM_value > 255) {
    PWM_value = 255;
  }
  else if (PWM_value < -255) {
    PWM_value = -255;
  }
  if (PWM_value >= 0) {
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
  }
  else if (PWM_value < 0) {
    digitalWrite(8, HIGH);
    digitalWrite(7, LOW);
  }
  analogWrite(PWM_PinR, abs(PWM_value));
  analogWrite(PWM_PinL, abs(PWM_value)); 
  


  // encoder part
  newPositionL = myEncL.read();
  newPositionR = myEncR.read();
  if (newPositionL != oldPositionL) { // if the wheel has moved
    vel_L = (newPositionL - oldPositionL)*delta / 0.008;
  }
  else {
    vel_L = 0;
  }
  if (newPositionR != oldPositionR) { // if the wheel has moved
    vel_R = (newPositionR - oldPositionR)*delta / 0.008;
  }
  else {
    vel_R = 0;
  }
  current_theta = old_theta + (0.008)*(radius/betw)*(vel_L-vel_R);
  Serial.print(vel_L);
  Serial.print("\t");
  Serial.print(vel_R);
  Serial.print("\t");
  Serial.println(current_theta);

  oldPositionL = newPositionL;
  oldPositionR = newPositionR;
  time_after = (((int)millis())) / 1000.0;
  delay(8 - (.001*(time_after - time_now)));
}
