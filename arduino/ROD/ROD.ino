#include <Servo.h>

// Controller state encoding
int MESSAGE_SIZE = 9;
int MESSAGE_BUTTONS[2] = {0, 4};
int MESSAGE_LS[2] = {5, 9};

// PWM out pins
int pinWheelsLeft = 3;
int pinWheelsRight = 5;
int pinLoader = 6;

// Digital out pins
int pinReverseLeft = 7;
int pinReverseRight = 8;

// Controller state
bool buttons[16];
float leftStick;
float rightStick;
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
		decodeButtons(message, buttons);
		leftStick = decodeLeftStick(message);

		// Print button state
		Serial.print("Buttons: ");
		for (int i = 0; i < 16; i++) {
			Serial.print(buttons[i]);
		}
		Serial.println();

		// Print left stick state
		Serial.print("Left stick: ");
		Serial.println(leftStick);

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

bool decodeButtons(char msg[], bool *arr) {
	bool hash[16][4] = {
	    {0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 1, 0}, {0, 0, 1, 1},
	    {0, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 1},
	    {1, 0, 0, 0}, {1, 0, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 1},
	    {1, 1, 0, 0}, {1, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 1}
	};

	// Get the button state from the message
	int length = MESSAGE_BUTTONS[1] - MESSAGE_BUTTONS[0];
	char valueString[length];
	for (int i = 0; i < length; i++) {
	    valueString[i] = msg[MESSAGE_BUTTONS[0] + i];
	}

    // Convert hex string to bool array
	for (int hexIndex = 0; hexIndex < length; hexIndex++) {
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

float decodeLeftStick(char msg[]) {
    // Get left stick state from the message
    int length = MESSAGE_LS[1] - MESSAGE_LS[0];
    char valueString[length];
    for (int i = 0; i < length; i++) {
        valueString[i] = msg[MESSAGE_LS[0] + i];
    }

    return atof(valueString) - 1.0;
}
