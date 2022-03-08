#define M1PWM 9
#define M2PWM 10
#define M1DIR 7
#define M2DIR 8

#define M1FB A0
#define M2FB A1

#define ENC1 3
#define ENC2 5

#define EN 4

void setup() {
  // put your setup code here, to run once:
  digitalWrite(EN, LOW);
  pinMode(M1FB, INPUT);
  pinMode(M2FB, INPUT);
  pinMode(ENC1, INPUT);
  pinMode(ENC2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}
