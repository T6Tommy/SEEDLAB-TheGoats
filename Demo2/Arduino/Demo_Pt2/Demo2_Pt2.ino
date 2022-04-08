#include <Wire.h>
#include <Encoder.h>
Encoder myEncR(3, 5);
Encoder myEncL(2, 11);
#define self_addr 0x04 // The i2c address that the arduino uses
#define block_mx 32    // Maximum size for an i2c message
#define message_length 1 // The peripheral will only ever send one byte
                         // to signal its status.
/**********************I2C Buffer variables**************************/
//A data buffer for incoming messages:
uint8_t received[block_mx];
//An index to know how much data is on the messsage buffer:
uint8_t received_length = 0;

uint8_t message[block_mx]; //A data buffer for outgoing messages

int PWM_Pin = 9; // PWM wave outputs to pin 9

//encoder
long oldPositionL  = 0;
long oldPositionR  = 0;
long newPositionL = 0;
long newPositionR = 0;
double camera_theta = 0;



const double delta = 0.001963495; // This is [(2*pi) / 3200]
const double new_delta = 0.00582569; // cirumference / 3200
const double radius = 2.967; // constant for radius if wheels
const double betw = 5.23; // constant for distance between wheels
const double presentVoltage = 7.5;

int PWM_PinR = 9; // PWM wave outputs to pin 9 & 10
int PWM_PinL = 10; 


void setup() {
  Serial.begin(57600);
  
  Wire.begin(0x04); // Initialize i2c communication
  Wire.onReceive(receiveData);
  
  digitalWrite(4, HIGH);
  pinMode(PWM_PinR, OUTPUT);
  pinMode(PWM_PinL, OUTPUT);
  analogWrite(PWM_PinR,0);
  analogWrite(PWM_PinL,0);

}

// Variables 
double time_now = 0;
double time_after = 0;
int PWM_value = 0;

double current_theta = 0;
double old_theta = 0;
double vel_R = 0;
double vel_L = 0;

double current_pos = 0;
double old_pos = 0;
double changeR = 0;
double changeL = 0;

int cam_signal = 1;
int moveaft = 0;
bool hit_center = false;
double error = 0;

// Controller 
const double Ki = 0; // ------------------------
double I = 0;
double I_past = 0;
double voltage = 0;
const double Kp = 35; // ------------------------

bool cent = false;
bool nada = false;
bool endnow = false;

void loop() {
  time_now = (((int)millis())) / 1000.0;
  
  old_theta = current_theta;
  old_pos = current_pos;

  if(moveaft != 1 && !nada) {
    switch (cam_signal) {
      case 1: // Tape is not seen
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      analogWrite(PWM_PinR, 40);
      analogWrite(PWM_PinL, 40);
      break;
  
      case 2: // Tape is to the Left
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      analogWrite(PWM_PinR, 20);
      analogWrite(PWM_PinL, 20);
      break;
  
      case 3:
      // Centered - Go forward
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      analogWrite(PWM_PinR, 150);
      analogWrite(PWM_PinL, 150);
      hit_center = true;
      delay(500);
      break;
  
      case 4: // Tape is to the Right
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      analogWrite(PWM_PinR, 20);
      analogWrite(PWM_PinL, 20);
      break;
  
      case 5: // Stop
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      if(!nada) {
        analogWrite(PWM_PinR, 70);
        analogWrite(PWM_PinL, 70);
        delay(600);
      }
      analogWrite(PWM_PinR, 0);
      analogWrite(PWM_PinL, 0);
      nada = true;
      break;
    }
  }
  else if (moveaft == 0) {
    if (cam_signal == 1) {
      analogWrite(PWM_PinR, 0);
      analogWrite(PWM_PinL, 0);
      endnow = true;
    }
    else if (cam_signal == 3) {
      // Centered - Go forward
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      analogWrite(PWM_PinR, 150);
      analogWrite(PWM_PinL, 150);
      hit_center = true;
      delay(500);
    }
    else if (cam_signal == 2) {
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      analogWrite(PWM_PinR, 20);
      analogWrite(PWM_PinL, 20);
    }
    else if (cam_signal == 4) {
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      analogWrite(PWM_PinR, 20);
      analogWrite(PWM_PinL, 20);
    }
  }
  else if (moveaft == 1) {
    analogWrite(PWM_PinR, 0);
    analogWrite(PWM_PinL, 0);
  }

 /* else{
    if (cam_signal == 1) {
      analogWrite(PWM_PinR, 0);
      analogWrite(PWM_PinL, 0);
    }
    else if (cam_signal == 3) {
      // Centered - Go forward
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      analogWrite(PWM_PinR, 150);
      analogWrite(PWM_PinL, 150);
      hit_center = true;
      delay(500);
    }
    else if (cam_signal == 2) {
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      analogWrite(PWM_PinR, 20);
      analogWrite(PWM_PinL, 20);
    }
    else if (cam_signal == 4) {
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      analogWrite(PWM_PinR, 20);
      analogWrite(PWM_PinL, 20);
    }
  }*/
  
}

/**********************I2C Communication Methods**************************/

void receiveData(int byteCount) {
  //Serial.print("Data received:");
  uint8_t i = 0; // Index for the number of bytes received
  while (Wire.available()) {
    received[i] = Wire.read(); // take the next byte
    /*Serial.print("");
    Serial.print(received[i]); //print that byte as a hexadecimal
    Serial.print(" ");*/
    i++; // increment the index
  }
  received_length = i; // The index is now the length of the message
  //Serial.println(""); // end the line on the serial console
    // For each register, set the associated parameter:
    if (received[0] == 0) {
      Serial.println("Setting State:");
      Serial.println(received[1]);
      cam_signal = received[1];
    }
    if (received[0] == 0x01) {
      uint8_t value[4] = {received[2], received[3], received[4], received[5]};
      //Serial.println("Setting angle:");
      camera_theta = fromBytes(value);
      //Serial.println(camera_theta);
    }
    if (received[0] == 0x02) {
      Serial.println("Setting State:");
      Serial.println(received[1]);
      moveaft = received[1];
    }
    received_length = 0;
}

float fromBytes(uint8_t *input_array) {
  uint32_t input = 0; // Buffer for storing four bytes as one variable
  // This for loop "stacks" the bytes on top of each other
  // by shifting each byte by some multiple of eight and ORing
  // with the buffer:
  for (int i = 0; i < 4; i++) {
    input |= (uint32_t)input_array[i] << (8 * i);
  }
  // Copy the data at the input buffer location to a floating point
  // as if that the data at that location was a floating point.
  float output = *(float *)&input;
  return output;
}
