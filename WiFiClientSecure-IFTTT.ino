#include "config.h" // Wifi and IFTTT
#include "certificate.h" // IFTTT
#include <WiFi.h>
#include <WiFiClientSecure.h>

const int inputPir = 5; // IO5
const int outputLed = 2; // IO2
bool outputState = true;
unsigned long t = 0; // Time

const char* server = "maker.ifttt.com";  // Server URL

WiFiClientSecure client;

void errorBlink() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(outputLed, HIGH);
    delay(300);
    digitalWrite(outputLed, LOW);
    delay(300);
  }
}

void wifiBegin() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
}

bool checkWifiConnected() {
  // attempt to connect to Wifi network:
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);

    count++;
    if (count > 15) { // about 15s
      Serial.println("(wifiConnect) failed!");
      errorBlink();
      return false;
    }
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  return true;
}

void send() {
  while (!checkWifiConnected()) {
    wifiBegin();
  }

  client.setCACert(test_ca_cert);

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443)) {
    Serial.println("Connection failed!");
  } else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    String url = "/trigger/" + makerEvent + "/with/key/" + makerKey;
    // url += "?value1=VALUE1";
    client.println("GET " + url + " HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

    Serial.print("Waiting for response "); //WiFiClientSecure uses a non blocking implementation

    int count = 0;
    while (!client.available()) {
      delay(50); //
      Serial.print(".");

      count++;
      if (count > 20 * 20) { // about 20s
        Serial.println("(send) failed!");
        errorBlink();
        return;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting from server.");
      client.stop();
    }
  }
}

void setup() {
  pinMode(outputLed, OUTPUT);
  pinMode(inputPir, INPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);

  wifiBegin();
  while (!checkWifiConnected()) {
    wifiBegin();
  }
}

void loop() {
  if (digitalRead(inputPir) == HIGH) {
    if (!outputState && millis() - t >= 10000) {
      digitalWrite(outputLed, HIGH);
      outputState = true;
      send();
      t = millis();
    }
  } else {
    digitalWrite(outputLed, LOW);
    outputState = false;
  }
}

