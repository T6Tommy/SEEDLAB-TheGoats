#include "DualMC33926MotorShield.h"

DualMC33926MotorShield md;

#include <Wire.h>
#include <Encoder.h>
Encoder myEnc(3, 5);

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


int PWM_Pin = 9; // PWM wave outputs to pin 9

void setup() {
  Serial.begin(57600);
  digitalWrite(4, HIGH);
  pinMode(PWM_Pin, OUTPUT);
  analogWrite(PWM_Pin,0);
  md.init();

  analogWrite(PWM_Pin,0);

  Wire.begin(0x04); // Initialize i2c communication

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

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
const double Kp = 3;
const double Ki = 0;
double I_past = 0;
double I = 0;


void loop() {
  //Serial.print(desired_theta);
  //Serial.print(" ");
  time_now = (((int)millis())) / 1000.0;
  old_theta = theta;

  // Kp part
  desired_pos = desired_theta * 509;
  error = (desired_pos - newPosition);
  //Serial.print(" ");
  
  // Ki pat
  I = I_past+((error*delta)*0.008);
  I_past = I;
  
  voltage = (Kp * (error * delta)) + (Ki * I);
  
  PWM_value = ((voltage/presentVoltage))*255;
  
  /// implement directional pin -----------------------------------
  
  if (PWM_value > 255) {
    PWM_value = 255;
  }
  else if (PWM_value < -255) {
    PWM_value = -255;
  }
  Serial.print(desired_pos);
  Serial.print(" ");
  Serial.print(newPosition);
  Serial.print(" ");
  Serial.print(error);
  Serial.print(" ");
  Serial.println(PWM_value);
  if (PWM_value >= 0)  
    digitalWrite(7, LOW);
  if (PWM_value < 0)
    digitalWrite(7, HIGH);
  
  analogWrite(PWM_Pin, abs(PWM_value));
  
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
