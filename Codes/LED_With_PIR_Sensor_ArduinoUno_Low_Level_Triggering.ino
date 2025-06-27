// Define pins
const int pirPin = 4;    // PIR sensor output pin
const int RELAY_Pin = 2;   // LED pin

void setup() {
  pinMode(pirPin, INPUT);  // Set PIR sensor pin as input
  pinMode(RELAY_Pin, OUTPUT); // Set LED pin as output
  Serial.begin(9600);      // Initialize serial communication for debugging
}

void loop() {
  int motionDetected = digitalRead(pirPin); // Read PIR sensor

  if (motionDetected == HIGH) {
    digitalWrite(RELAY_Pin, LOW);
    delay(7000);// Turn on LED
    Serial.println("Motion detected!");
  } else {
    digitalWrite(RELAY_Pin, HIGH);  // Turn off LED
    Serial.println("No motion.");
  }

  delay(1000); // Small delay for stability
}
