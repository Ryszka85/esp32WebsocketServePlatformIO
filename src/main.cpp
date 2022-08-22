#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

 
const char *ssid     = "TP-Link_F1D2";
const char *password = "Albertynskie28";

// Timer variables
unsigned long lastTime = 0;  
const unsigned long TIMER_DELAY = 10000;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String extractString(String _received) {
  char startingChar = '{';
  char finishingChar = '}';

  String tmpData = "";
  bool _flag = false;
  for (int i = 0; i < _received.length(); i++) {
    char tmpChar = _received[i];
    if (tmpChar == startingChar) {
      tmpData += startingChar;
      _flag = true;
    }
    else if (tmpChar == finishingChar) {
      tmpData += finishingChar;
      break;
    }
    else if (_flag == true) {
      tmpData += tmpChar;
    }
  }

  return tmpData;

}

void processJsonData(String _received) {
  String json = extractString(_received);
  json.replace("\\", "");
  Serial.println(json);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);
  JsonObject obj = doc.as<JsonObject>();
  String receivedMessage = obj["message"];
  Serial.println(receivedMessage);

}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;    

    String d = (char*) data;
    processJsonData(d);
    
  }
}
 
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){
    
    Serial.println("Websocket client connection received " + client->remoteIP().toString());
    client->text("Hello from ESP32 Server");
 
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client disconnected " + client->remoteIP().toString());
 
  } else if(type == WS_EVT_DATA){
    handleWebSocketMessage(arg, data, len);
    client->text("Geh scheissen");
  }
}
 
void setup(){
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());

  
 
  ws.onEvent(onWsEvent);
  
  server.addHandler(&ws);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
}
 
void loop(){
  if ((millis() - lastTime) > TIMER_DELAY) {
    Serial.println("Aha");
    lastTime = millis();
  }

}