/*

Read voltage & ampere by fan motor 

1. Wire connection between OLED & Mega 2560
OLED Display (I2C) – SSD1306
OLED Pin	Arduino Pin
VCC	5V
GND	GND
SDA	A4 (UNO) / 20 (Mega)
SCL	A5 (UNO) / 21 (Mega)

2. ACS712 Current Sensor & Mega 2560
ACS712 Pin	Arduino Pin
VCC	5V
GND	GND
OUT	A1

3. OLED and Mega 2560
 Wiring: I2C OLED
OLED Pin	Arduino Uno / Mega
VCC	5V
GND	GND
SDA	A4 (Uno) / 20 (Mega)
SCL	A5 (Uno) / 21 (Mega)

The wire connection can be flxeible UNO/nano/Mega 2560

*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup (128x64 I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Analog pins
const int voltPin = A0;    
const int currentPin = A1; 
const int motorRotationPin = A2;  // Analog input for motor rotation sensor

// ACS712 (5A) parameters
const float VCC = 5.0;
const float ZERO_CURRENT_VOLTAGE = 2.5;
const float SENSITIVITY = 0.185;

// Max measurable values for scaling bars
const float MAX_VOLTAGE = 5.0;  // Max voltage expected for bar scale
const float MAX_CURRENT = 5.0;  // Max current expected for bar scale
const float MAX_ROTATION = 1023.0; // Max analog reading for motor rotation

// Bar dimensions
const int BAR_WIDTH = 80;
const int BAR_HEIGHT = 8;
const int VERT_BAR_HEIGHT = 40;

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Initializing");
  display.setCursor(0, 35);
  display.println("Sensors...");
  display.display();

  delay(2000);
  display.clearDisplay();
}

void loop() {
  int rawVoltage = analogRead(voltPin);
  int rawCurrent = analogRead(currentPin);
  int rawRotation = analogRead(motorRotationPin);  // motor rotation reading

  float voltage = (rawVoltage * VCC) / 1023.0;
  float currentVoltage = (rawCurrent * VCC) / 1023.0;
  float current = (currentVoltage - ZERO_CURRENT_VOLTAGE) / SENSITIVITY;

  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V | Current: ");
  Serial.print(current, 2);
  Serial.print(" A | Motor Rotation (raw): ");
  Serial.println(rawRotation);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (voltage > 0) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Volt: ");
    display.print(voltage, 2);
    display.println(" V");

    // Draw voltage bar (width max BAR_WIDTH px, height BAR_HEIGHT px)
    int voltBarWidth = (int)((voltage / MAX_VOLTAGE) * BAR_WIDTH);
    display.fillRect(0, 12, voltBarWidth, BAR_HEIGHT, SSD1306_WHITE);
    display.drawRect(0, 12, BAR_WIDTH, BAR_HEIGHT, SSD1306_WHITE);  // Bar outline

    display.setCursor(0, 25);
    display.print("Curr: ");
    display.print(current, 2);
    display.println(" A");

    // Draw current bar (width max BAR_WIDTH px, height BAR_HEIGHT px)
    int currBarWidth = (int)((current / MAX_CURRENT) * BAR_WIDTH);
    if(currBarWidth < 0) currBarWidth = 0;
    if(currBarWidth > BAR_WIDTH) currBarWidth = BAR_WIDTH;
    display.fillRect(0, 37, currBarWidth, BAR_HEIGHT, SSD1306_WHITE);
    display.drawRect(0, 37, BAR_WIDTH, BAR_HEIGHT, SSD1306_WHITE);  // Bar outline

    // Motor rotation vertical bar on right side (rightmost 15 px)
    int rotationBarHeight = (int)((rawRotation / MAX_ROTATION) * VERT_BAR_HEIGHT);
    if(rotationBarHeight > VERT_BAR_HEIGHT) rotationBarHeight = VERT_BAR_HEIGHT;

    int xPos = SCREEN_WIDTH - 15;  // 15 pixels from right edge
    int yBase = 63;  // bottom of the display

    // Draw outline for vertical bar
    display.drawRect(xPos, yBase - VERT_BAR_HEIGHT, 10, VERT_BAR_HEIGHT, SSD1306_WHITE);

    // Draw filled part of vertical bar (from bottom up)
    display.fillRect(xPos + 1, yBase - rotationBarHeight, 8, rotationBarHeight, SSD1306_WHITE);

    // Label for motor rotation
    display.setCursor(xPos - 10, yBase - VERT_BAR_HEIGHT - 10);
    display.setTextSize(1);
    display.print("Motor");
  } else {
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("No voltage detected");
  }

  display.display();
  delay(100);
} 