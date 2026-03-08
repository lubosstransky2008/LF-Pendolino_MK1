#define DEFAULT_SPEED 50

// Motor A - Left
const int PWM1 = 11;
const int DIR1 = 10;

// Motor B - Right
const int PWM2 = 6;
const int DIR2 = 7;

// IR Sensory - Prohozené jsou protože jsem debil
#define U7 A1
#define U6 A2
#define U5 A3
#define U4 A4
#define U3 A5
#define U2 A6
#define U1 A7

int IR1, IR2, IR3, IR4, IR5, IR6, IR7;

const int threshold = 900;

const int buttonPin = 9;
int buttonState;
int lastButtonState = LOW;
bool isOn = false;

int LFspeed, Lspeed, Rspeed;

char lastDirection = 'S';

void setup() {
  pinMode(PWM1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR2, OUTPUT);

  pinMode(U7, INPUT);
  pinMode(U6, INPUT);
  pinMode(U5, INPUT);
  pinMode(U4, INPUT);
  pinMode(U3, INPUT);
  pinMode(U2, INPUT);
  pinMode(U1, INPUT);

  pinMode(buttonPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  //načtení hodnot a očištění o limit, aby nás nerušil šum
  int IR7 = analogRead(U7) < threshold ? 0 : 1;
  int IR6 = analogRead(U6) < threshold ? 0 : 1;
  int IR5 = analogRead(U5) < threshold ? 0 : 1;
  int IR4 = analogRead(U4) < threshold ? 0 : 1;
  int IR3 = analogRead(U3) < threshold ? 0 : 1;
  int IR2 = analogRead(U2) < threshold ? 0 : 1;
  int IR1 = analogRead(U1) < threshold ? 0 : 1;

  //1 = černá, 0 = bílá
  Serial.print(isOn);
  Serial.print("; ");
  Serial.print(IR1);
  Serial.print(",");
  Serial.print(IR2);
  Serial.print(",");
  Serial.print(IR3);
  Serial.print(",");
  Serial.print(IR4);
  Serial.print(",");
  Serial.print(IR5);
  Serial.print(",");
  Serial.print(IR6);
  Serial.print(",");
  Serial.print(IR7);
  Serial.print("; ");
  Serial.print(lastDirection);
  Serial.write(10);

  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      isOn = !isOn;
      if (isOn) {
        if (IR1 == 1 && IR7 == 0) {
          sharpLeft();
        } else if (IR1 == 0 && IR7 == 1) {
          sharpRight();
        } else if (IR1 == 1 && IR7 == 1) {
          sharpLeft();
        } else {
          linefollow();
        }
      } else {
        stop();
      }
    }
    delay(50);
  }
  lastButtonState = buttonState;
}

void linefollow() {
  int error = (-3 * IR1 - 2 * IR2 - 1 * IR3 + 1 * IR5 + 2 * IR6 + 3 * IR7);
  if (error == 0 || IR4 == 1) {
    digitalWrite(DIR1, HIGH);
    analogWrite(PWM1, DEFAULT_SPEED);
    digitalWrite(DIR2, HIGH);
    analogWrite(PWM2, DEFAULT_SPEED);
    lastDirection = 'F';
  } else if (error > 0) {
    LFspeed = 0.3 * error * DEFAULT_SPEED;
    digitalWrite(DIR1, LOW);
    analogWrite(PWM1, LFspeed);
    digitalWrite(DIR2, HIGH);
    analogWrite(PWM2, LFspeed);
    lastDirection = 'l';
  } else if (error < 0) {
    LFspeed = -0.3 * error * DEFAULT_SPEED;
    digitalWrite(DIR1, HIGH);
    analogWrite(PWM1, LFspeed);
    digitalWrite(DIR2, LOW);
    analogWrite(PWM2, LFspeed);
    lastDirection = 'r';
  }
}

void sharpLeft() {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, 0);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, DEFAULT_SPEED);
  lastDirection = 'L';
}

void sharpRight() {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, DEFAULT_SPEED);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, 0);
  lastDirection = 'R';
}

void stop() {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, 0);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, 0);
  lastDirection = 'S';
}