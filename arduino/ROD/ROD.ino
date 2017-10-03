int fwdPin = 3;
int bckPin = 5;

void setup() {
  // Enable pin 13 as an output
  pinMode(13, OUTPUT);
  
  // Turn off both outputs
  analogWrite(fwdPin, 0);
  analogWrite(bckPin, 0);

  // Enable serial connetion using Ethernet or WiFi
  Serial1.begin(115200);

  // Clear the buffer
  while (Serial1.available()) {
    Serial1.read();
  }
}

void loop() {
  if (Serial1.available() > 0) {
    char input = Serial1.read();
    switch (input) {
      case '0':
        analogWrite(fwdPin, 0);
        analogWrite(bckPin, 0);
        break;
      case '1':
        analogWrite(fwdPin, 255);
        analogWrite(bckPin, 0);
        break;
      case '2':
        analogWrite(fwdPin, 0);
        analogWrite(bckPin, 255);
        break;
    }
  }
}
