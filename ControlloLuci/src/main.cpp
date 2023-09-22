#define ENABLE_GxEPD2_GFX 0

// https://github.com/ZinggJM/GxEPD2

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define EPD_MOSI                (23)
#define EPD_MISO                (-1) //elink no use
#define EPD_SCLK                (18)

#define EPD_BUSY                (4)
#define EPD_RSET                (16)
#define EPD_DC                  (17)
#define EPD_CS                  (5)

#define SDCARD_CS               (13)
#define SDCARD_MOSI             (15)
#define SDCARD_MISO             (2)
#define SDCARD_SCLK             (14)

#define BUTTON_1                (37)
#define BUTTON_2                (38)
#define BUTTON_3                (39)

#define SPK_POWER_EN            (19)

#define BUTTONS                 {37,38,39}
#define BUTTON_COUNT            (3)

#define LED_PIN                 (26)
#define LED_ON                  (LOW)

#define ADC_PIN                 (35)

#define SPERKER_PIN             (25)

#define LEDC_CHANNEL_0          (0)
#define _HAS_ADC_DETECTED_
#define _HAS_LED_
#define _HAS_SPEAKER_
#define _BUILTIN_DAC_
#define _HAS_SDCARD_
#define _HAS_PWR_CTRL_

// DEPG0213BN

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT>
display(GxEPD2_213_BN(/*CS=5*/ EPD_CS, /*DC=*/ EPD_DC, /*RST=*/ EPD_RSET, /*BUSY=*/ EPD_BUSY)); // GDEY029T94  128x296, SSD1680, (FPC-A005 20.06.15)

// START -------------- WIFI -------------- START 
#include <WiFi.h>
void testWiFi()
{    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();

    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("");
}
// END -------------- WIFI -------------- END


const char HelloWorld[] = "Hello World!";

void helloWorld()
{
    Serial.println("helloWorld");
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center bounding box by transposition of origin:
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(HelloWorld);
    } while (display.nextPage());
    Serial.println("helloWorld done");
}


void drawGraphics()
{
    display.setPartialWindow(0, 0, display.width(), display.height());
    display.setRotation(0);
    // display.firstPage();
    do {
      display.drawRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
      display.drawRect(2, 2, display.width() - 4, display.height() - 4, GxEPD_BLACK);
      display.drawRect(4, 4, display.width() - 8, display.height() - 8, GxEPD_BLACK);
      display.drawRect(6, 6, display.width() - 12, display.height() - 12, GxEPD_BLACK);
      display.drawRect(8, 8, display.width() - 16, display.height() - 16, GxEPD_BLACK);
    } while (display.nextPage());
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

    display.init(115200, true, 2, false); 

    testWiFi();

    helloWorld();
    delay(1000);
    drawGraphics();

    display.powerOff();
}

void loop() {
  // put your main code here, to run repeatedly:

}
