#define DEFAULT_SPEED 115    //rychlost robota
#define MAX_RANG 520         //maximální dosah
#define ADC_SOLUTION 1023.0  //ADC apřesnost pro Arduino Nano
#define OBSTACLE_DIST 8     // vzdálenost v cm, kdy zastaví
// Motor A - Levý
const int PWM1 = 11;
const int DIR1 = 10;

// Motor B - Pravý
const int PWM2 = 6;
const int DIR2 = 7;

// IR Sensory - Prohozené jsou protože jsem debil (chyba na DPS)
#define U7 A1
#define U6 A2
#define U5 A3
#define U4 A4
#define U3 A5
#define U2 A6
#define U1 A7

int sensityPin = A0;

int IR1, IR2, IR3, IR4, IR5, IR6, IR7;

const int threshold = 840;  //830 - 880

const int buttonPin = 9;
int buttonState;
int lastButtonState = LOW;
bool isOn = false;

float sensity_t;
float dist_t;

int LFspeed, Lspeed, Rspeed;
int lastError = 0;
float Kp = 3;   // Proporcionální konstanta (ladit podle potřeby)
float Kd = 11;  // Derivační konstanta (tlumí cukání)

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
  pinMode(sensityPin, INPUT);

  Serial.begin(57600);
}

void loop() {
  //načtení hodnot a očištění o limit, aby nás nerušil šum
  readAllSensors();

  sensity_t = analogRead(sensityPin);
  dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;

  if (isOn && dist_t > 0 && dist_t < OBSTACLE_DIST) {
    isOn = !isOn; //robot se vypne x cm před zdí aby nenaboural, jen funkce navíc
  }

  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      isOn = !isOn;
    }
    delay(50);
  }
  lastButtonState = buttonState;

  if (isOn) {
    linefollow();
  } else {
    stop();
  }
}

void readAllSensors() {
  IR7 = analogRead(U7) > threshold ? 1 : 0;
  IR6 = analogRead(U6) > threshold ? 1 : 0;
  IR5 = analogRead(U5) > threshold ? 1 : 0;
  IR4 = analogRead(U4) > threshold ? 1 : 0;
  IR3 = analogRead(U3) > threshold ? 1 : 0;
  IR2 = analogRead(U2) > threshold ? 1 : 0;
  IR1 = analogRead(U1) > threshold ? 1 : 0;
}

void linefollow() {
  int error = (-50 * IR1 - 20 * IR2 - 5 * IR3 + 5 * IR5 + 20 * IR6 + 50 * IR7);
  int derivative = error - lastError;  // Výpočet změny chyby
  int adjustment = (Kp * error) + (Kd * derivative);

  if (IR1 == 0 && IR2 == 0 && IR3 == 0 && IR4 == 0 && IR5 == 0 && IR6 == 0 && IR7 == 0) {
    if (lastError < 0) sharpLeft();        // Čára zmizela vlevo
    else if (lastError > 0) sharpRight();  // Čára zmizela vpravo
    return;
  }

  int Lspeed = DEFAULT_SPEED + adjustment;
  int Rspeed = DEFAULT_SPEED - adjustment;

  moveCustom(Lspeed, Rspeed);
  lastError = error;  // Uložení chyby pro příští smyčku
}

void sharpLeft() {
  digitalWrite(DIR1, LOW);
  analogWrite(PWM1, DEFAULT_SPEED);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, DEFAULT_SPEED);
}

void sharpRight() {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, DEFAULT_SPEED);
  digitalWrite(DIR2, LOW);
  analogWrite(PWM2, DEFAULT_SPEED);
}

void stop() {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, 0);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, 0);
}

void moveCustom(int L, int R) {
  digitalWrite(DIR1, L >= 0 ? HIGH : LOW);
  analogWrite(PWM1, constrain(abs(L), 0, 255));
  digitalWrite(DIR2, R >= 0 ? HIGH : LOW);
  analogWrite(PWM2, constrain(abs(R), 0, 255));
}