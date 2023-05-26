#if defined(ESP32)
#include <WiFi.h>
#include <RH_ASK.h>
#include <SPI.h>
RH_ASK rf_driver;
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif


#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>
#include <unistd.h>



#define WIFI_SSID "Nishith's Nord"
#define WIFI_PASSWORD "nishith@nishadam"

#define API_KEY "AIzaSyCQNeCwKC8NViJe0SdEv41_cdkJaOIFM4g"
#define DATABASE_URL "https://pill-dispenser-6ad77-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

int count = 1;
float floatValue;
unsigned long count1 = 0;
char buf[20];
int distance; 


#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>


SoftwareSerial mySerial(4,5);


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int LEDPIN = 25;  // white led
const int LEDPIN3 = 32;  // yellow led
int count2 = 0;
int totalpill = 2;   //number of pills to be dispensed

#include <SPI.h>




#include <SoftwareSerial.h>
SoftwareSerial myserial1(18,19);  // rx,tx  sim900  net vcc rst rxd txd gnd
char msg;
char call;

#include <string.h>
#include <math.h>









int dbsize;
#include <Stepper.h>
const int STEPS_PER_REV = 200;
int motorSpeed = 10;
Stepper stepper_NEMA17(STEPS_PER_REV, 12, 14, 27, 26);


#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;



void concatenate_string(char* s, char* s1)
{
    int i;
 
    int j = strlen(s);
 
    for (i = 0; s1[i] != '\0'; i++) {
        s[i + j] = s1[i];
    }
 
    s[i + j] = '\0';
 
    return;
}



void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}





void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LEDPIN,OUTPUT);
  pinMode(LEDPIN3,OUTPUT);
  myserial1.begin(9600); 
  

  

  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.println("connecting to wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }


  Serial.println();
  Serial.print("connected with IP:");
  Serial.println(WiFi.localIP());
  Serial.printf("firebase client v%s\n\n",FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(DATABASE_URL,API_KEY);
  Firebase.setDoubleDigits(5);
  
  Wire.begin();

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();


  Serial.println("ready to write value");

  
  Serial.println("ENROLL test");
  mySerial.begin(57600);
  if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) yield();
    }

  Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
    Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
    Serial.print(F("Security level: ")); Serial.println(finger.security_level);
    Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
    Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
    Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

    finger.getTemplateCount();
    if (finger.templateCount == 0) {
      Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else {
      Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    }

  
  

}

void loop() {
  // put your main code here, to run repeatedly:
  getFingerprintID();
  delay(50);            //don't ned to run this at full speed.

}



uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
    for(int i=0;i<25;i++){
      digitalWrite(LEDPIN,HIGH);
      digitalWrite(LEDPIN3,HIGH);
      delay(100);
      digitalWrite(LEDPIN,LOW);
      digitalWrite(LEDPIN3,LOW);
      delay(100);
      
    }
    int count3 = 0;
    Serial.println("starting motor");
    do{
      distance = sensor.readRangeContinuousMillimeters();
      Serial.println(distance);
      if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
      if(motorSpeed<220){
        motorSpeed = motorSpeed + 1;
      }
      stepper_NEMA17.setSpeed(motorSpeed);
      stepper_NEMA17.step(STEPS_PER_REV / 100);
      }while(distance<40);
    /*
    time_t endwait;
    time_t start = time(NULL);
    time_t seconds = 10;

    endwait = start + seconds;
    while (start < endwait){
      if(motorSpeed<220){
        motorSpeed = motorSpeed + 1;
      }
      stepper_NEMA17.setSpeed(motorSpeed);
      stepper_NEMA17.step(STEPS_PER_REV / 100);
      start = time(NULL);
    }*/
    Serial.println("stopping motor");
    while(1){}
    /*while(1){
      if(distance<5){
        if(count3>=totalpill){
          esc.write(0);
          while(1){
            esc.write(0);
          }
        }
        else{
          count3+=1;
          Serial.println("count1 val is ");
          Serial.print(count3);
          digitalWrite(LEDPIN,HIGH);
          digitalWrite(LEDPIN3,HIGH);
          delay(50);
          digitalWrite(LEDPIN,LOW);
          digitalWrite(LEDPIN3,LOW);
        }
      }
    }*/
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    count2 = count2 + 1;
    Serial.println(3-count2);
    Serial.print(" attempts remaining");
    digitalWrite(LEDPIN3,HIGH);
    delay(250);
    digitalWrite(LEDPIN3,LOW);
    if(count2==3){
      if (Firebase.RTDB.getFloat(&fbdo, "/contact/part1")) {
        floatValue = fbdo.floatData();
        Serial.println(fbdo.dataType());
        FirebaseJsonArray &arr = fbdo.jsonArray();
        dbsize = arr.size();
        String arrStr1;
        arr.toString(arrStr1, true);
        Serial.println("first value");
        Serial.println(arrStr1);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
      }
      Serial.println("db size is ");
      Serial.print(dbsize);
      int p1[dbsize];
      int p2[dbsize];
      int p3[dbsize];
      if (Firebase.RTDB.getFloat(&fbdo, "/contact/part1")) {
        if(1){
          floatValue = fbdo.floatData();
          Serial.println(fbdo.dataType());
          FirebaseJsonArray &arr1 = fbdo.jsonArray();
          String arrStr1;
          arr1.toString(arrStr1, true);
          Serial.println(arrStr1);
          Serial.println();
          Serial.println("Iterate array values:");
          Serial.println();
          
          for (size_t i = 0; i < arr1.size(); i++){
            Serial.print(i);
            Serial.print(", Value: ");
            FirebaseJsonData &jsonData = fbdo.jsonData();
            arr1.get(jsonData, i);
            p1[i] = jsonData.intValue;
            Serial.println(jsonData.intValue);
          }
          Serial.println("float value");
          Serial.println(floatValue);
        }
      }
      if (Firebase.RTDB.getFloat(&fbdo, "/contact/part2")) {
        if(1){
          floatValue = fbdo.floatData();
          Serial.println(fbdo.dataType());
          FirebaseJsonArray &arr2 = fbdo.jsonArray();
          String arrStr2;
          arr2.toString(arrStr2, true);
          Serial.println(arrStr2);
          Serial.println();
          Serial.println("Iterate array values:");
          Serial.println();
          
          for (size_t i = 0; i < arr2.size(); i++){
            Serial.print(i);
            Serial.print(", Value: ");
            FirebaseJsonData &jsonData = fbdo.jsonData();
            arr2.get(jsonData, i);
            p2[i] = jsonData.intValue;
            Serial.println(jsonData.intValue);
        }
      }
      if (Firebase.RTDB.getFloat(&fbdo, "/contact/part3")) {
        if(1){
          floatValue = fbdo.floatData();
          Serial.println(fbdo.dataType());
          FirebaseJsonArray &arr3 = fbdo.jsonArray();
          String arrStr3;
          arr3.toString(arrStr3, true);
          Serial.println(arrStr3);
          Serial.println();
          Serial.println("Iterate array values:");
          Serial.println();
          
          for (size_t i = 0; i < arr3.size(); i++){
            Serial.print(i);
            Serial.print(", Value: ");
            FirebaseJsonData &jsonData = fbdo.jsonData();
            arr3.get(jsonData, i);
            p3[i] = jsonData.intValue;
            Serial.println(jsonData.intValue);
          }
          Serial.println("float value");
          Serial.println(floatValue);
        }
      }
      Serial.println("printing phone numbers");
      for(int i=0;i<dbsize;i++){
        Serial.print(p1[i]);
        Serial.print(p2[i]);
        Serial.println(p3[i]);
        char c[50] = "ATD+",c1[5];
        sprintf(c1, "%d", p1[i]);
        int strcnt = strlen(c1);
        for(int i = 0;i<4-strcnt;i++){
          concatenate_string(c, "0");
        }
        concatenate_string(c, c1);
        sprintf(c1, "%d", p2[i]);
        strcnt = strlen(c1);
        for(int i = 0;i<4-strcnt;i++){
          concatenate_string(c, "0");
        }
        concatenate_string(c, c1);
        sprintf(c1, "%d", p3[i]);
        strcnt =strlen(c1);
        for(int i = 0;i<4-strcnt;i++){
          concatenate_string(c, "0");
        }
        concatenate_string(c, c1);
        char c2[2] = ";";
        concatenate_string(c, c2);
        Serial.println(c);
        myserial1.println(c);
        Serial.println("Calling  ");
        delay(25000);
        myserial1.println("ATH");
        Serial.println("Hangup Call");
        delay(1000);
      }
      while(1){}
    }
      
      while(1){
        delay(1000);
      }
    }
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
