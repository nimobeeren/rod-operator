#include <Servo.h>

typedef union {
	struct name {
		bool a, b, x, y,
			leftTop, rightTop, leftTrigger, rightTrigger,
			select, start, leftStick, rightStick,
			dpadUp, dpadDown, dpadLeft, dpadRight;
	} name;
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

// Servo position ranges
const int RANGE_LOADER[2] = {0, 120};
const int RANGE_BOARDING[2] = {0, 180};
const int RANGE_CRANE[2] = {0, 70};
const int RANGE_CAMERA_PITCH[2] = {0, 180};
const int RANGE_CAMERA_YAW[2] = {0, 180};

// Servo speeds (°/s)
const int SPEED_LOADER = 90;
const int SPEED_BOARDING = 60;
const int SPEED_CRANE = 40;
const int SPEED_CAMERA_PITCH = 20;
const int SPEED_CAMERA_YAW = 20;

// Controller input deadzones
const float DEADZONE_STICK = 0.2;
const float DEADZONE_TRIGGER = 0;

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
int pinForwardRight = 8;

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

	// Enable serial for debugging purposes
	Serial.begin(9600);

	// Enable output on all out pins
	pinMode(pinWheelsLeft, OUTPUT);
	pinMode(pinWheelsRight, OUTPUT);
	pinMode(pinLoader, OUTPUT);
	pinMode(pinReverseLeft, OUTPUT);
	pinMode(pinForwardRight, OUTPUT);

	// Set control pin for servo's
	svoLoader.attach(pinLoader);
	svoBoarding.attach(pinBoarding);
	svoCameraPitch.attach(pinCameraPitch);
	svoCameraYaw.attach(pinCameraYaw);
	svoCrane.attach(pinCrane);

	// Turn off all outputs
	analogWrite(pinWheelsLeft, 0);
	analogWrite(pinWheelsRight, 0);
	setReverseLeft(false);
	setReverseRight(false);

	// Set all servo's to their initial position
	svoLoader.write(RANGE_LOADER[1]);
	svoBoarding.write(RANGE_BOARDING[1]);
	svoCrane.write(RANGE_CRANE[0]);
	svoCameraPitch.write(90);
	svoCameraYaw.write(90);

	// Clear the buffer
	while (Serial1.available()) {
		Serial1.read();
	}
}

void loop() {
	char packet[PACKET_SIZE];
	int readBytes = 0;
	while (readBytes < PACKET_SIZE) {
		if (Serial1.available()) {
		    // Read incoming byte
			char newByte = Serial1.read();
			bool bad = false;

			// Validate byte
			if (readBytes == PACKET_BUTTONS[1] || readBytes == PACKET_LSX[1] || readBytes == PACKET_LSY[1] ||
			readBytes == PACKET_RSX[1] || readBytes == PACKET_RSY[1] || readBytes == PACKET_LT[1]) {
			    if (newByte != ',') {
			        Serial.println("Missing separator");
			        bad = true;
			    }
			} else if (readBytes < PACKET_SIZE - 1 && newByte == ';') {
			    // We found a termination symbol before the end of the packet
			    Serial.println("Premature termination");
                bad = true;
			}

			packet[readBytes] = newByte;
			readBytes++;

			if (bad) {
                Serial.print("Bad packet: ");
                for (int i = 0; i < readBytes; i++) {
                    Serial.print(packet[i]);
                }
                Serial.print("\n\n");
                return;
            }
		}
	}

	// Print received packet
	Serial.print("Packet: ");
	Serial.println(packet);

	// Decode controller state
	buttons = decodeButtons(packet);
	leftStick = decodeLeftStick(packet);
	rightStick = decodeRightStick(packet);
	leftTrigger = decodeLeftTrigger(packet);
	rightTrigger = decodeRightTrigger(packet);

	// Print controller state for debugging
//	printControllerState();

	// Wheels
	if (leftTrigger > DEADZONE_TRIGGER || rightTrigger > DEADZONE_TRIGGER || abs(leftStick.x) > DEADZONE_STICK) {
		float leftSpeed, rightSpeed;
		float x = rightTrigger - leftTrigger;
		float y = leftStick.x;

		// Don't read small deficiencies in stick placement
		if (abs(y) <= DEADZONE_STICK) {
			y = 0;
		}

		// Calculate speed of wheels with respect to throttle and steering
		leftSpeed = x + (-6.0/5.0 * x + abs(x) - 1) * -y;
		rightSpeed = x + (-6.0/5.0 * x + abs(x) - 1) * y;
		leftSpeed = constrain(leftSpeed, -1, 1);
		rightSpeed = constrain(rightSpeed, -1, 1);

		// Print wheel speed
		Serial.print("Left wheels: ");
		Serial.print(leftSpeed);
		Serial.print("\tRight wheels: ");
		Serial.println(rightSpeed);

		// Apply wheel speed
		analogWrite(pinWheelsLeft, map(abs(leftSpeed), 0, 1, 0, 255));
		analogWrite(pinWheelsRight, map(abs(rightSpeed), 0, 1, 0, 255));

		// Apply wheel direction
		if (leftSpeed < 0) {
			setReverseLeft(true);
		} else {
			setReverseLeft(false);
		}
		if (rightSpeed < 0) {
			setReverseRight(true);
		} else {
			setReverseRight(false);
		}
	} else {
	    // Triggers and wheels are in standard position, don't move
	    analogWrite(pinWheelsLeft, 0);
	    analogWrite(pinWheelsRight, 0);
	    setReverseLeft(false);
	    setReverseRight(false);
	}

	// Loader controls
	if (buttons.name.a) {
		Serial.println("Loader up");
		moveServo(svoLoader, RANGE_LOADER[1], SPEED_LOADER);
	} else if (buttons.name.b) {
		Serial.println("Loader down");
		moveServo(svoLoader, RANGE_LOADER[0], SPEED_LOADER);
	}

	// Boarding controls
	if (buttons.name.leftTop) {
		Serial.println("Boarding in");
		moveServo(svoBoarding, RANGE_BOARDING[1], SPEED_BOARDING);
	} else if (buttons.name.rightTop) {
		Serial.println("Boarding out");
		moveServo(svoBoarding, RANGE_BOARDING[0], SPEED_BOARDING);
	}

	// Crane controls
	if (buttons.name.y) {
		Serial.println("Crane up");
		moveServo(svoCrane, RANGE_CRANE[1], SPEED_CRANE);
	} else if (buttons.name.x) {
		Serial.println("Crane down");
		moveServo(svoCrane, RANGE_CRANE[0], SPEED_CRANE);
	}

	// Camera controls
	if (buttons.name.dpadUp) {
		Serial.println("Camera pitch up");
		int newPos = constrain(svoCameraPitch.read() + 10, RANGE_CAMERA_PITCH[0], RANGE_CAMERA_PITCH[1]);
		moveServo(svoCameraPitch, newPos, SPEED_CAMERA_PITCH);
	} else if (buttons.name.dpadDown) {
		Serial.println("Camera pitch down");
		int newPos = constrain(svoCameraPitch.read() - 10, RANGE_CAMERA_PITCH[0], RANGE_CAMERA_PITCH[1]);
		moveServo(svoCameraPitch, newPos, SPEED_CAMERA_PITCH);
	} else if (buttons.name.dpadLeft) {
		Serial.println("Camera yaw up");
		int newPos = constrain(svoCameraYaw.read() + 10, RANGE_CAMERA_YAW[0], RANGE_CAMERA_YAW[1]);
		moveServo(svoCameraYaw, newPos, SPEED_CAMERA_YAW);
	} else if (buttons.name.dpadRight) {
		Serial.println("Camera yaw down");
		int newPos = constrain(svoCameraYaw.read() - 10, RANGE_CAMERA_YAW[0], RANGE_CAMERA_YAW[1]);
		moveServo(svoCameraYaw, newPos, SPEED_CAMERA_YAW);
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

void printControllerState() {
	// Print button state
	Serial.print("Buttons: ");
	for (int i = 0; i < 16; i++) {
		Serial.print(buttons.index[i]);
	}
	Serial.println();

	// Print left stick state
	Serial.print("Left stick: ");
	Serial.print(leftStick.x);
	Serial.print(", ");
	Serial.println(leftStick.y);

	// Print right stick state
	Serial.print("Right stick: ");
	Serial.print(rightStick.x);
	Serial.print(", ");
	Serial.println(rightStick.y);

	// Print left trigger state
	Serial.print("Left trigger: ");
	Serial.println(leftTrigger);

	// Print right trigger state
	Serial.print("Right trigger: ");
	Serial.println(rightTrigger);
}

void moveServo(Servo svo, int to, int speed) {
	int from = svo.read();
	if (from < to) {
		for (int i = from; i <= to; i++) {
			Serial.print("Servo pos: ");
			Serial.println(i);
			svo.write(i);
			delay(1000 / speed);
		}
	} else {
		for (int i = from; i >= to; i--) {
			Serial.print("Servo pos: ");
			Serial.println(i);
			svo.write(i);
			delay(1000 / speed);
		}
	}
}

void setReverseLeft(bool reverse) {
	if (reverse) {
		digitalWrite(pinReverseLeft, HIGH);
	} else {
		digitalWrite(pinReverseLeft, LOW);
	}
}

void setReverseRight(bool reverse) {
	if (reverse) {
		digitalWrite(pinForwardRight, LOW);
	} else {
		digitalWrite(pinForwardRight, HIGH);
	}
}
