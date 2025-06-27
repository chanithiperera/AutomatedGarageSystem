                                                   
# Automated Garage System

An Arduino-based system for automating a garage with the following features:
- **Roller Gate**: Controlled via RFID (MFRC522) and ultrasonic sensor (HC-SR04) with an IR sensor for state detection.
- **Lighting**: PIR sensor for motion-activated lighting and LDR for low-light activation.
- **Wi-Fi Control**: Device control via Blynk app using ESP8266.
- **Display**: Dot matrix display showing time, temperature, date, and day using DS3231 RTC and MAX7219.
- **Exhaust Fan**: Bluetooth-controlled relay.

## Hardware
- **Boards**: Multiple boards (e.g., Arduino Uno for PIR, roller gate, display; ESP8266 for Blynk).
- **Components**: DS3231 RTC, MAX7219 Dot Matrix, HC-SR04 Ultrasonic, MFRC522 RFID, PIR sensor, LDR, Relay, Stepper Motor (28BYJ-48), Bluetooth module (e.g., HC-05).

## Files
- `dotMatrix_LDR_Fan.ino`: Dot matrix display, LDR, Bluetooth fan control.
- `LED_Controlled_BY_WIFI.ino`: Blynk app control via ESP8266.
- `LED_With_PIR_Sensor_ArduinoUno_Low_Level_Triggering.ino`: PIR sensor for lighting.
- `Roller_Door_System_Code.ino`: Roller gate with RFID, ultrasonic, and IR sensors.
- `DumpInfo.ino`: RFID debugging utility.
- `Font7Seg.h`: Custom font for dot matrix display.

## Setup Instructions
1. Install Arduino IDE (available at [arduino.cc](https://www.arduino.cc)).
2. Install the following libraries via Arduino IDE’s Library Manager:
   - `MD_Parola`, `MD_MAX72xx` (for dot matrix display)
   - `DS3231`, `Wire`, `SoftwareSerial` (for RTC and Bluetooth)
   - `ESP8266WiFi`, `BlynkSimpleEsp8266` (for Blynk)
   - `MFRC522`, `SPI`, `Stepper` (for RFID and stepper motor)
3. Copy `Font7Seg.h` to your Arduino libraries folder or project directory.
4. Update `LED_Controlled_BY_WIFI.ino` with your Blynk auth token, Wi-Fi SSID, and password.
5. Connect hardware as per your wiring setup (add a schematic if available).
6. Upload each `.ino` file to the appropriate board:
   - `dotMatrix_LDR_Fan.ino`: Arduino Uno (or similar) for display, LDR, Bluetooth.
   - `LED_Controlled_BY_WIFI.ino`: ESP8266.
   - `LED_With_PIR_Sensor_ArduinoUno_Low_Level_Triggering.ino`: Arduino Uno (or similar).
   - `Roller_Door_System_Code.ino`: Arduino Uno (or similar).
   - `DumpInfo.ino`: Optional for RFID testing.

## Notes
- Ensure Bluetooth pins in `dotMatrix_LDR_Fan.ino` (e.g., pins 8 and 9 for SoftwareSerial) avoid conflicts with Serial.
- `Font7Seg.h` supports digits, colon, and some letters. Expand if additional characters are needed.
- Test each sketch on your hardware before deployment.

**Happy Coding!**