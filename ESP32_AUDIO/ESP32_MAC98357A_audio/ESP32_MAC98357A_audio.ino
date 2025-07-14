#include "WiFi.h"
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#include "Audio.h"

#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC  26

Audio audio;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.print("Wifi Conneting");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.println("Wifi Connected");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(100);
  audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3");
}

void loop() {
  audio.loop();
}

void audio_info(const char *info) {
  Serial.print("audio_info: ");
  Serial.println(info);
}