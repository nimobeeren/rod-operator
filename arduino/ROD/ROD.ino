#include <Servo.h>

int MESSAGE_SIZE = 4;

// PWM out pins
int pinWheelsLeft = 3;
int pinWheelsRight = 5;
int pinLoader = 6;

// Digital out pins
int pinReverseLeft = 7;
int pinReverseRight = 8;

Servo svoLoader;

void setup() {
	// Enable serial connetion using Ethernet or WiFi
	Serial1.begin(115200);

    // Enable output on all out pins
    pinMode(pinWheelsLeft, OUTPUT);
	pinMode(pinWheelsRight, OUTPUT);
	pinMode(pinLoader, OUTPUT);
	pinMode(pinReverseLeft, OUTPUT);
	pinMode(pinReverseRight, OUTPUT);

	// Enable serial for debugging purposes
	Serial.begin(9600);
	Serial.print("Hello World!");

	// Set control pin for servo's
	svoLoader.attach(pinLoader);

	// Turn off all outputs
	analogWrite(pinWheelsLeft, 0);
	analogWrite(pinWheelsRight, 0);

	// Clear the buffer
	while (Serial1.available()) {
		Serial1.read();
	}
}

void loop() {
	if (Serial1.available() >= MESSAGE_SIZE) {
		// Read the latest message
		char message[MESSAGE_SIZE];
		for (int i = 0; i < MESSAGE_SIZE; i++) {
			message[i] = Serial1.read();
		}

		// Discard the rest of the buffer
		while (Serial1.available()) {
			Serial1.read();
		}

		// Print received message
//		Serial.print(millis());
//		Serial.print(" - ");
//		Serial.println(message);

		bool buttons[16];
		decodeButtons(message, buttons);
		for (int i = 0; i < 16; i++) {
			Serial.print(buttons[i]);
		}
		Serial.println();

		// Wheels
		if (buttons[7]) {
			// Move forwards
			Serial.println("Moving forwards");
			analogWrite(pinWheelsLeft, 255);
			analogWrite(pinWheelsRight, 255);
			digitalWrite(pinReverseLeft, LOW);
			digitalWrite(pinReverseRight, LOW);
		} else if (buttons[6]) {
			// Move backwards
			Serial.println("Moving backwards");
			analogWrite(pinWheelsLeft, 255);
			analogWrite(pinWheelsRight, 255);
			digitalWrite(pinReverseLeft, HIGH);
			digitalWrite(pinReverseRight, HIGH);
		} else if (buttons[4]) {
			// Turn left
			Serial.println("Turning left");
			analogWrite(pinWheelsLeft, 255);
			analogWrite(pinWheelsRight, 255);
			digitalWrite(pinReverseLeft, HIGH);
			digitalWrite(pinReverseRight, LOW);
		} else if (buttons[5]) {
			// Turn right
			Serial.println("Turning right");
			analogWrite(pinWheelsLeft, 255);
			analogWrite(pinWheelsRight, 255);
			digitalWrite(pinReverseLeft, LOW);
			digitalWrite(pinReverseRight, HIGH);
		} else {
		    // Stop moving
		    Serial.println("Stopping");
			analogWrite(pinWheelsLeft, 0);
			analogWrite(pinWheelsRight, 0);
			digitalWrite(pinReverseLeft, LOW);
			digitalWrite(pinReverseRight, LOW);
		}
	}
}

bool decodeButtons(char msg[], bool *arr) {
	bool hash[16][4] = { {false, false, false, false}, {false, false, false, true}, {false, false, true, false}, {false, false, true, true}, {false, true, false, false}, {false, true, false, true}, {false, true, true, false}, {false, true, true, true}, {true, false, false, false}, {true, false, false, true}, {true, false, true, false}, {true, false, true, true}, {true, true, false, false}, {true, true, false, true}, {true, true, true, false}, {true, true, true, true} };
	for (int hexIndex = 0; hexIndex < 4; hexIndex++) {
		int location;
		if (msg[hexIndex] >= '0' && msg[hexIndex] <= '9') {
			location = msg[hexIndex] - '0';
		} else if (msg[hexIndex] >= 'a' && msg[hexIndex] <= 'f') {
			location = msg[hexIndex] - 'a' + 10;
		} else {
			location = 0;
		}

		for (int binIndex = 0; binIndex < 4; binIndex++) {
			arr[4 * hexIndex + binIndex] = hash[location][binIndex];
		}
	}
	return arr;
}
