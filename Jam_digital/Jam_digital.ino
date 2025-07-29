#include <Wire.h>
#include <RTClib.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Konfigurasi hardware MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // Jumlah panel 8x8

#define DATA_PIN  D8
#define CLK_PIN   D5
#define CS_PIN    D7

// Objek display & RTC
MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
RTC_DS1307 rtc;

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1);  // SDA = D2 (GPIO4), SCL = D1 (GPIO5)

  // Inisialisasi RTC
  if (!rtc.begin()) {
    Serial.println("RTC tidak terdeteksi!");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC tidak berjalan. Menyetel waktu ke waktu kompilasi.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Inisialisasi Display
  display.begin();
  display.setIntensity(5);       // kecerahan (0-15)
  display.setTextAlignment(PA_CENTER);
  display.displayClear();
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long nowMillis = millis();

  if (nowMillis - lastUpdate >= 1000) {
    lastUpdate = nowMillis;

    DateTime now = rtc.now();
    char waktu[6]; // Format HH:MM
    sprintf(waktu, "%02d : %02d", now.hour(), now.minute());

    Serial.println(waktu);  // Debug di Serial Monitor

    display.displayClear();
    display.displayText(waktu, PA_CENTER, 100, 1000, PA_PRINT, PA_NO_EFFECT);
  }

  if (display.displayAnimate()) {
    display.displayReset();
  }
}
