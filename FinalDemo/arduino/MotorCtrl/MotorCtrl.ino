#include <Wire.h>
#include <Encoder.h>
#include <StateMachine.h>

#include "TurnCtrl.h"
#include "VelCtrl.h"
#include "constants.h"

Encoder myEncR(3, 5);
Encoder myEncL(2, 11);

float magnitude_voltage = 0;  //Voltage magnitude, intended for moving the plant forward
uint8_t motor_PWM = 0; //The unsigned integer to be sent to the PWM ports connected to the motors.

uint8_t received[RECEIVED_MX];
uint8_t received_amt = 0;

uint8_t report_counter = 0; // When it reaches 13, data is reported to the serial console.

bool is_tape_detected = 0;
bool is_cross_detected = 0;

float angle_bounds = 0.1;
float angle_desired = 0;
float velocity_desired = 0.08;
float vel_l = 0;
float vel_r = 0;
double delta_t = 0;
long time_stamp[2] = {0, 0};

Turn_Ctrl turn_ctrl;
Vel_Ctrl vel_ctrl;

int pos_l[2] = {0, 0};
int pos_r[2] = {0, 0};

Encoder enc_r(3,5);
Encoder enc_l(2,11);

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

void report_data() {
  Serial.print(delta_t, 8);
  Serial.print("\t");
  Serial.print(vel_l);
  Serial.print("\t");
  Serial.print(vel_r);
  Serial.print("\t");
  Serial.print(turn_ctrl.get_phi());
  Serial.print("\t");
  Serial.print(turn_ctrl.get_phi_error());
  Serial.print("\t");
  Serial.print(turn_ctrl.delta_v[1]);
  Serial.println("");
}

StateMachine comm = StateMachine();

bool is_tape_within_bounds() {
  if(abs(angle_desired) < angle_bounds) return true;
  else return false;
}

// States for the communications machine
void state_parse() {
  // We expect bytes 3-6 to be a floating point value (4 bytes):
  uint8_t value[4] = {received[2], received[3], received[4], received[5]};
  // For each register, set the associated parameter:
  if (received[0] == 0x01) velocity_desired = fromBytes(value);
  if (received[0] == 0x02) angle_desired = fromBytes(value);
  if (received[0] == 0x03) is_tape_detected = received[2];
  if (received[0] == 0x04) is_cross_detected = received[2];
  received_amt = 0;
}
void state_new_target() {
  turn_ctrl.reset_phi();
 }
void state_report() {
  report_data();
 }

void state_standby() {
  report_counter++;
}

bool transitionSsbSp() {
  if (received_amt) return true;
  else return false;
}

bool transitionSsbSr() {
  if (report_counter >= 13) {
    report_counter = 0;
    return true;
  }
  else return false;
}

bool transitionSsb() {
  if(!received_amt && (report_counter < 13)) return true;
  else return false;
}

bool transitionSpSnt() {
  if(turn_ctrl.is_error_0()) return true;
  else return false;
}

bool transitionSpSsb() {
  if(!turn_ctrl.is_error_0()) return true;
  else return false;
}

bool transitionSntSsb() {
  return true;
}

bool transitionSrSsb() {
  return true;
}

// States for the actuation machine
void state0() {
  delay(100);
}

void state1() {
  turn_ctrl.tick(&angle_atom, &delta_t, &vel_l, &vel_r);
  update_motors(turn_ctrl.delta_v[1]);
}
void state2() {
  turn_ctrl.tick(&angle_desired, &delta_t, &vel_l, &vel_r);
  update_motors(turn_ctrl.delta_v[1]);
  }
void state3() {
  vel_ctrl.tick(&velocity_desired, &delta_t, &vel_l, &vel_r);
  update_motors(vel_ctrl.mag_v[1]);
  }
void state4() {
  delay(1000);
  }
void state5() {
  vel_ctrl.tick(&velocity_desired, &delta_t, &vel_l, &vel_r);
  update_motors(vel_ctrl.mag_v[1]);
  }
void state6() {
  velocity_desired = 0;
  vel_ctrl.tick(&velocity_desired, &delta_t, &vel_l, &vel_r);
  update_motors(vel_ctrl.mag_v[1]);
  }

bool transitionS0() {
  if(!angle_desired)
    return true;  // Continue to wait for data...
  else return false;
}
bool transitionS1() {
  if(turn_ctrl.is_error_0() && !is_tape_detected) turn_ctrl.reset_phi();
  return !is_tape_detected;  // Continue to turn until tape appears...
}
bool transitionS2() {
  if(!turn_ctrl.is_error_0()) {
    turn_ctrl.reset_phi();
    return true;
  }
  else return false;  // Continue to turn until the tape is centered...
}
bool transitionS3() {
  if(is_tape_detected && is_tape_within_bounds) return true;
  else return false;  // Continue to move forward...
}
bool transitionS5() {
  if(is_cross_detected) return true;
  else return false;
}
bool transitionS6() {
  return true;
}

// S0->Si
bool transitionS0S1() {
  if(angle_desired && !is_tape_detected) return true;  // Data has been recieved, and no tape is detected.  Turn.
  else return false;
}
bool transitionS0S2() {
  if(angle_desired && is_tape_detected && !is_tape_within_bounds) return true;
  else return false;  // Data has been recieved, and the tape is off center. Turn.
}
bool transitionS0S3() {
  if(angle_desired && is_tape_detected && is_tape_within_bounds) return true;
  else return false;  // Data has been recieved, and the tape is centered. Forward.
}

// S1->Si
bool transitionS1S2() {
  if(is_tape_detected && !is_tape_within_bounds) return true;
  else return false;  // The tape is now visible. Turn to center it.
}
bool transitionS1S3() {
  if(is_tape_detected && is_tape_within_bounds) return true;
  else return false;  // The tape is now centered. Move forward.
}

// S2->Si
bool transitionS2S3() {
  if(is_tape_detected && is_tape_within_bounds) return true;
  else return false;  // The tape is now centered. Move forward.
}
bool transitionS2S1() {
  if(!is_tape_detected) return true;
  else return false;  // The tape has moved out of view. Search for the tape.
}

// S3->Si
bool transitionS3S2() {
  if(is_tape_detected && !is_tape_within_bounds) return true;
  else return false;  // The tape is off-center.  Center it.
}
bool transitionS3S4() {
  if(!is_tape_detected) return true;
  else return false;  // The tape has disappeared from view. We have probably reached the cross.
}

// S4->Si
bool transitionS4S5() {
  return is_cross_detected;  // We have detected the cross. Move forward until we don't see it anymore.
}
bool transitionS4S6() {
  return !is_cross_detected;  // If you don't see the cross at this point, STOP!!!
}

// S5->Si
bool transitionS5S6() {
  return !is_cross_detected;
}


State* Sp = comm.addState(&state_parse);
State* Snt = comm.addState(&state_new_target);
State* Sr = comm.addState(&state_report);
State* Ssb = comm.addState(&state_standby);

bool is_actuate_per_elapsed = 1;

StateMachine actuate = StateMachine();

State* S0 = actuate.addState(&state0);
State* S1 = actuate.addState(&state1);
State* S2 = actuate.addState(&state2);
State* S3 = actuate.addState(&state3);
State* S4 = actuate.addState(&state4);
State* S5 = actuate.addState(&state5);
State* S6 = actuate.addState(&state6);

void calc_deltas() {
  pos_l[0] = pos_l[1];
  pos_r[0] = pos_r[1];
  time_stamp[0] = time_stamp[1];
  
  pos_l[1] = enc_l.read();
  pos_r[1] = enc_r.read();
  time_stamp[1] = micros();
  delta_t = double(time_stamp[1] - time_stamp[0]) / 1000000;
  
  vel_l = (pos_l[1]-pos_l[0])*rad_enc_step/delta_t;
  vel_r = (pos_r[1]-pos_r[0])*rad_enc_step/delta_t;
}

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


void transitions_init() {
  // Self-transitions
  S0->addTransition(&transitionS0, S0); // Continue to wait for data...
  S1->addTransition(&transitionS1, S1); // Continue to turn until tape appears...
  S2->addTransition(&transitionS2, S2); // Continue to turn until the tape is centered...
  S3->addTransition(&transitionS3, S3); // Continue to move forward...
  S5->addTransition(&transitionS5, S5);
  S6->addTransition(&transitionS6, S6);

  // S0->Si
  S0->addTransition(&transitionS0S1, S1); // Data has been recieved, and no tape is detected.  Turn.
  S0->addTransition(&transitionS0S2, S2); // Data has been recieved, and the tape is off center. Turn.
  S0->addTransition(&transitionS0S3, S3); // Data has been recieved, and the tape is centered. Forward.

  // S1->Si
  S1->addTransition(&transitionS1S2, S2); // The tape is now visible. Turn to center it.
  S1->addTransition(&transitionS1S3, S3); // The tape is now centered. Move forward.

  // S2->Si
  S2->addTransition(&transitionS2S3, S3); // The tape is now centered. Move forward.
  S2->addTransition(&transitionS2S1, S1); // The tape has moved out of view. Search for the tape.

  // S3->Si
  S3->addTransition(&transitionS3S2, S2); // The tape is off-center.  Center it.
  S3->addTransition(&transitionS3S4, S4); // The tape has disappeared from view. We have probably reached the cross.

  // S4->Si
  S4->addTransition(&transitionS4S5, S5); // We have detected the cross. Move forward until we don't see it anymore.
  S4->addTransition(&transitionS4S6, S6); // If you don't see the cross at this point, STOP!!!

  // S5->Si
  S5->addTransition(&transitionS5S6, S6);

  //Communications Transitions
  Ssb->addTransition(&transitionSsbSp, Sp);
  Ssb->addTransition(&transitionSsbSr, Sr);
  Ssb->addTransition(&transitionSsb, Ssb);
  Sp->addTransition(&transitionSpSnt, Snt);
  Sp->addTransition(&transitionSpSsb, Ssb);
  Snt->addTransition(&transitionSntSsb, Ssb);
  Sr->addTransition(&transitionSrSsb, Ssb);
}

void update_motors(float voltage) {
  int pwm = float(voltage/4)*255;
  //if(voltage > v_max) pwm = 255;
  //if(voltage < -(v_max)) pwm = -255;
  //Serial.println(pwm);
  if (pwm >= 0) {
    digitalWrite(MRDIR, LOW);
    digitalWrite(MLDIR, HIGH);
  }
  else {
    digitalWrite(MRDIR, HIGH);
    digitalWrite(MLDIR, LOW); 
  }

  analogWrite(MLPWM, abs(pwm));
  analogWrite(MRPWM, abs(pwm));
  
}

void setup() {
  Serial.begin(57600);

  transitions_init();
  
  digitalWrite(EN, HIGH);
  pinMode(MRPWM, OUTPUT);
  pinMode(MLPWM, OUTPUT);
  analogWrite(MRPWM, 0);
  analogWrite(MLPWM, 0);

  Wire.begin(PERIPH_ADDRESS); // Initialize I2C
  Wire.onReceive(receiveEvent); // Define interupt method for receiving data
  Serial.println("Initialization complete."); // Let the user know we're ready.
}

void loop() {
  delay(8);
  calc_deltas();
  actuate.run();
  comm.run();
}
