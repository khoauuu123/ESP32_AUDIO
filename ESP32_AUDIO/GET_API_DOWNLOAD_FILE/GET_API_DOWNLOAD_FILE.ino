#include "WiFi.h"
#include <HTTPClient.h>
#define WIFI_SSID "MEIGROUP01 - 2.4GHZ"  //"Khoa"
#define WIFI_PASSWORD "77namhai88"       //"10121999"

#include "SD.h"
#define SD_CS 13
#define SPI_MOSI 15
#define SPI_MISO 2
#define SPI_SCK 14

void setup() {
  Serial.begin(115200);
  delay(200);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);
  SD.begin(SD_CS);

  Serial.print("Wifi Conneting");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.println("Wifi Connected");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  Download_file_mp3("http://192.168.1.99/download.php", "audio.mp3");

  Serial.print("Run loop");
}

void loop() {
  
}

void Download_file_mp3(String URL, String FILENAME) {
  File file = SD.open("/" + FILENAME, FILE_WRITE);
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  Serial.println("URL: " + URL);
  if (http.begin(URL)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      int len = http.getSize();
      Serial.println("len: " + String(len));
      uint8_t buf[1024];
      WiFiClient* stream = http.getStreamPtr();
      int dung_luong = len;
      int phantram, phantram_ol = -1;
      uint32_t TIME_RUN_CHECK = millis();
      while (http.connected() && (len > 0 || len == -1)) {
        size_t size = stream->available();
        if (size) {
          int c = stream->readBytes(buf, ((size > sizeof(buf)) ? sizeof(buf) : size));
          file.write(buf, c);
          if (len > 0) {
            len -= c;
          }
          //Serial.println(len);
          int phantram = (float((dung_luong - len)) / float(dung_luong)) * 100;
          if (phantram != phantram_ol) {
            phantram_ol = phantram;
            Serial.print("Download: ");
            Serial.print(phantram);
            Serial.println("%");
          }
          TIME_RUN_CHECK = millis();
        } 
        else {
          if((millis() - TIME_RUN_CHECK) > 20000){
             Serial.print("Download failed!");
             break;
          }
        }
      }
      Serial.println();
      Serial.print("[HTTP] connection closed or file end.\n");
      file.close();
    } else {
      Serial.print("HTTP error: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}