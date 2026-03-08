#define DEFAULT_SPEED 20

// Motor A
const int PWM1 = 11;
const int DIR1 = 10;

// Motor B
const int PWM2 = 6;
const int DIR2 = 7;

void setup() {
  pinMode(PWM1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR2, OUTPUT);
}

void loop() {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, DEFAULT_SPEED);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, DEFAULT_SPEED);
  delay(2000);
}