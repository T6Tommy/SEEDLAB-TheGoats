//#include "DualMC33926MotorShield.h"

#include <Wire.h>
#include <Encoder.h>
Encoder myEncR(3, 5);
Encoder myEncL(2, 6);

// pin 7 is H bridge for R
// pin 8 is H bridge for L

#define self_addr 0x04 // The i2c address that the arduino uses
#define block_mx 32    // Maximum size for an i2c message
#define message_length 1 // The peripheral will only ever send one byte
                         // to signal its status.


/**********************I2C Buffer variables**************************/
//A data buffer for incoming messages:
uint8_t recieved[block_mx];
//An index to know how much data is on the messsage buffer:
uint8_t recieved_length = 0;

uint8_t message[block_mx]; //A data buffer for outgoing messages


int PWM_PinR = 9; // PWM wave outputs to pin 9 & 10
int PWM_PinL = 10; 

void setup() {
  Serial.begin(57600);
  digitalWrite(4, HIGH);
  pinMode(PWM_PinR, OUTPUT);
  pinMode(PWM_PinL, OUTPUT);
  analogWrite(PWM_PinR,0);
  analogWrite(PWM_PinL,0);

  Wire.begin(0x04); // Initialize i2c communication

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

}

long oldPositionL  = -999;
long oldPositionR  = -999;
long newPositionL = 0;
long newPositionR = 0;

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

double theta_screen = 0; // theta from arduino

const double KpR = 10;
const double KiR = 0;
double I_pastR = 0;
double IR = 0;
const double KpF = 10;
const double KiF = 0;
double I_pastF = 0;
double IF = 0;

const double radius = 2.967; // constant for radius if wheels
const double betw = 5.23; // constant for distance between wheels
bool FORWARD = false;
bool SPIN = false;

double vel_R = 0;
double vel_L = 0;
double Rot_vel = 0;

double current_pos = 0;
double desired_pos = 0;

void loop() {

  time_now = (((int)millis())) / 1000.0;

  if (SPIN) {
    
    IR = I_pastR+(theta_screen*0.008);
    I_pastR = IR;
    
    voltage = (KpR * theta_screen) + (KiR *IR);
    
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

    if (theta_screen == 0) {  // need to add error bounds to this ----------------------------------
      SPIN = false;
      analogWrite(PWM_PinR, 0);
      analogWrite(PWM_PinL, 0); 
      delay(400);
    }
  } // ---------------------------------------------------------------------------------------------------------------------
  else if (FORWARD) {
    error = desired_pos - current_pos;
    
    IF = I_pastF+(error*0.008);
    I_pastF = IF;
    
    voltage = (KpF * theta_screen) + (KiF *IF);
    if (PWM_value >= 0) {
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
    }
    else if (PWM_value < 0) {
      digitalWrite(8, HIGH);
      digitalWrite(7, HIGH);
    }
    
    PWM_value = ((voltage/presentVoltage))*255;
    if (PWM_value > 255) {
      PWM_value = 255;
    }
    else if (PWM_value < -255) {
      PWM_value = -255;
    }
    analogWrite(PWM_PinR, abs(PWM_value));
    analogWrite(PWM_PinL, abs(PWM_value)); 
  }
  else {
    current_pos = 0;
    if (theta_screen > 0 | theta_screen < 0) {
      SPIN = true;
    }
    else if ((desired_pos - current_pos) > 0 | (desired_pos - current_pos) < 0) {
      error = desired_pos - current_pos;
      FORWARD = true;
    }
  }
  

  // Encoder 
  newPositionL = myEncL.read();
  newPositionR = myEncR.read();
  if (newPositionL != oldPositionL) { // if the wheel has moved
    vel_L = (newPositionL - oldPositionL)*delta / 0.008;
    changeL = (newPositionL - oldPositionL) * new_delta;
  }
  else {
    vel_L = 0;
  }
  if (newPositionR != oldPositionR) { // if the wheel has moved
    vel_R = (newPositionR - oldPositionR)*delta / 0.008;
    changeR = (newPositionR - oldPositionR) * new_delta;
  }
  else {
    vel_R = 0;
  }
  current_theta = old_theta + (0.008)*(radius/betw)*(vel_L-vel_R);
  current_pos = old_pos + ((changeL + changeR) / 2);
  
  time_after = (((int)millis())) / 1000.0;
  delay(8 - (.001*(time_after - time_now)));
}


/**********************I2C Communication Methods**************************/

void receiveData(int byteCount) {
  Serial.print("data recieved:");
  uint8_t i = 0; //Index: tracks the used space in the message
  
  //The message should be no longer than 32 values:
  while(Wire.available() && (i < block_mx)) {
     recieved[i] = Wire.read(); // Store to message buffer
     Serial.print(recieved[i]); // Print that value to serial
     Serial.print(' ');
     i++; //increment index for next value
  }
  Serial.println(' ');
  //recieved_length = i;
  Serial.println(i);
  if((i == 2)&&(recieved[1] != 0)) {
    desired_theta = (recieved[1]-1)*1.57079632679;
    Serial.print("Turning the wheel to ");
    Serial.println(" radians.");
  }
}

// sendData sends the arduino's status back to the Raspberry Pi. Currently it only
// sends a 0 for "OK"
void sendData() {
  message[0] = 0x00;
  Serial.print("Sending data: ");
  for(int i = 0; i < message_length; i++) {
    Wire.write(message[i]); // send the contents of the message buffer to the i2c controller
    Serial.print(message[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
} 
