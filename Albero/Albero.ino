/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/* Set these to your desired credentials. */
const char *ssid = "xxx";
const char *password = "xxx";

ESP8266WebServer server(80);

void handleRoot() {
  char body[] = 
  "<html>"
    "<head>"
      "<title>Albero di natale</title>"
      "<style>"
        "body {"
          "display: flex;"
          "flex-direction: column;"
          "align-items: center;"
        "}"
        "a {"
          "border: solid 1px #791616;"
          "display: block;"
          "padding: 1em;"
          "text-transform: uppercase;"
          "background-color: gray;"
          "color: white;"
          "margin: 0.3em;"
          "min-width: 10em;"
          "text-align: center;"
          "text-decoration: none;"
          "border-radius: 8px;"
          "box-shadow: 0 0 1.1em -0.3em black;"
        "}"
      "</style>"
    "</head>"
    "<body>"
      "<h1>Buon natale!</h1>"
      "<a href=\"/on\">Accendi l'albero</a>"
      "<a href=\"/off\">Spegni l'albero</a>"
    "</body>"
  "</html>";
  server.send(200, "text/html", body);
}

void handleOn() {
  digitalWrite(D0, HIGH);
  digitalWrite(1, HIGH);
  handleRoot();
}
void handleOff() {
  digitalWrite(D0, LOW);
  digitalWrite(1, LOW);
  handleRoot();
}

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(1, OUTPUT);

  delay(1000);
  digitalWrite(D0, HIGH);
  digitalWrite(1, HIGH);
  delay(1000);
  digitalWrite(D0, LOW);
  digitalWrite(1, LOW);
  delay(1000);

  Serial.begin(9600);

  Serial.println();
  Serial.print("Connecting to access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  connectWiFi();
  IPAddress myIP = WiFi.localIP();
  Serial.print("IP address: ");
  Serial.println(myIP);

  startmDNS();

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void connectWiFi() {
  // Connect to WiFi
  WiFi.begin(ssid, password);

// while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
}

void startmDNS() {
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("albero")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");  
}