//#include "DualMC33926MotorShield.h"

#include <Wire.h>
#include <Encoder.h>
Encoder myEncR(3, 5);
Encoder myEncL(2, 6);

// pin 7 is H bridge for R
// pin 8 is H bridge for L

//Important constants for I2C:
#define PERIPH_ADDRESS 0x04
#define RECEIVED_MX 32

/**********************I2C Buffer variables**************************/
uint8_t received[RECEIVED_MX];
uint8_t received_amt = 0;



int PWM_PinR = 9; // PWM wave outputs to pin 9 & 10
int PWM_PinL = 10; 

void setup() {
  Serial.begin(57600);
  digitalWrite(4, HIGH);
  pinMode(PWM_PinR, OUTPUT);
  pinMode(PWM_PinL, OUTPUT);
  analogWrite(PWM_PinR,0);
  analogWrite(PWM_PinL,0);

  Wire.begin(PERIPH_ADDRESS); // Initialize I2C
  Wire.onReceive(receiveEvent); // Define interupt method for receiving data
  Serial.println("Initialization complete."); // Let the user know we're ready.
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
double theta_screen_old = 0;

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
double error = 0;
float desired_pos_old = 0;

void loop() {

  time_now = (((int)millis())) / 1000.0;

  if (received_amt) {
    // We expect bytes 2-9 to be a double floating point value (8 bytes):
    uint8_t value[8] = {received[2], received[3], received[4], received[5],
                        received[6], received[7], received[8], received[9]};
    // For each register, set the associated parameter:
    if (received[0] == 0x01) desired_pos = fromBytes(value);
    if (received[0] == 0x02) theta_screen = fromBytes(value);
    received_amt = 0;
  }

  // If a parameter changed, let the user know:
  if (theta_screen_old != theta_screen) {
    Serial.print("theta_screen: ");
    Serial.println(theta_screen);
    theta_screen_old = theta_screen;
  }
  if (desired_pos_old != desired_pos) {
    Serial.print("desired_pos: ");
    Serial.println(desired_pos);
    desired_pos_old = desired_pos;
  }

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

void receiveEvent(int count) {
  Serial.print("Data received:");
  uint8_t i = 0; // Index for the number of bytes received
  while (Wire.available()) {
    received[i] = Wire.read(); // take the next byte
    Serial.print("0x");
    Serial.print(received[i], HEX); //print that byte as a hexadecimal
    Serial.print(" ");
    i++; // increment the index
  }
  received_amt = i; // The index is now the length of the message
  Serial.println(""); // end the line on the serial console
}

float fromBytes(uint8_t *input_array) {
  uint32_t input = 0; // Buffer for storing four bytes as one variable
  // This for loop "stacks" the bytes on top of each other
  // by shifting each byte by some multiple of eight and ORing
  // with the buffer:
  for (int i = 0; i < 8; i++) {
    input |= (uint64_t)input_array[i] << (8 * i);
  }
  // Copy the data at the input buffer location to a floating point
  // as if that the data at that location was a floating point.
  float output = *(float *)&input;
  return output;
}
