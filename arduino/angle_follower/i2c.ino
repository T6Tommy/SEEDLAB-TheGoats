#include <Wire.h>

#define self_addr 0x04 // The i2c address that the arduino uses
#define block_mx 32    // Maximum size for an i2c message
#define message_length 1 // The peripheral will only ever send one byte
                         // to signal its status.

float theta_target = 0;

/**********************I2C Buffer variables**************************/
//A data buffer for incoming messages:
uint8_t recieved[block_mx];
//An index to know how much data is on the messsage buffer:
uint8_t recieved_length = 0;
uint8_t message[block_mx]; //A data buffer for outgoing messages

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
  recieved_length = i;
  if(recieved_length == 0) {
    delay(2000);
    sendData(); //If there is 1 value in the buffer, we are being asked to send data.
  }
  else {
    if(recieved[0] = 0 && recieved_length == 1) {
      theta_target = recieved[1]*1.57079632679;
      Serial.print("Turning the wheel to ");
      Serial.print(theta_target);
      Serial.println(" radians.");
    }
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
