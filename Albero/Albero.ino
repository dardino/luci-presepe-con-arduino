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
   
   // URL per recuperare l'ora:
   http://iot-gb.azurewebsites.net/timeinfo/city/rome
*/

/* Create a WiFi access point and provide a web server on it. */

#include <elapsedMillis.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h> //Include File System Headers

const char *imagefile = "/albero.jpg";
const char *htmlfile = "/index.html";

/* Set these to your desired credentials. */
const char *ssid = "xxx";
const char *password = "xxx";
String tree_state = "off";
ESP8266WebServer server(80);

void handleWebRequests()
{
  if (loadFromSpiffs(server.uri()))
    return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}
bool loadFromSpiffs(String path)
{
  String dataType = "text/plain";
  if (path.endsWith("/"))
    path += "index.htm";
  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html"))
    dataType = "text/html";
  else if (path.endsWith(".htm"))
    dataType = "text/html";
  else if (path.endsWith(".css"))
    dataType = "text/css";
  else if (path.endsWith(".js"))
    dataType = "application/javascript";
  else if (path.endsWith(".png"))
    dataType = "image/png";
  else if (path.endsWith(".gif"))
    dataType = "image/gif";
  else if (path.endsWith(".jpg"))
    dataType = "image/jpeg";
  else if (path.endsWith(".ico"))
    dataType = "image/x-icon";
  else if (path.endsWith(".xml"))
    dataType = "text/xml";
  else if (path.endsWith(".pdf"))
    dataType = "application/pdf";
  else if (path.endsWith(".zip"))
    dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download"))
    dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
  }

  dataFile.close();
  return true;
}
void handleRoot()
{
  server.sendHeader("Location", "/index.html?state=" + tree_state, true); //Redirect to our html web page
  server.send(302, "text/plane", "");

  //char body[] =
  //    "<html>"
  //    "<head>"
  //    "  <title>Albero di natale</title>"
  //    "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />"
  //    "  <style>"
  //    "    body {"
  //    "      display: flex;"
  //    "      flex-direction: column;"
  //    "      align-items: center;"
  //    "    }"
  //    "    a {"
  //    "      border: solid 1px #791616;"
  //    "      display: block;"
  //    "      padding: 1em;"
  //    "      text-transform: uppercase;"
  //    "      background-color: gray;"
  //    "      color: white;"
  //    "      margin: 0.3em;"
  //    "      min-width: 10em;"
  //    "      text-align: center;"
  //    "      text-decoration: none;"
  //    "      border-radius: 8px;"
  //    "      box-shadow: 0 0 1.1em -0.3em black;"
  //    "    }"
  //    "  </style>"
  //    "</head>"
  //    "<body>"
  //    "<h1>Buon natale!</h1>"
  //    "<a href=\"/on\">Accendi l'albero</a>"
  //    "<a href=\"/off\">Spegni l'albero</a>"
  //    "</body>"
  //    "</html>";
  //server.send(200, "text/html", body);
}

void handleOn()
{
  digitalWrite(D0, HIGH);
  digitalWrite(1, HIGH);
  tree_state = "on";
  handleRoot();
}
void handleOff()
{
  digitalWrite(D0, LOW);
  digitalWrite(1, LOW);
  tree_state = "off";
  handleRoot();
}

void setup()
{
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
  startFS();

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.onNotFound(handleWebRequests);
  server.begin();
  Serial.println("HTTP server started");
}

elapsedMillis elapsed = 0;
long interval = 60000;
void loop()
{
  server.handleClient();

  if (elapsed >= interval)
  {
    elapsed = 0;
  }

  if (elapsed == 0)
  {
    getTimeFromNet();
  }
}

void getTimeFromNet()
{
  //   HttpClient client;
  //   client.get("http://iot-gb.azurewebsites.net/timeinfo/city/rome");
  //   // if there are incoming bytes available
  //   // from the server, read them and print them:
  //   while (client.available())
  //   {
  //     char c = client.read();
  //     Serial.print(c);
  //   }
}

void connectWiFi()
{
  // Connect to WiFi
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
}
void startFS()
{
  SPIFFS.begin();
  Serial.println("File System Initialized");
}
void startmDNS()
{
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("albero"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
}
