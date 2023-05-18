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

#define DATEPOSY 12
#define DATEPOSX 50
#define DATEHIGH 22
#define DATEMARG 20

#define INFOPOSY 95
#define INFOMARG 5

#define SIGNPOSY 150
#define SIGNMARG 6
#define SIGNHIGH 25

#define MAXBANDS 3

int signalpos = SIGNMARG;
int bandposy = SIGNPOSY;

void updateField(int posx, int posy, int w, int h, uint16_t color) {
  M5.Lcd.setCursor(posx, posy);
  M5.Lcd.fillRect(posx, posy, w, h, BLACK);
  M5.Lcd.setTextColor(color);
}

void printStatus(const char* msg) {
  M5.Lcd.setTextSize(1);
  updateField(0, INFOPOSY - 6, TFT_WIDTH, DATEHIGH + 5, TFT_WHITE);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.drawString(String(msg), TFT_WIDTH / 2, INFOPOSY);
}

void printBuffer(const char* msg, uint16_t color) {
  updateField(INFOMARG, INFOPOSY + DATEHIGH + 7, 60, DATEHIGH, color);
  M5.Lcd.printf("B:%s", msg);
}

void printParity(const char* msg, uint16_t color) {
  updateField(TFT_WIDTH - 60, INFOPOSY + DATEHIGH + 7, 60, DATEHIGH, color);
  M5.Lcd.printf("P:%s", msg);
}

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
  void onTimeUpdate(time_t DCFtime) {
    Serial.println("Time is updated");
    setTime(DCFtime);
    M5.Lcd.setTextSize(2);
    updateField(DATEMARG, DATEPOSY + DATEHIGH * 2, M5.Lcd.width() - DATEMARG, DATEHIGH, TFT_YELLOW);
    M5.Lcd.printf("%02d:%02d:%02d", hour(), minute(), second());
  };
  void onTimeUpdateMsg(const char* msg){
    printStatus(msg);
  };
  void onParityError() {
    updateField(TFT_WIDTH - 60, INFOPOSY+DATEHIGH+7, 60, DATEHIGH, TFT_CYAN);
    M5.Lcd.printf("P:Err");
  };
};

void displayLoop() {
  static uint_fast64_t tts = 0;  // timestamp for GUI refresh
  if (millis() - tts > 1000) {
    tts = millis(); 
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(DATEPOSX, DATEPOSY);
    M5.Lcd.setTextColor(TFT_WHITE);
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
  printStatus("Decoding..");
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
  buttonLoop();
  displayLoop();
  delay(80);
}



