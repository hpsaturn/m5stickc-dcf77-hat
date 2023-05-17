#include <Arduino.h>
#include <M5StickCPlus.h>
#include <DCF77.h>
#include <RTClib.h>

//#define BLINK_PIN 19        // Led pin
#define DCF77_PON_PIN 0    // DCF77 operation pin
//#define DCF77_OUT_PIN 27    // DCF77 output pin


#define DCF_PIN 26	       // Connection pin to DCF 77 device
#define DCF_INTERRUPT 26    // Interrupt number associated with pin

DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT, true);

#define BRIGHT_SIZE 5

int backlight[BRIGHT_SIZE] = {5, 15, 30, 50, 70};
int brightness = 2;  // MAX: BRIGHT_SIZE

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Axp.ScreenBreath(backlight[brightness]);

  delay(200);
  Serial.flush();
  // Configure DCF77
  // (LOW = Normal operation, HIGH = standby)
  pinMode(DCF77_PON_PIN, OUTPUT);
  digitalWrite(DCF77_PON_PIN, LOW);

  DCF.Start();
  Serial.println("Waiting for DCF77 time ... ");
  Serial.println("It will take at least 2 minutes until a first update can be processed.");
}

void dfcLoop() {
  static uint_fast64_t dts = 0;  // timestamp for DCF77 refresh
  if (millis() - dts > 1000) {
    dts = millis();
    time_t DCFtime = DCF.getTime();  // Check if new DCF77 time is available
    if (DCFtime != 0) {
      Serial.println("Time is updated");
      setTime(DCFtime);
    }
  }
}

void buttonLoop(){
  M5.update();  
  if (M5.BtnA.wasReleased()) {  // If the button A is pressed.
    if (brightness++ >= BRIGHT_SIZE-1) brightness = 0;
    M5.Axp.ScreenBreath(backlight[brightness]);
  }
}

void displayLoop() {
  static uint_fast64_t tts = 0;  // timestamp for GUI refresh
  if (millis() - tts > 1000) {
    tts = millis();
    // digital clock display of the time
    // Serial.print(hour());
    // printDigits(minute());
    // printDigits(second());
    // Serial.print(" ");
    // Serial.print(day());
    // Serial.print(" ");
    // Serial.print(month());
    // Serial.print(" ");
    // Serial.print(year());
    // Serial.println();
    M5.Lcd.setCursor(50, 80);
    M5.Lcd.fillRect(50, 80, 60, 25, BLACK);
    M5.Lcd.print(year());
    M5.Lcd.setCursor(20, 105);
    M5.Lcd.fillRect(20, 105, M5.Lcd.width() - 20, 25, BLACK);
    M5.Lcd.printf("%02d:%02d:%02d", hour(), minute(), second());
  }
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10) Serial.print('0');
  Serial.print(digits);
}

void loop() {
  buttonLoop();
  dfcLoop();
  displayLoop();
}


