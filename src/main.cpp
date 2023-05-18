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
int brightness = 1;  // MAX: BRIGHT_SIZE

#define DATEPOSY 20
#define DATEPOSX 50
#define DATEHIGH 25
#define DATEMARG 20

#define SIGNPOSY 70
#define SIGNMARG 2
#define SIGNHIGH 30

int signalpos = SIGNMARG;
int bandposy = SIGNPOSY;

class mDCF77EventsCallback : public DCF77EventsCallback {
  void onSignal(unsigned char signal) {
    Serial.print(signal);
    if (signal == 0)
      M5.Lcd.fillRect(signalpos,bandposy,2,SIGNHIGH,TFT_DARKGREY);
    else
      M5.Lcd.fillRect(signalpos,bandposy,2,SIGNHIGH,TFT_GREEN);
    signalpos = signalpos + 2;
  };
  void onBufferMsg(const char* msg) {
    Serial.println(msg);
    signalpos = SIGNMARG;
    if (String(msg)=="EoM"){
      M5.Lcd.fillRect(SIGNMARG, bandposy, 121, SIGNHIGH, BLACK);
      bandposy = SIGNPOSY;
    }
    else {
      bandposy = bandposy + SIGNHIGH + 3;
    }
  };
  void onGetTime(){

  };
  void onParityError(){

  };
};

void displayLoop() {
  static uint_fast64_t tts = 0;  // timestamp for GUI refresh
  if (millis() - tts > 1000) {
    tts = millis(); 
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(DATEPOSX, DATEPOSY);
    M5.Lcd.fillRect(DATEPOSX, DATEPOSY, 60, DATEHIGH, BLACK);
    M5.Lcd.print(year());
    M5.Lcd.setCursor(DATEMARG, DATEPOSY+DATEHIGH);
    M5.Lcd.fillRect(DATEMARG, DATEPOSY+DATEHIGH, M5.Lcd.width() - DATEMARG, DATEHIGH, BLACK);
    M5.Lcd.printf("%02d:%02d:%02d", hour(), minute(), second());
  }
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Axp.ScreenBreath(backlight[brightness]);

  delay(200);
  Serial.flush();
  // Configure DCF77
  // (LOW = Normal operation, HIGH = standby)
  pinMode(DCF77_PON_PIN, OUTPUT);
  digitalWrite(DCF77_PON_PIN, LOW);

  DCF.setCallBack(new mDCF77EventsCallback());
  DCF.Start();
  Serial.println("Waiting for DCF77 time ... ");
  Serial.println("It will take at least 2 minutes until a first update can be processed.");
  delay(100);
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

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10) Serial.print('0');
  Serial.print(digits);
}

void printClock() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void loop() {
  dfcLoop();
  buttonLoop();
  displayLoop();
  delay(80);
}



