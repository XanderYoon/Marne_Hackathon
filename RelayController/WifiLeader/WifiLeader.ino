#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "NodeMCU"
#define APPSK  "12345678"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

const int relay1 = 0; //D3
const int relay2 = 2; //D4
const int relay3 = 14; //D5
const int relay4 = 12; //D6

int relayPins[] = {0, 2, 14, 12};
ESP8266WebServer server(80);

bool node1Status = false;

/* http://192.168.4.1 in a web browser */

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  Serial.println("Configuring access point...");
  Serial.println(WiFi.softAP(ssid, password) ? "Success!" : "Failure!");

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handle_OnConnect);
  server.on("/node1on", handle_node1on);
  server.on("/node1off", handle_node1off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  server.send(200, "text/html", SendHTML(true)); 
}

void loop() {
  server.handleClient();
  printOut();
  freqHop();
}

void freqHop() {
  for (int i = 0; i < random(10,16)*1000; i++) {
    int pinNum = random(0,4);
    digitalWrite(relayPins[pinNum], HIGH);
    delay(1);
    digitalWrite(relayPins[pinNum], LOW);
  }
}

void printOut() {
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  Serial.println(node1Status);
  delay(1000);
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(true)); 
}

void handle_node1on() {
  server.send(200, "text/html", SendHTML(true)); 
}

void handle_node1off() {
  server.send(200, "text/html", SendHTML(false));
  ESP.deepSleep(5e6); //sleeps for 5 seconds
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t node1Status){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Radio Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  ptr +="<h3>Server</h3>\n";
  
  if(node1Status)
  {ptr +="<p>Node1 Status: ON</p><a class=\"button button-off\" href=\"/node1off\">OFF</a>\n";}
  else
  {ptr +="<p>Node1 Status: OFF</p><a class=\"button button-on\" href=\"/node1on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
