#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_CCS811.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>


Adafruit_CCS811 ccs;
Adafruit_CCS811 ccs2;

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1  // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;


// WiFi credentials
const char* ssid = "PTCL-BB";         // change SSID
const char* password = "bluewhale";    // change password


// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbzzwd7MMQ0GEQtSRoJiAH0Sk4ocJ5QHArJtGhb1dB44JPIJA8cZq8kJBIHXeDWAZbJ4";    // change Gscript ID
int count = 0;


void setup() {
  delay(1000);
  Serial.begin(9600);
  //delay(1000);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  delay(500);
  Serial.println("CCS811 test");


//check if sensors are turned on or not
//sensor 2 = 0x5B
//sensor 1 = 0x5A


  if(!ccs.begin(0x5A)){
Serial.println("Failed to start sensor 1! Please check your wiring.");
}
  if(!ccs2.begin(0x5B)){
Serial.println("Failed to start sensor 2! Please check your wiring.");
}

delay(100);
//initialize sensors:
  ccs.begin(0x5A);
  ccs2.begin(0x5B);

  //calibrate temperature sensor
  // initialize the OLED object
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }



  // Clear the buffer.
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 24);
  display.setTextSize(2);
  display.println("C - CARGO");
  display.display();
  delay(500);
  display.clearDisplay();


  display.setTextSize(1.5);
  display.setCursor(0, 28);
  display.println("CO2 Sequestration \nMonitor");
  display.display();
  delay(500);
  display.clearDisplay();

  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("In:");
  display.setCursor(76, 0);
  display.setTextSize(2);
  display.println("Out:");
  display.setCursor(28, 46);
  display.setTextSize(1);
  display.println("ppm");
  display.setCursor(102, 46);
  display.println("ppm");
  display.setCursor(0, 8);
  display.setTextSize(1);

  display.setTextSize(2);
  display.setCursor(0, 28);
  display.print(85);

  display.setCursor(76, 28);
  display.setTextSize(2);
  display.print(85);

  display.display();
}

void upd() {
  delay(500);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("In:");
  display.setCursor(76, 0);
  display.setTextSize(2);
  display.println("Out:");
  display.setCursor(28, 46);
  display.setTextSize(1);
  display.println("ppm");
  display.setCursor(102, 46);
  display.println("ppm");
  display.setCursor(0, 8);
  display.setTextSize(2);
}

void loop() {
  upd();
  int sensor_in = ccs.geteCO2();
  int sensor_out = ccs2.geteCO2();
  int sensor_in_tvoc = ccs.getTVOC();
  int sensor_out_tcov = ccs2.getTVOC();

  push (sensor_in, sensor_out, sensor_in_tvoc, sensor_out_tcov);
  display.setCursor(0, 28);

if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2 on S1: ");
      Serial.println(sensor_in);
      display.print(sensor_in);
      
    }}
    else{
      Serial.println("ERROR 1 !");
      display.print("Err1");
      ccs.begin(0x5A);
    }


if(ccs2.available()){
    if(!ccs2.readData()){
      Serial.print("CO2 on S2: ");
      Serial.println(sensor_out);
      display.setCursor(76, 28);
      display.print(sensor_out);
    }}
    else{
      Serial.println("ERROR 2 !");
      display.setCursor(76, 28);
      display.print("Err2");
      ccs2.begin(0x5B);
    }
  display.display();
  delay(500);
  display.clearDisplay();
  delay(100);
}


void push (int sen_in, int sen_out, int in_tvoc, int out_tvoc)
{
   if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50]; //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String asString(timeStringBuff);
    asString.replace(" ", "-");
    Serial.print("Time:");
    Serial.println(asString);
    // String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"sensor_1=" + sen_in + "&sensor=" + sen_out;
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?" + "in_co2=" + sen_in + "&in_tvoc=" + in_tvoc + "&out_co2=" + sen_out + "&out_tvoc=" + out_tvoc;
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    delay(100);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    delay(20);
    int httpCode = http.GET(); 
    delay(20);
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
    delay(20);
  }
  count++;
  delay(700);
} 
