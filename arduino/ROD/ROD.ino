#include <Servo.h>

typedef union {
    struct name {
        bool a, b, x, y,
            leftTop, rightTop, leftTrigger, rightTrigger,
            select, start, leftStick, rightStick,
            dpadUp, dpadDown, dpadLeft, dpadRight;
    };
    bool index[16];
} DigitalButtons;

typedef struct {
    float x, y;
} AnalogStick;

// Controller state encoding
const int PACKET_SIZE = 35;
const int PACKET_BUTTONS[2] = {0, 4};
const int PACKET_LSX[2] = {5, 9};
const int PACKET_LSY[2] = {10, 14};
const int PACKET_RSX[2] = {15, 19};
const int PACKET_RSY[2] = {20, 24};
const int PACKET_LT[2] = {25, 29};
const int PACKET_RT[2] = {30, 34};

// PWM out pins
int pinWheelsLeft = 3;
int pinWheelsRight = 5;
int pinLoader = 6;
int pinBoarding = 9;
int pinCameraPitch = 10;
int pinCameraYaw = 11;
int pinCrane = 13;

// Digital out pins
int pinReverseLeft = 7;
int pinReverseRight = 8;

// Variables to hold controller state
DigitalButtons buttons;
AnalogStick leftStick;
AnalogStick rightStick;
float leftTrigger;
float rightTrigger;

Servo svoLoader;
Servo svoBoarding;
Servo svoCameraPitch;
Servo svoCameraYaw;
Servo svoCrane;

void setup() {
	// Enable serial connetion using Ethernet or WiFi
	Serial1.begin(115200);

    // Enable output on all out pins
    pinMode(pinWheelsLeft, OUTPUT);
	pinMode(pinWheelsRight, OUTPUT);
	pinMode(pinLoader, OUTPUT);
    pinMode(pinBoarding, OUTPUT);
    pinMode(pinCameraPitch, OUTPUT);
    pinMode(pinCameraYaw, OUTPUT);
    pinMode(pinCrane, OUTPUT);
	pinMode(pinReverseLeft, OUTPUT);
	pinMode(pinReverseRight, OUTPUT);

	// Enable serial for debugging purposes
	Serial.begin(9600);
	Serial.print("Hello World!");

	// Set control pin for servo's
	svoLoader.attach(pinLoader);
	svoBoarding.attach(pinBoarding);
	svoCameraPitch.attach(pinCameraPitch);
	svoCameraYaw.attach(pinCameraYaw);
	svoCrane.attach(pinCrane);

	// Turn off all outputs
	analogWrite(pinWheelsLeft, 0);
	analogWrite(pinWheelsRight, 0);

	// Clear the buffer
	while (Serial1.available()) {
		Serial1.read();
	}
}

void loop() {
	if (Serial1.available() >= PACKET_SIZE) {
		// Read the latest packet
		char packet[PACKET_SIZE];
		for (int i = 0; i < PACKET_SIZE; i++) {
			packet[i] = Serial1.read();
		}

		// Discard the rest of the buffer
		while (Serial1.available()) {
			Serial1.read();
		}

		// Print received packet
		Serial.print("Packet: ");
		Serial.println(packet);

		// Validate packet
		if (packet[PACKET_SIZE - 1] != ';') {
		    Serial.println("Bad packet");
		    Serial.println();
		    return;
		}

		// Decode controller state
		buttons = decodeButtons(packet);
		leftStick = decodeLeftStick(packet);
		rightStick = decodeRightStick(packet);
		leftTrigger = decodeLeftTrigger(packet);
		rightTrigger = decodeRightTrigger(packet);

//		// Print button state
//		Serial.print("Buttons: ");
//		for (int i = 0; i < 16; i++) {
//			Serial.print(buttons.index[i]);
//		}
//		Serial.println();
//
//		// Print left stick state
//		Serial.print("Left stick: ");
//		Serial.print(leftStick.x);
//		Serial.print(", ");
//		Serial.println(leftStick.y);
//
//		// Print right stick state
//        Serial.print("Right stick: ");
//        Serial.print(rightStick.x);
//        Serial.print(", ");
//        Serial.println(rightStick.y);
//
//        // Print left trigger state
//        Serial.print("Left trigger: ");
//        Serial.println(leftTrigger);
//
//        // Print right trigger state
//        Serial.print("Right trigger: ");
//        Serial.println(rightTrigger);

		// Wheels
		if (buttons.index[7]) {
			// Move forwards
			Serial.println("Moving forwards");
			analogWrite(pinWheelsLeft, 255);
			analogWrite(pinWheelsRight, 255);
			digitalWrite(pinReverseLeft, LOW);
			digitalWrite(pinReverseRight, LOW);
		} else if (buttons.index[6]) {
			// Move backwards
			Serial.println("Moving backwards");
			analogWrite(pinWheelsLeft, 255);
			analogWrite(pinWheelsRight, 255);
			digitalWrite(pinReverseLeft, HIGH);
			digitalWrite(pinReverseRight, HIGH);
		} else if (buttons.index[4]) {
			// Turn left
			Serial.println("Turning left");
			analogWrite(pinWheelsLeft, 255);
			analogWrite(pinWheelsRight, 255);
			digitalWrite(pinReverseLeft, HIGH);
			digitalWrite(pinReverseRight, LOW);
		} else if (buttons.index[5]) {
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

//		if (buttons.name.a) {
//            moveServo(svoBoarding, 0, 180, 10);
//		} else if (buttons.name.b) {
//            moveServo(svoBoarding, 180, 0, 10);
//		} else if (buttons.name.x) {
//		    moveServo(svoCrane, 0, 70, 10);
//		} else if (buttons.name.y) {
//		    moveServo(svoCrane, 70, 0, 10);
//		} else if (buttons.name.select) {
//		    moveServo(svoLoader, 0, 180, 10);
//		} else if (buttons.name.start) {
//		    moveServo(svoLoader, 180, 0, 10);
//		} else if (buttons.name.dpadLeft) {
//		    moveServo(svoCameraYaw, 0, 180, 10);
//		} else if (buttons.name.dpadRight) {
//		    moveServo(svoCameraYaw, 180, 0, 10);
//		} else if (buttons.name.dpadUp) {
//            moveServo(svoCameraPitch, 70, 110, 10);
//		} else if (buttons.name.dpadDown) {
//            moveServo(svoCameraPitch, 110, 70, 10);
//		}

        if (buttons.index[0]) {
            Serial.println("Servo up");
            moveServo(svoBoarding, 0, 180, 10);
        } else if (buttons.index[1]) {
            Serial.println("Servo down");
            moveServo(svoBoarding, 180, 0, 10);
        }

		Serial.println();
	}
}

DigitalButtons decodeButtons(char *packet) {
	bool hash[16][4] = {
		{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 1, 0}, {0, 0, 1, 1},
		{0, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 1},
		{1, 0, 0, 0}, {1, 0, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 1},
		{1, 1, 0, 0}, {1, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 1}
	};

	// Get button state from packet
	int length = PACKET_BUTTONS[1] - PACKET_BUTTONS[0];
	char buttonStr[length];
	for (int i = 0; i < length; i++) {
		buttonStr[i] = packet[PACKET_BUTTONS[0] + i];
	}

	// Convert hex string to bool array
	DigitalButtons buttons;
	for (int hexIndex = 0; hexIndex < length; hexIndex++) {
	    // Convert hex character to decimal number
		int decimal;
		if (buttonStr[hexIndex] >= '0' && buttonStr[hexIndex] <= '9') {
			decimal = buttonStr[hexIndex] - '0';
		} else if (buttonStr[hexIndex] >= 'a' && buttonStr[hexIndex] <= 'f') {
			decimal = buttonStr[hexIndex] - 'a' + 10;
		} else {
			decimal = 0;
		}

        // Convert decimal number to 4-digit binary string using hash table
		for (int binIndex = 0; binIndex < 4; binIndex++) {
			buttons.index[4 * hexIndex + binIndex] = hash[decimal][binIndex];
		}
	}
	
	return buttons;
}

AnalogStick decodeLeftStick(char *packet) {
    AnalogStick stick;

	// Get left stick X value from packet
	int xLength = PACKET_LSX[1] - PACKET_LSX[0];
	char xStr[xLength];
	for (int i = 0; i < xLength; i++) {
		xStr[i] = packet[PACKET_LSX[0] + i];
	}
	stick.x = atof(xStr) - 1.0;

	// Get left stick Y value from packet
	int yLength = PACKET_LSY[1] - PACKET_LSY[0];
	char yStr[yLength];
	for (int i = 0; i < yLength; i++) {
		yStr[i] = packet[PACKET_LSY[0] + i];
	}
	stick.y = atof(yStr) - 1.0;

	return stick;
}

AnalogStick decodeRightStick(char *packet) {
    AnalogStick stick;

	// Get right stick X value from packet
	int xLength = PACKET_RSX[1] - PACKET_RSX[0];
	char xStr[xLength];
	for (int i = 0; i < xLength; i++) {
		xStr[i] = packet[PACKET_RSX[0] + i];
	}
	stick.x = atof(xStr) - 1.0;

	// Get right stick Y value from packet
	int yLength = PACKET_RSY[1] - PACKET_RSY[0];
	char yStr[yLength];
	for (int i = 0; i < yLength; i++) {
		yStr[i] = packet[PACKET_RSY[0] + i];
	}
	stick.y = atof(yStr) - 1.0;

	return stick;
}

float decodeLeftTrigger(char *packet) {
    // Get left stick value from packet
    int length = PACKET_LT[1] - PACKET_LT[0];
    char str[length];
    for (int i = 0; i < length; i++) {
        str[i] = packet[PACKET_LT[0] + i];
    }

    return atof(str);
}

float decodeRightTrigger(char *packet) {
    // Get left stick value from packet
    int length = PACKET_RT[1] - PACKET_RT[0];
    char str[length];
    for (int i = 0; i < length; i++) {
        str[i] = packet[PACKET_RT[0] + i];
    }

    return atof(str);
}

void moveServo(Servo svo, int from, int to, int delayTime) {
    for (int i = from; i < to; i++) {
        svo.write(i);
        delay(delayTime);
    }
}
