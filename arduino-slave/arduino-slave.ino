#include <Wire.h>


#define I2CAddressESPWifi 8

int pump_pin = 9;
int low_level_pin = 6;
int mid_level_pin = 7;
int humi_pin = A2;

int humi = 0;
unsigned char check_connection = 0;
unsigned char times_check = 0;
int countTrueCommand;
int countTimeCommand; 
boolean found = false;

boolean mid_alert_flag = false;
boolean low_alert_flag = false;

char mid[2];
char low[2];
String str_humi;
char char_humi[3];

void setup() {
  pinMode(pump_pin, OUTPUT);
  pinMode(humi_pin, INPUT);
  pinMode(low_level_pin, INPUT_PULLUP);
  pinMode(mid_level_pin, INPUT_PULLUP);
 
  digitalWrite(pump_pin, HIGH);
  
  Serial.begin(9600);
  Wire.begin(I2CAddressESPWifi);
  
  Wire.onReceive(espWifiReceiveEvent);
  Wire.onRequest(espWifiRequestEvent);
}

void loop() {
  
  boolean mid_status = digitalRead(mid_level_pin);
  boolean low_status = digitalRead(low_level_pin);

  if(mid_status == HIGH) {
    strcpy(mid, "t");
  } else {
    strcpy(mid, "f");
  }
  if(low_status == HIGH) {
    strcpy(low, "t");
  } else {
    strcpy(low, "f");
  }

  if( mid_status == HIGH && !mid_alert_flag) {
    Serial.println("Water level mid");
    // send alert
    mid_alert_flag = true;
  } else if( low_status == HIGH && mid_alert_flag && !low_alert_flag) {
    Serial.println("Water level low");
    // send alert
    low_alert_flag = true;
  }
  if( low_status == LOW ) {
    low_alert_flag = false;
  }
  if( mid_status == LOW ) {
    mid_alert_flag = false;
  }

  humi = analogRead(humi_pin);
  humi = map(humi, 1023, 0, 10, 99);
  str_humi = String(humi);
  str_humi.toCharArray(char_humi,3);
  delay(10);

  if(humi <= 50) {
    digitalWrite(pump_pin, LOW);
    Serial.println("Pump on");
    delay(2000);
    digitalWrite(pump_pin, HIGH);
    Serial.println("Pump off");
  }
  delay(5000);
}

void espWifiReceiveEvent(int count) {
  Serial.print("Received[");
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.println("]");
  //calc response.
}

void espWifiRequestEvent() {
  Serial.print(char_humi);
  Serial.print(',');
  Serial.print(mid);
  Serial.print(',');
  Serial.println(low);
  
  Wire.write(char_humi);
  Wire.write(',');
  Wire.write(mid);
  Wire.write(',');
  Wire.write(low);
}
