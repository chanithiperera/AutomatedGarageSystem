#include <MFRC522.h>       // Library for RFID
#include <SPI.h>           // Library for SPI communication
#include <Stepper.h>       // Include Stepper library

// Stepper motor drive to Arduino connection
int IN1 = 5;
int IN2 = 6;
int IN3 = 7;
int IN4 = 8;

// Number of steps in one revolution of stepper motor (28BYJ-48)
int revolution = 2038;
// The correct stepping order for the stepper motor (28BYJ-48)
Stepper stepper(revolution, IN1, IN3, IN2, IN4);

// RFID SDA & RST pins to Arduino pins
int SDA_pin = 10;
int RST_pin = 9;
MFRC522 mfrc522(SDA_pin, RST_pin);

int IR = 2;               // IR sensor signal pin
int trigPin = 3;          // HC-SR04 Trigger pin
int echoPin = 4;          // HC-SR04 Echo pin

long duration;            // Variable to store the duration of the pulse
int distance;             // Variable to store the calculated distance

bool doorOpen = false;             // Flag to track if the door is open
int state = 0;                     // 0 = First Cycle, 1 = Second Cycle
bool waitingForUltrasonic = false; // Flag to monitor vehicle entry in the first cycle

// Function Prototypes
void monitorUltrasonicSensor();
void handleRFID();
void openDoor();
void closeDoor();

void setup() {
  Serial.begin(9600);    // Start Serial communication

  // Set Stepper Motor driver pins as OUTPUT
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(IR, INPUT);        // Set IR as INPUT
  pinMode(trigPin, OUTPUT);  // Set the trigPin as OUTPUT
  pinMode(echoPin, INPUT);   // Set the echoPin as INPUT

  delay(2000); // Wait for 2 seconds to ensure the module is ready
  mfrc522.PCD_Init(); // Initialize RFID module
  Serial.println("RFID module initialized.");   
  SPI.begin();     // Setting SCK, MOSI, and MISO to outputs

  Serial.println("System Initialized...");
  Serial.println("Place RFID card near reader to control the door.");
}

void loop() {
  // Check for an RFID card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    handleRFID();
  }

  // If in the first cycle and waiting for ultrasonic sensor input, monitor the ultrasonic sensor
  if (waitingForUltrasonic && state == 0) {
    monitorUltrasonicSensor();
  }
}

// Monitor the HC-SR04 sensor for vehicle detection
void monitorUltrasonicSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the pulse
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  distance = (duration / 2) / 29.1;

  // Output the distance to the Serial Monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // If a vehicle is detected, close the door automatically
  if (distance > 0 && distance < 3) {
    Serial.println("Vehicle detected. Closing the door...");
    closeDoor();
    waitingForUltrasonic = false; // Stop monitoring ultrasonic sensor
    state = 1;                    // Transition to the second cycle
  }
}

void handleRFID() {
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  content = content.substring(1);

  // Add valid RFID card values here
  if (content == "83 95 E0 E1" || content == "41 AB 96 3F" || content == "17 C6 DF 73") {
    Serial.println("Valid RFID card detected!");

    if (!doorOpen) {
      // Open the door if it's closed
      openDoor();
      if (state == 0) {
        waitingForUltrasonic = true; // Start monitoring ultrasonic sensor in the first cycle
      } else {
        Serial.println("Door will remain open until RFID is presented again in the second cycle.");
      }
    } else {
      // Close the door in the second cycle
      if (state == 1) {
        closeDoor();
        Serial.println("Door closed via RFID in the second cycle.");
        state = 0; // Transition back to the first cycle
      }
    }
  } else {
    Serial.println("Invalid RFID card detected.");
  }
}

// Open the door
void openDoor() {
  Serial.println("Opening the door...");
  stepper.setSpeed(14);
  stepper.step(revolution * 3 / 4); // Rotate motor 90 degrees anti-clockwise
  delay(1000);
  doorOpen = true;
  Serial.println("Door is now OPEN.");
}

// Close the door
void closeDoor() {
  Serial.println("Closing the door...");
  stepper.setSpeed(14);
  stepper.step(-revolution * 3 / 4); // Rotate motor to close the door
  delay(1000); // Wait for the motor to finish

  // Add delay to allow the IR sensor to stabilize
  delay(500);

  // Retry mechanism for IR sensor detection (expecting LOW signal at closed position)
  unsigned long startTime = millis();
  const unsigned long timeout = 10000; // 10 seconds timeout

  while (digitalRead(IR) != LOW) { // IR sensor outputs LOW at closed position
    if (millis() - startTime > timeout) {
      Serial.println("Timeout: IR sensor did not detect closed position. Check alignment or system.");
      return; // Exit if timeout occurs
    }
    delay(100); // Brief delay before checking again
  }

  // If sensor eventually detects the closed position
  doorOpen = false;
  Serial.println("Door is now CLOSED.");
}
