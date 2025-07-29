#include <Wire.h>
#include <RTClib.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Konfigurasi hardware MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // Jumlah panel 8x8 (32x8 LED matrix)

#define DATA_PIN  D8
#define CLK_PIN   D5
#define CS_PIN    D7

// Objek display & RTC
MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
RTC_DS1307 rtc;

// Variabel untuk menyimpan menit terakhir
int lastMinute = -1;

// Frame animasi Pacman (8x8)
// Pacman mulut terbuka ke kiri, dengan mata di atas
byte pacmanOpen[8] = {
  B00111000,
  B01111100,
  B11110110,  // ← ada mata di bit ke-3 (1) dan bit ke-5 (mata)
  B00011110,
  B00011110,
  B11111110,
  B01111100,
  B00111000
};

// Pacman mulut tertutup, mata tetap di atas
byte pacmanClose[8] = {
  B00111000,
  B01111100,
  B11110110,  // ← tetap ada mata di atas
  B11111110,
  B11111110,
  B11111110,
  B01111100,
  B00111000
};



// Fungsi untuk menampilkan animasi Pacman berjalan dari kiri ke kanan
void tampilkanAnimasiPacman() {
  MD_MAX72XX *ledMatrix = display.getGraphicObject();

  for (int offset = MAX_DEVICES * 8; offset >= -8; offset--) {
    ledMatrix->clear();

    // Ganti frame setiap langkah
    byte* currentFrame = (offset % 2 == 0) ? pacmanOpen : pacmanClose;

    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (bitRead(currentFrame[row], 7 - col)) {
          int drawX = offset + col;
          if (drawX >= 0 && drawX < MAX_DEVICES * 8) {
            ledMatrix->setPoint(row, drawX, true);
          }
        }
      }
    }

    ledMatrix->update();  // Refresh display
    delay(80);
  }

  ledMatrix->clear();  // Bersihkan setelah animasi
  ledMatrix->update();
}



void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1);  // SDA = D2 (GPIO4), SCL = D1 (GPIO5)

  if (!rtc.begin()) {
    Serial.println("RTC tidak terdeteksi!");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC tidak berjalan. Menyetel waktu ke waktu kompilasi.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  display.begin();
  display.setIntensity(5); // kecerahan (0-15)
  display.setTextAlignment(PA_CENTER);
  display.displayClear();
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long nowMillis = millis();

  if (nowMillis - lastUpdate >= 1000) {
    lastUpdate = nowMillis;

    DateTime now = rtc.now();
    int currentMinute = now.minute();

    // Setiap 1 menit dan hanya sekali per menit
    if (currentMinute % 1 == 0 && currentMinute != lastMinute) {
      tampilkanAnimasiPacman();
      lastMinute = currentMinute;
    }

    char waktu[6];
    sprintf(waktu, "%02d : %02d", now.hour(), now.minute());
    Serial.println(waktu);

    display.displayClear();
    display.displayText(waktu, PA_CENTER, 100, 1000, PA_PRINT, PA_NO_EFFECT);
  }

  if (display.displayAnimate()) {
    display.displayReset();
  }
}