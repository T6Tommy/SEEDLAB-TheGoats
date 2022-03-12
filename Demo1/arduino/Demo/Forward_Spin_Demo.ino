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
bool SPIN = true;
bool FORWARD = false;

//encoder
long oldPositionL  = 0;
long oldPositionR  = 0;
long newPositionL = 0;
long newPositionR = 0;

// theta
double desired_theta = 3.19;
double current_theta = 0;
double old_theta = 0;
double vel_R = 0;
double vel_L = 0;

// distance 
double current_pos = 0;
double desired_pos = 36; // in inches 
double old_pos = 0;
double changeR = 0;
double changeL = 0;
double error = 0;

// PI Controller 
const double KpS = 5;
const double KiS = 0.4;
double I_pastS = 0;
double IS = 0;
const double KpF = 0.3;
const double KiF = 0;
double I_pastF = 0;
double IF = 0;

void loop() {
  time_now = (((int)millis())) / 1000.0;

  old_theta = current_theta;
  old_pos = current_pos;

  if (SPIN) {
    Serial.print("SPIN");
    Serial.print('\t');
    // PI
    error = desired_theta - current_theta;
    IS = I_pastS+(error*0.008);
    I_pastS = IS;
    
    voltage = (KpS * error) + (KiS *IS);
    
    PWM_value = ((voltage/presentVoltage))*255;
    if (PWM_value > 50) {
      PWM_value = 50;
    }
    else if (PWM_value < -50) {
      PWM_value = -50;
    }
    if (PWM_value >= 0) {
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
    }
    else if (PWM_value < 0) {
      digitalWrite(8, LOW);
      digitalWrite(7, HIGH);
    }
    analogWrite(PWM_PinR, abs(PWM_value));
    analogWrite(PWM_PinL, abs(PWM_value+3));

    if (error < 0.01 & error > -0.01) {  // need to add error bounds to this ----------------------------------
      SPIN = false;
      FORWARD = true;
      analogWrite(PWM_PinR, 0);
      analogWrite(PWM_PinL, 0); 
      delay(400);
    }
  }
  else if (FORWARD) {
    Serial.print("FORWARD");
    Serial.print('\t');
    // PI Controller
    error = desired_pos - current_pos;
    IF = I_pastF+(error*0.008);
    I_pastF = IF;
    voltage = (KpF * error) + (KiF *IF);
    PWM_value = ((voltage/presentVoltage))*255;
    if (PWM_value > 100) {
      PWM_value = 100;
    }
    else if (PWM_value < -100) {
      PWM_value = -100;
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
      analogWrite(PWM_PinR, abs(PWM_value+5));
      analogWrite(PWM_PinL, abs(PWM_value));
    }
    else if (changeR > changeL) {
      analogWrite(PWM_PinL, abs(PWM_value+5));
      analogWrite(PWM_PinR, abs(PWM_value));
    }
    else {
      analogWrite(PWM_PinR, abs(PWM_value+2));
      analogWrite(PWM_PinL, abs(PWM_value));
    }
    
  }

  // encoder --------------------------------------------------------
  newPositionL = myEncL.read();
  newPositionR = myEncR.read();
  vel_L = 0;
  if (newPositionL != oldPositionL) { // if the wheel has moved
    vel_L = (newPositionL - oldPositionL)*delta / 0.008;
    changeL = (newPositionL - oldPositionL) * new_delta;
  }
  else {
    vel_L = 0;
  }
  vel_R = 0;
  if (newPositionR != oldPositionR) { // if the wheel has moved
    vel_R = (newPositionR - oldPositionR)*delta / 0.008;
    changeR = (newPositionR - oldPositionR) * new_delta;
  }
  else {
    vel_R = 0;
  }
  current_theta = old_theta + (0.008)*(radius/betw)*(vel_L-vel_R)/2;
  current_pos = old_pos + ((changeL + changeR) / 2);

  Serial.print(vel_L);
  Serial.print('\t');
  Serial.print(vel_R);
  Serial.print('\t');
  Serial.print(error);
  Serial.print('\t');
  Serial.print(desired_theta);
  Serial.print('\t');
  Serial.println(current_theta);
  oldPositionR = newPositionR;
  oldPositionL = newPositionL;
  
  time_after = (((int)millis())) / 1000.0;
  delay(8 - (.001*(time_after - time_now)));
}
