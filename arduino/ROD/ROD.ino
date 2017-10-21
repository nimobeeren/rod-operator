#include <Servo.h>

// Controller state encoding
int MESSAGE_SIZE = 25;
int MESSAGE_BUTTONS[2] = {0, 4};
int MESSAGE_LSX[2] = {5, 9};
int MESSAGE_LSY[2] = {10, 14};
int MESSAGE_RSX[2] = {15, 19};
int MESSAGE_RSY[2] = {20, 24};

// PWM out pins
int pinWheelsLeft = 3;
int pinWheelsRight = 5;
int pinLoader = 6;

// Digital out pins
int pinReverseLeft = 7;
int pinReverseRight = 8;

// Variables to hold controller state
bool buttons[16];
float leftStick[2];
float rightStick[2];
float leftTrigger;
float rightTrigger;

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
	    // Print amount of received bytes
	    Serial.print("Bytes: ");
	    Serial.println(Serial1.available());

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
		Serial.print("Message: ");
		Serial.println(message);

		// Decode controller state
		decodeButtons(buttons, message);
		decodeLeftStick(leftStick, message);
		decodeRightStick(rightStick, message);

		// Print button state
		Serial.print("Buttons: ");
		for (int i = 0; i < 16; i++) {
			Serial.print(buttons[i]);
		}
		Serial.println();

		// Print left stick state
		Serial.print("Left stick: ");
		Serial.print(leftStick[0]);
		Serial.print(", ");
		Serial.println(leftStick[1]);

		// Print right stick state
        Serial.print("Right stick: ");
        Serial.print(rightStick[0]);
        Serial.print(", ");
        Serial.println(rightStick[1]);

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

		Serial.println();
	}
}

void decodeButtons(bool *buf, char *msg) {
	bool hash[16][4] = {
		{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 1, 0}, {0, 0, 1, 1},
		{0, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 1},
		{1, 0, 0, 0}, {1, 0, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 1},
		{1, 1, 0, 0}, {1, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 1}
	};

	// Get the button state from the message
	int length = MESSAGE_BUTTONS[1] - MESSAGE_BUTTONS[0];
	char buttonStr[length];
	for (int i = 0; i < length; i++) {
		buttonStr[i] = msg[MESSAGE_BUTTONS[0] + i];
	}

	// Convert hex string to bool array
	for (int hexIndex = 0; hexIndex < length; hexIndex++) {
		int location;
		if (buttonStr[hexIndex] >= '0' && buttonStr[hexIndex] <= '9') {
			location = buttonStr[hexIndex] - '0';
		} else if (buttonStr[hexIndex] >= 'a' && buttonStr[hexIndex] <= 'f') {
			location = buttonStr[hexIndex] - 'a' + 10;
		} else {
			location = 0;
		}

		for (int binIndex = 0; binIndex < 4; binIndex++) {
			buf[4 * hexIndex + binIndex] = hash[location][binIndex];
		}
	}
}

void decodeLeftStick(float *buf, char *msg) {
	// Get left stick X value from the message
	int xLength = MESSAGE_LSX[1] - MESSAGE_LSX[0];
	char xStr[xLength];
	for (int i = 0; i < xLength; i++) {
		xStr[i] = msg[MESSAGE_LSX[0] + i];
	}
	buf[0] = atof(xStr) - 1.0;

	// Get left stick Y value from the message
	int yLength = MESSAGE_LSY[1] - MESSAGE_LSY[0];
	char yStr[yLength];
	for (int i = 0; i < yLength; i++) {
		yStr[i] = msg[MESSAGE_LSY[0] + i];
	}
	buf[1] = atof(yStr) - 1.0;
}

void decodeRightStick(float *buf, char *msg) {
	// Get right stick X value from the message
	int xLength = MESSAGE_RSX[1] - MESSAGE_RSX[0];
	char xStr[xLength];
	for (int i = 0; i < xLength; i++) {
		xStr[i] = msg[MESSAGE_RSX[0] + i];
	}
	buf[0] = atof(xStr) - 1.0;

	// Get right stick Y value from the message
	int yLength = MESSAGE_RSY[1] - MESSAGE_RSY[0];
	char yStr[yLength];
	for (int i = 0; i < yLength; i++) {
		yStr[i] = msg[MESSAGE_RSY[0] + i];
	}
	buf[1] = atof(yStr) - 1.0;
}
