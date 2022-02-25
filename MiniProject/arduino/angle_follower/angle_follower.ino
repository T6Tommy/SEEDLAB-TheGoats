#define motor_pin 11 // PWM wave outputs to pin 11
#include <Wire.h>

void setup() {
  Serial.begin(57600);
  pinMode(motor_pin, OUTPUT); // Initialize motor PWM:
  analogWrite(motor_pin,0);

  Wire.begin(0x04); // Initialize i2c communication

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}

void loop() {
  // put your main code here, to run repeatedly:

}
