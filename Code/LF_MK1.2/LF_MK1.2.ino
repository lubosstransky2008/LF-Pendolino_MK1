#define DEFAULT_SPEED 65     //rychlost robota
#define MAX_RANG 520         //the max measurement vaule of the module is 520cm(a little bit longer than  effective max range)
#define ADC_SOLUTION 1023.0  //ADC accuracy of Arduino UNO is 10bit
#define OBSTACLE_DIST 10     // vzdálenost v cm, kdy začne objíždět
#define AVOID_SPEED 80       // rychlost při objíždění
#define TURN_TIME 420        // čas v ms potřebný pro otočení o 90 stupňů (nutno vyladit!)
#define SIDE_TIME 600        // čas v ms pro jízdu podél kostky (nutno vyladit!)

// Motor A - Levý
const int PWM1 = 11;
const int DIR1 = 10;

// Motor B - Pravý
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

int sensityPin = A0;

int IR1, IR2, IR3, IR4, IR5, IR6, IR7;

const int threshold = 800;

const int buttonPin = 9;
int buttonState;
int lastButtonState = LOW;
bool isOn = false;

bool objectInWay = false;  //předmět před 10 cm robotem
float sensity_t;
float dist_t;

int LFspeed, Lspeed, Rspeed;
int lastError = 0;
float Kp = 2;  // Proporcionální konstanta (ladit podle potřeby)
float Kd = 8;  // Derivační konstanta (tlumí cukání)

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
  IR7 = analogRead(U7) < threshold ? 0 : 1;
  IR6 = analogRead(U6) < threshold ? 0 : 1;
  IR5 = analogRead(U5) < threshold ? 0 : 1;
  IR4 = analogRead(U4) < threshold ? 0 : 1;
  IR3 = analogRead(U3) < threshold ? 0 : 1;
  IR2 = analogRead(U2) < threshold ? 0 : 1;
  IR1 = analogRead(U1) < threshold ? 0 : 1;
  
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
  Serial.write(10);

  sensity_t = analogRead(sensityPin);
  dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;

  if (isOn && dist_t > 0 && dist_t < OBSTACLE_DIST) {
    avoidObstacle();
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

void linefollow() {
  int error = (-8 * IR1 - 4 * IR2 - 1 * IR3 + 1 * IR5 + 4 * IR6 + 8 * IR7);
  int derivative = error - lastError;  // Výpočet změny chyby
  int adjustment = (Kp * error) + (Kd * derivative);

  if (IR1 == 0 && IR2 == 0 && IR3 == 0 && IR4 == 0 && IR5 == 0 && IR6 == 0 && IR7 == 0) {
    if (lastError < 0) sharpLeft();        // Čára zmizela vlevo
    else if (lastError > 0) sharpRight();  // Čára zmizela vpravo
    return;
  }

  if (IR1 == 1 && IR7 == 0) {
    sharpLeft();
  } else if (IR1 == 0 && IR7 == 1) {
    sharpRight();
  } else if (IR1 == 1 && IR7 == 1) {
    sharpLeft();
  } else {
    int Lspeed = DEFAULT_SPEED + adjustment;
    int Rspeed = DEFAULT_SPEED - adjustment;

    move(Lspeed, Rspeed);
    lastError = error;  // Uložení chyby pro příští smyčku
  }
}

void move(int Lspeed, int Rspeed) {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, constrain(Lspeed, 0, 255));
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, constrain(Rspeed, 0, 255));
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
/*
void crossOver() { //Na křižovatce robot zatáčí doleva (pravidla Robotix Přerov)
  digitalWrite(DIR1, LOW);
  analogWrite(PWM1, DEFAULT_SPEED);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, DEFAULT_SPEED);
}
*/
void stop() {
  digitalWrite(DIR1, HIGH);
  analogWrite(PWM1, 0);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, 0);
}

// Pomocná funkce pro snazší pohyb v obou směrech
void moveCustom(int L, int R) {
  digitalWrite(DIR1, L >= 0 ? HIGH : LOW);
  analogWrite(PWM1, abs(L));
  digitalWrite(DIR2, R >= 0 ? HIGH : LOW);
  analogWrite(PWM2, abs(R));
}

void avoidObstacle() {
  stop();
  delay(500);

  moveCustom(-AVOID_SPEED, AVOID_SPEED); //doleva
  delay(TURN_TIME);

  moveCustom(AVOID_SPEED, AVOID_SPEED); //rovně
  delay(SIDE_TIME);

  moveCustom(AVOID_SPEED, -AVOID_SPEED); //doprava
  delay(TURN_TIME);

  moveCustom(AVOID_SPEED, AVOID_SPEED); //rovně
  delay(SIDE_TIME * 2 );  // delší čas

  moveCustom(AVOID_SPEED, -AVOID_SPEED); //doprava
  delay(TURN_TIME);

  while (analogRead(U4) < threshold) {
    moveCustom(DEFAULT_SPEED, DEFAULT_SPEED);
  }

  stop();
  delay(200);
}