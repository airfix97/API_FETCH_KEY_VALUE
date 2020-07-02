#include <FS.h>
#include "SPIFFS.h" 
#include <WiFi.h>              
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "FreeRTOS.h"

const char* ssid = "airfix";
const char* password = "1122334455";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED){
    Serial.print("...");
    delay(500);
  }

  Serial.println("\n Connected to WiFi");

  if(!SPIFFS.begin(true)){
    Serial.println("There was an error mounting SPIFFS");       // border/failure case
    return;
  }

  xTaskCreate(Task1, "Task1", 1000, NULL, 2, NULL);             // Task1 for fetching the API and storing in SD card

}

void loop() {

}

void Task1(void *p){
    if (WiFi.status() == WL_CONNECTED){
    HTTPClient client;
    client.begin("http://demo4657392.mockable.io/list-tag-ids");
    int httpCode = client.GET();

    if(httpCode > 0){
      String payload = client.getString();
      Serial.println(String(httpCode));

      File file = SPIFFS.open("/api.txt", FILE_WRITE);

      if(!file){
        Serial.println("There was an error opening the file for writing");    //border/failure case
        return;
      }
      if (file.print(payload)){
        Serial.print("API written to FLASH ");
      } 
      else {
        Serial.println("API write failed");
      }
      file.close();

      const size_t capacity = JSON_ARRAY_SIZE(10000) + 10001*JSON_OBJECT_SIZE(1) + 265450;      // Memory size pool for parsing JSON
      DynamicJsonDocument doc(capacity);  
    }
    else{
      Serial.println("Connection lost");        //border/failure case
    }
  }
  vTaskDelete(NULL);
}
