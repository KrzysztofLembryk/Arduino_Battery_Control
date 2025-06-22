#include "routes.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "../global_vars/global_vars.h"
#include "../http_handler/http_handler.h"
#include "../../include/error_constants.h"

// helper functions pre-declarations
void handle_set_data_ret_code(int result, AsyncWebServerRequest *request);

// /serverIP
void get_server_ip_addr(AsyncWebServerRequest *request)
{
    request->send(HTTP_CODE_OK, "text/plain", WiFi.localIP().toString().c_str());
}

void printRequestDetails(AsyncWebServerRequest *request) {
  // Basic request info
  Serial.println("\n-------- REQUEST DETAILS --------");
  Serial.printf("Method: %s\n", request->methodToString());
  Serial.printf("URL: %s\n", request->url().c_str());
  Serial.printf("Host: %s\n", request->host().c_str());
  Serial.printf("Content Type: %s\n", request->contentType().c_str());
  Serial.printf("Content Length: %d\n", request->contentLength());
  
  // Headers
  Serial.println("\n-------- HEADERS --------");
  int headers = request->headers();
  for(int i=0; i<headers; i++) {
    const AsyncWebHeader* h = request->getHeader(i);
    Serial.printf("Header[%s]: %s\n", h->name().c_str(), h->value().c_str());
  }
  
  // URL Parameters (GET parameters)
  Serial.println("\n-------- URL PARAMETERS --------");
  int params = request->params();
  for(int i=0; i<params; i++) {
    const AsyncWebParameter* p = request->getParam(i);
    Serial.printf("Parameter[%s]: %s\n", p->name().c_str(), p->value().c_str());
  }
  
  // POST parameters
  Serial.println("\n-------- POST PARAMETERS --------");
  int postParams = request->params();
  for(int i=0; i<postParams; i++) {
    const AsyncWebParameter* p = request->getParam(i);
    if(p) {
      Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }
  
  // Request body (for POST/PUT with JSON or other raw data)
  if(request->hasParam("plain", true)) {
    Serial.println("\n-------- REQUEST BODY --------");
    Serial.println(request->getParam("plain", true)->value());
  }
  
  Serial.println("\n--------------------------------");
  Serial.flush();
}


// /userData
void recv_charging_times_from_user(AsyncWebServerRequest *request, 
                                    JsonVariant &json)
{
    // Serial.println("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    // Serial.println("@@@@@@@@RECV DATA FROM USER@@@@@@@@");
    // Serial.println("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    // printRequestDetails(request);
    JsonObject json_obj = json.as<JsonObject>();

    handle_set_data_ret_code(server_data.set_data(json_obj,
                                        CHARGING_DATA_KEY,
                                        CHARGING_MODE_KEY), 
                                        request);
    return;
}


void get_user_interface(AsyncWebServerRequest *request)
{
  digitalWrite(LED_BUILTIN, HIGH);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
  </html>",

           hr, min % 60, sec % 60);
  request->send(200, "text/html", temp);
  digitalWrite(LED_BUILTIN, LOW);
}


// helper functions impl
void handle_set_data_ret_code(int result, AsyncWebServerRequest *request)
{
    switch (result)
    {
    case SUCCESS:
        request->send(HTTP_CODE_OK, "text/plain", "Data successfully set");
        break;

    case ERROR_JSON_WRONG_RECVD_ARR_SIZE:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Received array size is incorrect, should be " + String(NBR_OF_INTERVALS));
        break;

    case ERROR_JSON_UNDER_DATA_KEY_NOT_ARRAY:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Data under:" + String(CHARGING_DATA_KEY) + " key is not an array");
        break;

    case ERROR_JSON_UNSUPPORTED_MODE_RCVD:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Unsupported mode received");
        break;

    case ERROR_JSON_NO_MODE_KEY:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Mode key is missing in JSON");
        break;

    default:
        request->send(HTTP_CODE_INTERNAL_SERVER_ERROR, "text/plain", "Error: Unknown error occurred");
        break;
    }
}
