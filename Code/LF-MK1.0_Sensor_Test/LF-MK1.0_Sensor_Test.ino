// IR Sensory - Prohozené jsou protože jsem debil
#define U7 A1
#define U6 A2
#define U5 A3
#define U4 A4
#define U3 A5
#define U2 A6
#define U1 A7

const int threshold = 600;

void setup() {
  pinMode(U7, INPUT);
  pinMode(U6, INPUT);
  pinMode(U5, INPUT);
  pinMode(U4, INPUT);
  pinMode(U3, INPUT);
  pinMode(U2, INPUT);
  pinMode(U1, INPUT);

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
}