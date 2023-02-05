#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_CCS811.h>

Adafruit_CCS811 ccs;
Adafruit_CCS811 ccs2;

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1  // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  Serial.println("CCS811 test");

  if(!ccs2.begin(0x5A)){
Serial.println("Failed to start sensor! Please check your wiring.");
}

  if(!ccs.begin(0x5B)){
Serial.println("Failed to start sensor! Please check your wiring.");
}

  ccs2.begin(0x5A);
  ccs.begin(0x5B);
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
  delay(2000);
  display.clearDisplay();


  display.setTextSize(1.5);
  display.setCursor(0, 28);
  display.println("CO2 Sequestration \nMonitor");
  display.display();
  delay(2000);
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
  display.setCursor(0, 28);  
if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2 on S1: ");
      Serial.println(ccs.geteCO2());
      display.print(ccs.geteCO2());
      
    }}
    else{
      Serial.println("ERROR 1 !");
      display.print("Err1");
    }


    if(ccs2.available()){
    if(!ccs2.readData()){
      Serial.print("CO2 on S2: ");
      Serial.println(ccs2.geteCO2());
      display.setCursor(76, 28);
      display.print(ccs2.geteCO2());
    }}
    else{
      Serial.println("ERROR 2 !");
      display.print("Err2");
    }      
  display.display();
  delay(2000);
  display.clearDisplay();
}