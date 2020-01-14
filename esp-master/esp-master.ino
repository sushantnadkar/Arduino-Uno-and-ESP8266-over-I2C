#include <Wire.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


#define I2CAddressESPWifi 8

#define SECRET_SSID "SSID"
#define SECRET_PASS "PASSWORD"

#define SECRET_CH_ID 000000 // 6 digit channel id
#define SECRET_WRITE_APIKEY "API_KEY"

int x=32;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClient  client;
WiFiClientSecure  client_secure;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;


void setup() {
  Serial.begin(115200);
  Wire.begin(0,2);//Change to Wire.begin() for non ESP.
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);  
    } 
    Serial.println("\nConnected.");
  }
}

void loop() {
  
  
  Wire.beginTransmission(I2CAddressESPWifi);
  Wire.write(x);
  Wire.endTransmission();
  x++;
  delay(1);//Wait for Slave to calculate response.
  Wire.requestFrom(I2CAddressESPWifi,6);
  String str = "";
  Serial.print("Request Return:[");
  while (Wire.available()) {
    delay(1);
    char c = Wire.read();
    str += c;
  }
  if(str.length() > 0) {
    Serial.print(str);
    Serial.println("]");
    String humidity = str.substring(0,2);
    Serial.println(humidity);
    int mid_int, low_int;
    String mid = str.substring(3,4);
    String low = str.substring(5);
    String status = "";
    if(mid == "f" && low == "f") {
      status = "Tank full";
      mid_int = 0;
      low_int = 0;
    } else if(mid == "t" && low == "f") {
      status = "Tank half";
      mid_int = 1;
      low_int = 0;
    } else if(mid == "t" && low == "t") {
      status = "Tank low";
      mid_int = 1;
      low_int = 1;
    }
    
    ThingSpeak.setField(1, atoi(humidity.c_str()));
    ThingSpeak.setField(2, mid_int);
    ThingSpeak.setField(3, low_int);
    ThingSpeak.setStatus(status);
    
    int request_status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(request_status == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(request_status));
    }
  }
  
  delay(20000);
}
