#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font7Seg.h"
#include <DS3231.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// Real-Time Clock (RTC) and Matrix Display
DS3231 Clock;
#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
#define SPEED_TIME 75
#define PAUSE_TIME 0
#define MAX_MESG 20
char szTime[9];
char szMesg[MAX_MESG + 1] = "";
bool Century = false, h12, PM;
byte dd, mm, yyy;
uint16_t h, m, s;

// LDR and LED
const int ldr_pin = 7;
const int led_pin = 3;

// Bluetooth and Relay
SoftwareSerial BTSerial(0, 1); // RX, TX
const int relayPin = 6;

// Function Prototypes
char *mon2str(uint8_t mon, char *psz, uint8_t len);
char *dow2str(uint8_t code, char *psz, uint8_t len);
void getTime(char *psz, bool f = true);
void getDate(char *psz);

void setup() {
  // Setup RTC and Matrix Display
  P.begin(2);
  P.setInvert(false);
  Wire.begin();
  P.setZone(0, MAX_DEVICES - 4, MAX_DEVICES - 1);
  P.setZone(1, MAX_DEVICES - 4, MAX_DEVICES - 1);
  P.displayZoneText(1, szTime, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(0, szMesg, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);

  // Setup LDR and LED
  pinMode(ldr_pin, INPUT);
  pinMode(led_pin, OUTPUT);

  // Setup Bluetooth and Relay
  pinMode(relayPin, OUTPUT);
  BTSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Bluetooth is ready and waiting for commands...");
}

void loop() {
  static uint32_t lastTime = 0;
  static uint8_t display = 0;
  static bool flasher = false;

  // RTC and Matrix Display Logic
  P.displayAnimate();
  if (P.getZoneStatus(0)) {
    switch (display) {
      case 0:
        P.setPause(0, 1000);
        P.setTextEffect(0, PA_OPENING, PA_CLOSING);
        display++;
        dtostrf(Clock.getTemperature(), 3, 1, szMesg);
        strcat(szMesg, "C"); // Corrected to show the degree symbol
        break;

      case 1:
        P.setTextEffect(0, PA_OPENING, PA_CLOSING);
        display++;
        dtostrf((1.8 * Clock.getTemperature()) + 32, 3, 1, szMesg);
        strcat(szMesg, "F"); // Corrected to show the degree symbol
        break;

      case 2:
        P.setFont(0, numeric7Seg);
        P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
        P.setPause(0, 0);
        if (millis() - lastTime >= 1000) {
          lastTime = millis();
          getTime(szMesg, flasher);
          flasher = !flasher;
        }
        if (s == 0 && s <= 30) {
          display++;
          P.setTextEffect(0, PA_PRINT, PA_SCROLL_UP);
        }
        break;

      case 3:
        P.setFont(0, nullptr);
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display++;
        dow2str(Clock.getDoW() + 1, szMesg, MAX_MESG);
        break;

      default:
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display = 0;
        getDate(szMesg);
        break;
    }
    P.displayReset(0);
  }

  // LDR and LED Logic
  if (digitalRead(ldr_pin) == HIGH) {
    digitalWrite(led_pin, LOW);
  } else {
    digitalWrite(led_pin, HIGH);
  }
  Serial.println(digitalRead(ldr_pin));

  // Bluetooth and Relay Logic
  if (BTSerial.available()) {
    String command = BTSerial.readStringUntil('\n');
    command.trim();
    Serial.println(command);
    if (command.equalsIgnoreCase("FAN ON")) {
      digitalWrite(relayPin, HIGH);
      Serial.println("Fan is ON");
    } else if (command.equalsIgnoreCase("FAN OFF")) {
      digitalWrite(relayPin, LOW);
      Serial.println("Fan is OFF");
    } else {
      Serial.println("Unknown command");
    }
  }
}

// Utility Functions
char *mon2str(uint8_t mon, char *psz, uint8_t len) {
  static const __FlashStringHelper *str[] = {F("Jan"), F("Feb"), F("Mar"), F("Apr"), F("May"), F("Jun"), F("Jul"), F("Aug"), F("Sep"), F("Oct"), F("Nov"), F("Dec")};
  strncpy_P(psz, (const char PROGMEM *)str[mon - 1], len);
  psz[len] = '\0';
  return psz;
}

char *dow2str(uint8_t code, char *psz, uint8_t len) {
  static const __FlashStringHelper *str[] = {F("Sunday"), F("Monday"), F("Tuesday"), F("Wednesday"), F("Thursday"), F("Friday"), F("Saturday")};
  strncpy_P(psz, (const char PROGMEM *)str[code - 1], len);
  psz[len] = '\0';
  return psz;
}

void getTime(char *psz, bool f) {
  s = Clock.getSecond(); // Get seconds
  m = Clock.getMinute(); // Get minutes
  h = Clock.getHour(h12, PM); // Get hours
  sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m); // Format time as "HH:MM" or "HH MM" depending on the 'f' flag
}

void getDate(char *psz) {
  char szBuf[10]; // Temporary buffer for storing month string
  dd = Clock.getDate(); // Get day of the month
  mm = Clock.getMonth(Century); // Get month
  yyy = Clock.getYear(); // Get year
  sprintf(psz, "%d %s %04d", dd, mon2str(mm, szBuf, sizeof(szBuf) - 1), (yyy + 2000)); // Format the date as "DD Mon YYYY"
}
