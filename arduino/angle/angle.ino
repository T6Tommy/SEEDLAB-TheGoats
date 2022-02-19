#include <Wire.h>
#define self_addr 0x04
#define block_mx 32
#define message_length 1

//A data buffer for incoming messages:
uint8_t recieved[block_mx];

//An index to know how much data is on the messsage buffer:
uint8_t recieved_length = 0;

uint8_t angle = 0;

uint8_t message[block_mx]; //A data buffer for outgoing messages

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Wire.begin(self_addr); // Initialize i2c communication

    // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}

void receiveData(int byteCount) {
  Serial.print("data recieved:");
  uint8_t i = 0; //Index, which serves as both a length of the recieved message and a way to know whether to read or write data:
  
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
      angle = recieved[1]*90;
      Serial.print("Turning the wheel to ");
      Serial.print(angle);
      Serial.println(" degrees.");
    }
  }
}


void writeData() {
   // No errors encountered.
   message[0] = 0x00;
   for(int i = 0; i < message_length; i++) {
    Serial.print(message[i]); // Report the values being sent via I2C to the user via serial
    Serial.print(' ');
   }
   Serial.println(' ');
}

void sendData() {

  writeData(); // Write the pot value to the message buffer
  
  Serial.print("Sending data: ");
  for(int i = 0; i < message_length; i++) {
    Wire.write(message[i]); // send the contents of the message buffer to the i2c controller
    Serial.print(message[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}
