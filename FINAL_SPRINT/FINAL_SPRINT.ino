#include <NewPing.h>
#include <Servo.h>
#include <MotorDriver.h>
#include <WiFiNINA.h>
#include <SPI.h>
//#include <AESLib.h>
#include <ArduinoHttpClient.h>
#include <ThingSpeak.h>

#include <MotorDriver.h>

Servo myservo;

int trigpin = A5;
int echopin = A0;
int BotObstacleDistance = 100;
int BotObstacleDistancel = 0;
int BotObstacleDistancer = 0;

int MAX_DISTANCE = 200;
int ROUNDTRIP_CM = 57;
NewPing sonar(trigpin, echopin, MAX_DISTANCE);


int distance = 100;
int speedSet = 0;
float angle = 0;
float x = 0;
float y = 0;
float pi = 3.14;


MotorDriver m;
float on = false;

int channelID = 1578993;

const char* readAPI = "DF2WU62YYVWUCBGI";

const char * myWriteAPIKey = "A00BMSTZAM6Q3T2W";

String myStatus = "";

char ssid[] = "Garam Sthaan";   // your network SSID (name)
char pass[] = "69696969";   // your network password
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

  enable_WiFi();
  connect_WiFi();
  //
  ////  server.begin();
  //  printWifiStatus();
  //
  ThingSpeak.begin(wifi);
}

void loop() {
  //  Serial.print("On off status: ");
  //  float onOff = readThingSpeak();
  //  Serial.println(onOff);

  advObstacleDetection();

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
  m.motor(3, RELEASE, 120);
  m.motor(4, RELEASE, 120);

  Serial.println("Stopped");
}

void BACK()
{
  m.motor(3, BACKWARD, 255);
  m.motor(4, BACKWARD, 255);
}

void LEFT()
{
  m.motor(3, BACKWARD, 255);
  m.motor(4, FORWARD, 255);
}

void RIGHT()
{
  m.motor(4, BACKWARD, 255);
  m.motor(3, FORWARD, 255);
}

void AHEAD()
{
  m.motor(3, FORWARD, 255);
  m.motor(4, FORWARD, 255);

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
    }
    else
    {
      LEFT();
      delay(500);
      STOP();
    }
  }
  else if (distanceR < 30 && distanceL < 30)
  {
    BACK();
    delay(400);
    STOP();
  }
  else
  {
    AHEAD();
  }
  distance = readPing();
}


float readThingSpeak() {
  float val = ThingSpeak.readFloatField(1856792, 1, readAPI);
  Serial.println(ThingSpeak.getLastReadStatus());
  return val;
}

void pushToThingSpeak() {
  int number1 = BotObstacleDistance;
  int number2 = BotObstacleDistancel;
  int number3 = BotObstacleDistancer;

  // set the fields with the values
  ThingSpeak.setField(1, number1);
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

  int loopCount = 0;
  while (1) {
    Serial.print("On off status: ");
    //        onOff = readThingSpeak();
    //        Serial.println(onOff);

    //        if(onOff > 0){
    servoObstacleDetection();
    if (loopCount >= 90) {
      STOP();
      Serial.println("Trying ThingSpeak");
      pushToThingSpeak();
      Serial.println("Trying OM2M");
      createCI(String(BotObstacleDistance));
      loopCount = 0;
    }
    else {
      Serial.println("Incrementing loop");
      loopCount++;
    }

    //  }
  }
}
