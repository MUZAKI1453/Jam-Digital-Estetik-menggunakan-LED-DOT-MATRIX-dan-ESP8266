#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"

// Konfigurasi LED Matrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN D6

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
RTC_DS1307 rtc;

char buffer[10];
bool blink = true;

void setup() {
  Wire.begin(D2, D1);  // SDA = D2, SCL = D1
  rtc.begin();

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set waktu saat upload
  }

  display.begin();
  display.setIntensity(3);     // kecerahan 0-15
  display.displayClear();
  display.setTextAlignment(PA_CENTER);  // Tampilkan teks di tengah
}

void loop() {
  DateTime now = rtc.now();

  // Berkedip: ganti ":" dengan " " setiap detik
  if (blink) {
    sprintf(buffer, "%02d : %02d", now.hour(), now.minute());
  } else {
    sprintf(buffer, "%02d   %02d", now.hour(), now.minute());
  }
  blink = !blink; // Toggle berkedip

  display.displayClear();
  display.print(buffer);

  delay(1000); // Update tiap detik
}
