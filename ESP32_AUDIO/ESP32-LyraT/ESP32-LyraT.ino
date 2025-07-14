#include "WiFi.h"
#include <HTTPClient.h>
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#include "SPI.h"
#include "SD.h"
#include "Wire.h"
#include "ES8388.h"  // https://github.com/maditnerd/es8388
#include "Audio.h"   // https://github.com/schreibfaul1/ESP32-audioI2S
#define SD_CS 13
#define SPI_MOSI 15
#define SPI_MISO 2
#define SPI_SCK 14

// I2S GPIOs
#define I2S_SDOUT 26
#define I2S_BCLK 5
#define I2S_LRCK 25
#define I2S_MCLK 0
// I2C GPIOs
#define IIC_CLK 23
#define IIC_DATA 18

int volume = 80;  // 0...100

ES8388 es;
Audio audio;

// Amplifier enable
#define GPIO_PA_EN 35

const int ledPin = 22;

const int threshold = 20;
const int Play = 33;
const int Set = 32;
const int Vol_giam = 13;
const int Vol_tang = 27;
int Vol;


void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(ledPin, OUTPUT);

  // SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  // SPI.setFrequency(1000000);
  // SD.begin(SD_CS);

  Serial.print("Wifi Conneting");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.println("Wifi Connected");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  Serial.printf("Connect to ES8388 codec... ");
  while (not es.begin(IIC_DATA, IIC_CLK, 1000000)) {  // frequency = 1000000
    Serial.printf("Failed!\n");
    delay(1000);
  }
  Serial.printf("OK\n");
  //Setup ES8388
  es.volume(ES8388::ES_MAIN, volume);
  es.volume(ES8388::ES_OUT1, volume);
  es.volume(ES8388::ES_OUT2, volume);
  es.mute(ES8388::ES_OUT1, false);
  es.mute(ES8388::ES_OUT2, false);
  es.mute(ES8388::ES_MAIN, false);

  // Enable amplifier
  pinMode(GPIO_PA_EN, OUTPUT);
  digitalWrite(GPIO_PA_EN, HIGH);

  audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_SDOUT);
  audio.i2s_mclk_pin_select(I2S_MCLK);
  Vol = 21;
  audio.setVolume(Vol);  // 0...21

  //audio.connecttospeech("xin chào", "de");
  //audio.connecttoFS(SD, "audio_den.mp3");
  audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3");
  //audio.connecttohost("http://mp3channels.webradio.antenne.de:80/oldies-but-goldies");
  //audio.connecttohost("https://ep256.hostingradio.ru:8052/europaplus256.mp3");
  //audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
  //audio.connecttohost("https://wpr-ice.streamguys1.com/wpr-ideas-mp3-64");
  //audio.connecttohost("http://somafm.com/wma128/missioncontrol.asx"); //  asx
  //audio.connecttohost("http://192.168.1.99/audio.php?filename=amthanh1.mp3");
}

void loop() {
  audio.loop();

  if (Serial.available()) {  // put streamURL in serial monitor
    audio.stopSong();
    String r = Serial.readString();
    r.trim();
    Serial.println(r);
    if (r.length() > 5) audio.connecttohost(r.c_str());
    log_i("free heap=%i", ESP.getFreeHeap());
  }

  if (touchRead(Vol_tang) < threshold) {
    if (Vol < 21) {
      Vol++;
      audio.setVolume(Vol);  // 0...21
      Serial.println("Vol: " + String(Vol));
    }
    delay(200);
  }

  if (touchRead(Vol_giam) < threshold) {
    if (Vol > 0) {
      Vol--;
      audio.setVolume(Vol);  // 0...21
      Serial.println("Vol: " + String(Vol));
    }
    delay(200);
  }
}  // END LOOP

void audio_info(const char* info) {
  Serial.print("audio_info: ");
  Serial.println(info);
}