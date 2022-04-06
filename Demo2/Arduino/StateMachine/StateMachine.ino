#include <Wire.h>
#include <StateMachine.h>

#define self_addr 0x04 // The i2c address that the arduino uses
#define block_mx 32    // Maximum size for an i2c message
#define message_length 1 // The peripheral will only ever send one byte
                         // to signal its status.
                

StateMachine machine = StateMachine();

//State Definitions
State* S0 = machine.addState(&state_outside);
State* S1 = machine.addState(&state_right);
State* S2 = machine.addState(&state_centered);
State* S3 = machine.addState(&state_left);
State* S4 = machine.addState(&state_exit_bottom);
State* S5 = machine.addState(&state_stop);


/**********************I2C Buffer variables**************************/
//A data buffer for incoming messages:
uint8_t recieved[block_mx];
//An index to know how much data is on the messsage buffer:
uint8_t recieved_length = 0;

uint8_t message[block_mx]; //A data buffer for outgoing messages


int PWM_Pin = 9; // PWM wave outputs to pin 9

void setup() {
  Serial.begin(57600);

  analogWrite(PWM_Pin,0);

  Wire.begin(0x04); // Initialize i2c communication

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}


void loop() {
  machine.run();
  delay(20);
}

void state_outside(){
  //Keep turning...
}

void state_right(){
  //Keep turning...
}

void state_centered(){
  //Go straight!
}

void state_left(){
  //Turn the other way
}

void state_exit_bottom(){
  //Go just a little further
}

void state_stop(){
  //Don't do anything
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
  if(i == 2) {
    Serial.print("Recieved ");
    Serial.println(recieved[1]);
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
