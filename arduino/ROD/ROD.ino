#include <Servo.h>

int pinForwards = 3;
int pinBackwards = 5;
int pinLoader = 6;

Servo svoLoader;

void setup() {
	// Enable serial connetion using Ethernet or WiFi
	Serial1.begin(115200);

    // Enable serial for debugging purposes
	Serial.begin(9600);
	Serial.print("Hello World!");

	// Set control pin for servo's
	svoLoader.attach(pinLoader);

	// Turn off both outputs
	analogWrite(pinForwards, 0);
	analogWrite(pinBackwards, 0);

	// Clear the buffer
	while (Serial1.available()) {
		Serial1.read();
	}
}

void loop() {
	if (Serial1.available() > 0) {
		String inputstr = Serial1.readString();
		const char* input = inputstr.c_str();

		if (strcmp(input, "rt") == 0) {
		    // Turn wheels forwards
			analogWrite(pinForwards, 255);
			analogWrite(pinBackwards, 0);
		} else if (strcmp(input, "lt") == 0) {
		    // Turn wheels backwards
			analogWrite(pinForwards, 0);
			analogWrite(pinBackwards, 255);
		} else if (strcmp(input, "b") == 0) {
		    // Stop wheels
			analogWrite(pinForwards, 0);
			analogWrite(pinBackwards, 0);
		} else if (strcmp(input, "rb") == 0) {
		    // Move loader up
		    svoLoader.write(0);
		} else if (strcmp(input, "lb") == 0) {
		    // Move loader down
            svoLoader.write(180);
        }
	}
}
