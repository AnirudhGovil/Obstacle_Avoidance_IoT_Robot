#include <NewPing.h>
#include <Servo.h>
#include <MotorDriver.h>
#include <WiFiNINA.h>
#include <SPI.h>
//#include <AESLib.h>
#include <ArduinoHttpClient.h>
#include <ThingSpeak.h>

#include "AESLib.h"

#include <MotorDriver.h>

// ----------------------------------------------------------------------------------------------------------------------------------------

Servo myservo;

int trigpin = A3;
int echopin = A0;
int BotObstacleDistance = 100;
int BotObstacleDistancel = 0;
int BotObstacleDistancer = 0;

int MAX_DISTANCE = 200;
int ROUNDTRIP_CM = 57;
int LEFT_TURN_SPEED = 200;
int RIGHT_TURN_SPEED = 200;
int DRIVE_SPEED = 170;
int STOP_SPEED = 100;
NewPing sonar(trigpin, echopin, MAX_DISTANCE);

int distance = 100;
int speedSet = 0;
float angle = 0;
float x = 0;
float y = 0;
float pi = 3.14;

int turnDirection = 0;

MotorDriver m;
float on = false;

// ----------------------------------------------------------------------------------------------------------------------------------------

int channelID = 1578993;

const char* readAPI = "DF2WU62YYVWUCBGI";

const char * myWriteAPIKey = "A00BMSTZAM6Q3T2W";

String myStatus = "";

char ssid[] = "aneeshs-laptop";   // your network SSID (name)
char pass[] = "dkjgfhjnk";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

//char cse_ip[] = "192.168.137.11";
//int cse_port = 8080;

// clg om2m values
char cse_ip[] = "esw-onem2m.iiit.ac.in";
int cse_port = 443;
//int cse_port = 8080;


WiFiClient wifi;
String cse_server = "http://" + String(cse_ip) + ":" + String(cse_port) + "/~/in-cse/in-name/";
HttpClient Hclient = HttpClient(wifi, cse_ip, cse_port);
WiFiClient client = server.available();

int ledPin = 2;

// ----------------------------------------------------------------------------------------------------------------------------------------

// update variables

char jsonBuffer[3000] = "[";
unsigned long lastConnectionTime = 0; // Track the last connection time
unsigned long lastUpdateTime = 0; // Track the last update time
const unsigned long postingInterval = 20L * 1000L; // Post data every 30 seconds
const unsigned long updateInterval = 5L * 1000L; // Update once every 1 second

const char* thingspeak_server = "http://api.thingspeak.com";
int tgsp_port = 443;

HttpClient tgsp_http(wifi, thingspeak_server, tgsp_port);

AESLib aesLib;

String plaintext = "HELLO WORLD!";

char cleartext[256];


// AES Encryption Key
byte aes_key[] = { 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30 };

byte aes_iv[N_BLOCK] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x4a, 0x05, 0x40, 0x00 };

unsigned long encryptionLoopcount = 0;
byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// ----------------------------------------------------------------------------------------------------------------------------------------


void setup() {
  Serial.begin(115200);

  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  myservo.attach(10);
  myservo.write(90);
  delay(2000);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);

  //  while (!Serial);

  //  enable_WiFi();
    connect_WiFi();
  //
  ////  server.begin();
  //  printWifiStatus();
  //
    ThingSpeak.begin(wifi);

    
  aes_init();
  aesLib.set_paddingmode(paddingMode::CMS);
  char b64in[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char b64out[base64_enc_len(sizeof(aes_iv))];
  base64_encode(b64out, b64in, 16);
  char b64enc[base64_enc_len(10)];
  base64_encode(b64enc, (char*) "0123456789", 10);
  char b64dec[ base64_dec_len(b64enc, sizeof(b64enc))];
}

void loop() {
  //  Serial.print("On off status: ");
  //  float onOff = readThingSpeak();
  //  Serial.println(onOff);

  //  if(readThingSpeak()){
  advObstacleDetection();
  //  }/

  // drive
  //    if(loopCount < 20){
  //
  //
  //      loopCount++;
  //    }
  //    else {
  //      Serial.println("Trying ThingSpeak");
  //      pushToThingSpeak();
  //
  //      Serial.println("Trying OM2M");
  //      createCI(String(BotObstacleDistance));
  //
  //      loopCount = 0;
  //    }

  //  Serial.println("in loop");
  //  unsigned long t = millis();
  //  String val = String(x) + " " + String(y) + " " + String(t);
  //  createCI(val);
}

// ------------------------------------------------------------------------------------------------------------------

int lookRight()
{
  myservo.write(150);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(90);
  return distance;
}

int lookLeft()
{
  myservo.write(30);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(90);
  return distance;
  delay(100);
}

int readPing() {
  delay(70);
  int cm = int(sonar.ping_median() / ROUNDTRIP_CM);
  if (cm == 0)
  {
    cm = 250;
  }
  Serial.println(cm);
  return cm;
}

void STOP()
{
  m.motor(3, RELEASE, STOP_SPEED);
  m.motor(4, RELEASE, STOP_SPEED);

  Serial.println("Stopped");
}

void BACK()
{
  m.motor(3, BACKWARD, DRIVE_SPEED);
  m.motor(4, BACKWARD, DRIVE_SPEED);
}

void LEFT()
{
  m.motor(3, BACKWARD, RIGHT_TURN_SPEED);
  m.motor(4, FORWARD, LEFT_TURN_SPEED);
}

void RIGHT()
{
  m.motor(4, BACKWARD, LEFT_TURN_SPEED);
  m.motor(3, FORWARD, RIGHT_TURN_SPEED);
}

void AHEAD()
{
  m.motor(3, FORWARD, DRIVE_SPEED);
  m.motor(4, FORWARD, DRIVE_SPEED);

  Serial.println("forward");
}

void servoObstacleDetection() {
  int distanceR = 31;
  int distanceL = 32;
  //  delay(40);
  if (distance <= 30)
  {
    STOP();
    delay(100);
    BACK();
    delay(400);
    STOP();
    delay(100);
    unsigned long t = millis();
    String val = String(x) + " " + String(y) + " " + String(t);
    //        createCI(val);
    distanceR = lookRight();
    delay(100);
    distanceL = lookLeft();
    delay(100);
    if (distanceR >= distanceL)
    {
      RIGHT();
      delay(500);
      STOP();

      turnDirection = 2;
    }
    else
    {
      LEFT();
      delay(500);
      STOP();

      turnDirection = 1;
    }
  }
  else if (distanceR < 30 && distanceL < 30)
  {
    BACK();
    delay(400);
    STOP();

    turnDirection = -1;
  }
  else
  {
    AHEAD();

    turnDirection = 0;
  }
  distance = readPing();
  BotObstacleDistance = distance;

}


float readThingSpeak() {
  float val = ThingSpeak.readFloatField(1856792, 1, readAPI);
  Serial.println(ThingSpeak.getLastReadStatus());
  return val;
}

// ------------------------------------------------------------------------------------------------------------------------------------

// updates the json with the sensor reading
void updatesJson(char* jsonBuffer){
  static int flag = 0;
  const int points = 6;
 
  strcat(jsonBuffer,"{\"delta_t\":");
  unsigned long deltaT = (millis() - lastUpdateTime)/1000;
  
  size_t lengthT = String(deltaT).length();
  char temp[4];
  String(deltaT).toCharArray(temp,lengthT+1);
  strcat(jsonBuffer,temp);
  strcat(jsonBuffer,",");

  int k = BotObstacleDistance;
  int j = turnDirection;

  Serial.print("Sensor reading: ");
  Serial.println(k);

  // set field
  strcat(jsonBuffer, "\"field1\":");
  lengthT = String(k).length();
  String(k).toCharArray(temp,lengthT+1);
  strcat(jsonBuffer,temp);

  strcat(jsonBuffer, ",\"field2\":");
  lengthT = String(j).length();
  String(j).toCharArray(temp,lengthT+1);
  strcat(jsonBuffer,temp);
  strcat(jsonBuffer,"},");

  
  // If posting interval time has reached 15 seconds, update the ThingSpeak channel with your data
  if (millis() - lastConnectionTime >=  postingInterval) {
      size_t len = strlen(jsonBuffer);
      jsonBuffer[len-1] = ']';
      thingspeakRequest(jsonBuffer);
  }
  lastUpdateTime = millis(); // Update the last update time
}

void thingspeakRequest(char* jsonBuffer){
  char data[254] = "{\"write_api_key\":\"A00BMSTZAM6Q3T2W\",\"updates\":"; // Replace YOUR-CHANNEL-WRITEAPIKEY with your ThingSpeak channel write API key
  strcat(data,jsonBuffer);  // contains update array
  strcat(data,"}");

  String data_length = String(strlen(data)+1); //Compute the data buffer length
  
  // POST data to ThingSpeak
  Serial.println(data);
  String kk = String(data);
  kk.trim();

  sprintf(cleartext, "%s", kk.c_str()); // must not exceed 255 bytes; may contain a newline

  // encrypt json

  Serial.print("Test: ");
  Serial.println(String(cleartext));
  uint16_t clen = String(cleartext).length();
  Serial.println("Length: "+String(clen));
  String encrypted = encrypt(cleartext, clen+1, enc_iv);
  Serial.println(encrypted);
  char ciphertext[512];
  sprintf(ciphertext, "%s", encrypted.c_str()+'\0');
  Serial.print("Ciphertext: ");
  Serial.println(String(encrypted));

  delay(300);
    uint16_t dlen = encrypted.length();
    Serial.println("Length: "+String(clen));
    String decrypted = decrypt( ciphertext, dlen, dec_iv);
    Serial.print("Cleartext: ");
    Serial.println(decrypted);
    if (decrypted.equals(cleartext)) {
      Serial.println("SUCCES");
    }
    else
    {
      Serial.println("FAILURE");

    }
    for (int i = 0; i < 16; i++) {
      enc_iv[i] = 0;
      dec_iv[i] = 0;
    }
  

  if (wifi.connect("api.thingspeak.com", 80)) {
    wifi.println("POST /channels/1578993/bulk_update.json HTTP/1.1"); // Replace YOUR-CHANNEL-ID with your ThingSpeak channel ID
    wifi.println("Host: api.thingspeak.com");
    wifi.println("User-Agent: mw.doc.bulk-update (Arduino ESP8266)");
    wifi.println("Connection: close");
    wifi.println("Content-Type: application/json");
    wifi.println("Content-Length: "+data_length);
    wifi.println();
    wifi.println(data);

    Serial.println("Successfully pushed to ThingSpeak");
  }
  else {
    Serial.println("Failure: Failed to connect to ThingSpeak");
  }

  delay(250); //Wait to receive the response
  wifi.parseFloat();
  String resp = String(wifi.parseInt());
  Serial.println("Response code:"+resp); // Print the response code. 202 indicates that the server has accepted the response

//  int status = tgsp_http.responseStatusCode();
//  String responseBody = tgsp_http.responseBody();
//  Serial.println(status);
//  Serial.println(responseBody);

  jsonBuffer[0] = '['; //Reinitialize the jsonBuffer for next batch of data
  jsonBuffer[1] = '\0';
  lastConnectionTime = millis(); //Update the last conenction time
}

// ------------------------------------------------------------------------------------------------------------------------------------

void pushToThingSpeak() {
  int number1 = BotObstacleDistance;
  int number2 = BotObstacleDistancel;
  int number3 = BotObstacleDistancer;

  // set the fields with the values
  ThingSpeak.setField(1, 20);
  //  ThingSpeak.setField(2, number2);
  //  ThingSpeak.setField(3, number3);

  // figure out the status message
  //  if(number1 > threshold){
  //    myStatus = String("No obstacle");
  //  }
  //  else{
  //    myStatus = String("Obstacle detected");
  //  }
  //
  //  // set the status
  //  ThingSpeak.setStatus(myStatus);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(channelID, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Thingspeak update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

void createCI(String val) {

  String body = "{\"m2m:cin\": {\"lbl\": [ \"Team-12\" ],\"con\": \"" + String(val) + "\"}}";
  Hclient.beginRequest();
  //  Hclient.post("/~/in-cse/cnt-313720922/");

  // clg om2m
  Hclient.post("/~/in-cse/in-name/Team-12/Node-1/Data/Ultrasound_sensor_center/");
  Hclient.sendHeader("Content-Length", body.length());

  // my om2m
  //  Hclient.sendHeader("X-M2M-Origin", "admin:admin");

  // clg m2m
  Hclient.sendHeader("X-M2M-Origin", "B2uLYd:XzfQQB");

  Hclient.sendHeader("Content-Type", "application/json;ty=4");
  Hclient.sendHeader("Connection", "keep-alive");
  Hclient.beginBody();
  //  String body = "m2m:cin: {lbl: [ \"Team-13\" ],con: \"" + String(val)+ "\"}";
  Serial.println(body);
  Hclient.print(body);
  Hclient.endRequest();
  int status = Hclient.responseStatusCode();
  String responseBody = Hclient.responseBody();
  Serial.println(status);
  Serial.println(responseBody);
}

// ------------------------------------------------------------------------------------------------------------------------------------

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(3000);
  }
}

void advObstacleDetection() {

  int isOn = 1;
  int loopCount = 0;
  //  Serial.println("isOn: " + String(isOn));/
  while (isOn) {
    //    Serial.print("On off status: ");
    //        onOff = readThingSpeak();
    //        Serial.println(onOff);

    //        if(onOff > 0)

    //    isOn = readThingSpeak();/

    if (loopCount >= 90) {
//      STOP();
//            Serial.println("Trying ThingSpeak");
//            pushToThingSpeak();
      //      Serial.println("Trying OM2M");
      //      createCI(String(BotObstacleDistance));
      loopCount = 0;

    }
    else {
      servoObstacleDetection();
      Serial.println("Incrementing loop");

      if (millis() - lastUpdateTime >= updateInterval){
        updatesJson(jsonBuffer);
      }
      loopCount++;
    }
  }

  STOP();
}

// ------------------------------------------------------------------------------------------------------------------------------------

// Generate IV (once)
void aes_init() {
  Serial.println("gen_iv()");
  aesLib.gen_iv(aes_iv);
  //Serial.println("encrypt()");
  //Serial.println(encrypt(strdup(plaintext.c_str()), plaintext.length(), aes_iv));
}

String encrypt(char * msg, uint16_t msgLen, byte iv[]) {
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  char encrypted[cipherlength]; // AHA! needs to be large, 2x is not enough
  aesLib.encrypt64(msg, msgLen, encrypted, aes_key, sizeof(aes_key), iv);
  Serial.print("encrypted = "); Serial.println(encrypted);
  return String(encrypted);
}

String decrypt(char * msg, uint16_t msgLen, byte iv[]) {
  char decrypted[msgLen];
  aesLib.decrypt64(msg, msgLen, decrypted, aes_key, sizeof(aes_key), iv);
  return String(decrypted);
}