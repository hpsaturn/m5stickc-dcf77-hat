#include <Arduino.h>
#include <DCF77.h>
#include <RTClib.h>
#include "gui.h"

#define DCF77_PON_PIN 0     // DCF77 operation pin
#define DCF_PIN 26	        // Connection pin to DCF 77 device
#define DCF_INTERRUPT 26    // Interrupt number associated with pin

DCF77 DCF = DCF77(DCF_PIN, DCF_INTERRUPT, true);

#define BRIGHT_SIZE 5
int backlight[BRIGHT_SIZE] = {5, 15, 30, 50, 70};
int brightness = 1;        // MAX brightness is BRIGHT_SIZE

int signalpos = SIGNMARG;  // signal bar position
int bandposy = SIGNPOSY;   // signals band position

class mDCF77EventsCallback : public DCF77EventsCallback {

  void onSignal(unsigned char signal) {
    Serial.print(signal);
    if (signal == 0)
      M5.Lcd.fillRect(signalpos, bandposy, 2, SIGNHIGH, TFT_DARKGREY);
    else
      M5.Lcd.fillRect(signalpos, bandposy, 2, SIGNHIGH, TFT_GREEN);
    signalpos = signalpos + 2;
  };

  void onBufferMsg(const char* msg) {
    Serial.println(msg);
    signalpos = SIGNMARG;
    M5.Lcd.setTextSize(2);
    if (String(msg) == "EoM") {
      printBuffer(msg, TFT_RED);
      printParity("Ukn", TFT_CYAN);
      printStatus("Decoding..");
      bandposy = SIGNPOSY;
    } else if (String(msg) == "BF") {
      printBuffer(msg,TFT_WHITE);
      printParity("Ok", TFT_WHITE);
      bandposy = bandposy + SIGNHIGH + 3;
      if (bandposy >= (SIGNPOSY + (SIGNHIGH + 3) * MAXBANDS)) {
        bandposy = SIGNPOSY;
      }
    }
    M5.Lcd.fillRect(SIGNMARG, bandposy, 121, SIGNHIGH, BLACK);
  };

  void onTimeUpdateMsg(const char* msg){
    printStatus(msg);
  };

  void onParityError() {
    updateField(TFT_WIDTH - 60, INFOPOSY+DATEHIGH+7, 60, DATEHIGH, TFT_CYAN);
    M5.Lcd.printf("P:Err");
  };
};

void dcfLoop() {
  static uint_fast64_t tts = 0;  // timestamp for GUI refresh
  if (millis() - tts > 1000) {
    tts = millis();
    time_t DCFtime = DCF.getTime();  // Check if new DCF77 time is available
    if (DCFtime != 0) {
      Serial.printf("\rTime is updated: ");
      setTime(DCFtime);
      printLastTimeUpdate();
      serialPrintClock();
    }
    printYear();
    printTime();
  }
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.drawLine(0,DATEPOSY+DATEHIGH*3+2,TFT_WHITE,DATEPOSY+DATEHIGH*2+2,TFT_WHITE);

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
  printStatus("It will take at least 2 minutes");
}

void buttonLoop(){
  M5.update();  
  if (M5.BtnA.wasReleased()) {  // If the button A is pressed.
    if (brightness++ >= BRIGHT_SIZE-1) brightness = 0;
    M5.Axp.ScreenBreath(backlight[brightness]);
  }
}

void loop() {
  buttonLoop();
  dcfLoop();
  delay(80);
}



