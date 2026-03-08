const int buttonPin = 9;
int buttonState;
int lastButtonState = LOW;
bool isOn = false;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      isOn = !isOn;
      if (isOn) {
        //čudlík zapnutý
      } else {
        //čudlík vypnutý
      }
      Serial.println(isOn);
      Serial.write(10);
    }
    delay(50);
  }
  lastButtonState = buttonState;
}