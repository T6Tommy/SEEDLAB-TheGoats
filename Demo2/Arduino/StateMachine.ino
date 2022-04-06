#include <Wire.h>
#include <StateMachine.h>

#define self_addr 0x04 // The i2c address that the arduino uses
#define block_mx 32    // Maximum size for an i2c message
#define message_length 1 // The peripheral will only ever send one byte
                         // to signal its status.
                

StateMachine machine = StateMachine();

//State Definitions

/**********************I2C Buffer variables**************************/
//A data buffer for incoming messages:
uint8_t received[block_mx];
//An index to know how much data is on the messsage buffer:
uint8_t received_length = 0;

uint8_t message[block_mx]; //A data buffer for outgoing messages
uint8_t cam_signal = 0;
float angle = 0;

int PWM_Pin = 9; // PWM wave outputs to pin 9

void setup() {
  Serial.begin(57600);

  analogWrite(PWM_Pin,0);

  Wire.begin(0x04); // Initialize i2c communication

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("Initialized");
}


void loop() {  
  delay(100);
  
  if (received_length) {
    // We expect bytes 3-6 to be a floating point value (4 bytes):
    uint8_t value[4] = {received[2], received[3], received[4], received[5]};
    // For each register, set the associated parameter:
    if (received[0] == 0x00) cam_signal = value;
    if (received[0] == 0x01) angle = fromBytes(value);
    received_length = 0;
  }
}

/**********************I2C Communication Methods**************************/

void receiveData(int byteCount) {
  Serial.print("Data received:");
  uint8_t i = 0; // Index for the number of bytes received
  while (Wire.available()) {
    received[i] = Wire.read(); // take the next byte
    Serial.print("0x");
    Serial.print(received[i], HEX); //print that byte as a hexadecimal
    Serial.print(" ");
    i++; // increment the index
  }
  received_length = i; // The index is now the length of the message
  Serial.println(""); // end the line on the serial console
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
