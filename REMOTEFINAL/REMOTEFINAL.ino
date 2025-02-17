#include <IRremote.h>

const int IR_RECEIVE_PIN = 9;  // Define the pin connected to the IR receiver

// Pin Definitions
const int pinA = 5;  // Motor A speed (PWM)
const int pinB = 6;  // Motor B speed (PWM)
const int pinC = 7;  // Motor A direction
const int pinD = 8;  // Motor B direction
const int pinE = 3;  // Standby pin to stop the whole car

int carState = 0;          // Tracks the current state: 0 = stopped, 1 = forward, -1 = backward
int carSpeed = 80;         // Initial speed of the car (range: 0 to 255)
const int speedStep = 10;  // Increment for speed adjustment

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Initialize the receiver
  Serial.println("IR Receiver Initialized. Waiting for input...");
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinE, OUTPUT);
}

void loop() {
  // Check if a signal is received
  if (IrReceiver.decode()) {
    unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;

    Serial.print("Received HEX Value: 0x");
    Serial.println(receivedCode, HEX);  // Print the HEX value of the received signal

    // Handle buttons
    if (receivedCode == 0xB946FF00) {  // Forward button
      Serial.println("Forward button pressed.");
      forward();
      carState = 1;
    } else if (receivedCode == 0xEA15FF00) {  // Backward button
      Serial.println("Backward button pressed.");
      backward();
      carState = -1;
    } else if (receivedCode == 0xBF40FF00) {  // Stop button
      Serial.println("Stop button pressed.");
      stopMotors();
      carState = 0;
    } else if (receivedCode == 0xBC43FF00) {  // Turn right button
      Serial.println("Turn right button pressed.");
      if (carState == 1) {
        turnRight();
      } else if (carState == -1) {
        turnRightBackward();
      }
    } else if (receivedCode == 0xBB44FF00) {  // Turn left button
      Serial.println("Turn left button pressed.");
      if (carState == 1) {
        turnLeft();
      } else if (carState == -1) {
        turnLeftBackward();
      }
    } else if (receivedCode == 0xF609FF00) {  // Speed up button
      increaseSpeed();
    } else if (receivedCode == 0xF807FF00) {  // Speed down button
      decreaseSpeed();
    }

    IrReceiver.resume();  // Prepare to receive the next signal
  }
}

// Adjust speed up
void increaseSpeed() {
  carSpeed += speedStep;
  if (carSpeed > 255) carSpeed = 255;  // Limit maximum speed
  Serial.print("Speed increased to: ");
  Serial.println(carSpeed);
  applyCurrentState();
}

// Adjust speed down
void decreaseSpeed() {
  carSpeed -= speedStep;
  if (carSpeed < 0) carSpeed = 0;  // Limit minimum speed
  Serial.print("Speed decreased to: ");
  Serial.println(carSpeed);
  applyCurrentState();
}

// Reapply current state after speed change
void applyCurrentState() {
  if (carState == 1) {
    forward();
  } else if (carState == -1) {
    backward();
  }
}

// Move forward
void forward() {
  digitalWrite(pinC, HIGH);     // Motor A direction (forward)
  analogWrite(pinA, carSpeed);  // Motor A speed (set speed)
  digitalWrite(pinD, HIGH);     // Motor B direction (forward)
  analogWrite(pinB, carSpeed);  // Motor B speed (set speed)
  digitalWrite(pinE, HIGH);     // Enable car
}

// Move backward
void backward() {
  digitalWrite(pinC, LOW);      // Motor A direction (backward)
  analogWrite(pinA, carSpeed);  // Motor A speed (set speed)
  digitalWrite(pinD, LOW);      // Motor B direction (backward)
  analogWrite(pinB, carSpeed);  // Motor B speed (set speed)
  digitalWrite(pinE, HIGH);     // Enable car
}

// Stop motors
void stopMotors() {
  digitalWrite(pinC, LOW);  // Motor A direction (stop)
  analogWrite(pinA, 0);     // Motor A speed (stop)
  digitalWrite(pinD, LOW);  // Motor B direction (stop)
  analogWrite(pinB, 0);     // Motor B speed (stop)
  digitalWrite(pinE, LOW);  // Disable car
}

// Turn the car right while moving forward
void turnRight() {
  digitalWrite(pinC, HIGH);         // Motor A direction (backward)
  analogWrite(pinA, carSpeed / 4);  // Motor A speed (significantly reduced speed)
  digitalWrite(pinD, HIGH);         // Motor B direction (forward)
  analogWrite(pinB, carSpeed);      // Motor B speed (full speed)
}

// Turn the car left while moving forward
void turnLeft() {
  digitalWrite(pinC, HIGH);         // Motor A direction (forward)
  analogWrite(pinA, carSpeed);      // Motor A speed (full speed)
  digitalWrite(pinD, HIGH);         // Motor B direction (backward)
  analogWrite(pinB, carSpeed / 4);  // Motor B speed (significantly reduced speed)
}

// Turn the car right while moving backward
void turnRightBackward() {
  digitalWrite(pinC, LOW);          // Motor A direction (backward)
  analogWrite(pinA, carSpeed / 4);  // Motor A speed (significantly reduced speed)
  digitalWrite(pinD, LOW);          // Motor B direction (backward)
  analogWrite(pinB, carSpeed);      // Motor B speed (full speed)
}

// Turn the car left while moving backward
void turnLeftBackward() {
  digitalWrite(pinC, LOW);          // Motor A direction (backward)
  analogWrite(pinA, carSpeed);      // Motor A speed (full speed)
  digitalWrite(pinD, LOW);          // Motor B direction (backward)
  analogWrite(pinB, carSpeed / 4);  // Motor B speed (significantly reduced speed)
}

// Perform a U-turn
void UTurn() {
  digitalWrite(pinC, HIGH);     // Motor A direction (backward)
  analogWrite(pinA, carSpeed);  // Motor A speed (full speed)
  digitalWrite(pinD, LOW);      // Motor B direction (backward)
  analogWrite(pinB, carSpeed);  // Motor B speed (reduced speed)
}