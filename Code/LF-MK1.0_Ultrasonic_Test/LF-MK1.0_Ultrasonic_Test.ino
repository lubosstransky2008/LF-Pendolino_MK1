#define MAX_RANG 520         //maximální dosah
#define ADC_SOLUTION 1023.0  //ADC přesnost pro Arduino Nano

bool isOn = false;
int sensityPin = A0;
void setup() {
  Serial.begin(19200);
}
float dist_t, sensity_t;
void loop() {
  sensity_t = analogRead(sensityPin);

  dist_t = sensity_t * MAX_RANG / ADC_SOLUTION;
  Serial.print(dist_t, 0);
  Serial.println("cm");

  if (dist_t < 30){
    isOn = !isOn;
  }
  
  if (isOn) {
    //zapnuté
  } else {
    //vypnuté
  }

  delay(500);
}

//https://wiki.dfrobot.com/sen0307/docs/18358
