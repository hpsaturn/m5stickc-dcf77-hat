#include <M5StickCPlus.h>

#define DATEPOSY 12
#define DATEPOSX 50
#define DATEHIGH 22
#define DATEMARG 20

#define INFOPOSY 97
#define INFOMARG 5

#define SIGNPOSY 150
#define SIGNMARG 6
#define SIGNHIGH 25

#define MAXBANDS 3

void updateField(int posx, int posy, int w, int h, uint16_t color) {
  M5.Lcd.setCursor(posx, posy);
  M5.Lcd.fillRect(posx, posy, w, h, BLACK);
  M5.Lcd.setTextColor(color);
}

void printStatus(const char* msg) {
  M5.Lcd.setTextSize(1);
  updateField(0, INFOPOSY - 6, TFT_WIDTH, DATEHIGH + 5, TFT_WHITE);
  M5.Lcd.setTextDatum(CC_DATUM);

  String input = String(msg);
  int lenght = input.length();

  if (lenght < 16) {
    M5.Lcd.drawString(input, TFT_WIDTH / 2, INFOPOSY);
  } else {
    int found = 0;
    int strIndex[lenght];
    int maxIndex = lenght - 1;

    for (int i = 0; i <= maxIndex; i++) {
      if (input.charAt(i) == ' ') {
        strIndex[found] = i;
        found++;
      }
    }
    int splitpos = ceil(found / 2) + 1;
    M5.Lcd.drawString(input.substring(0, strIndex[splitpos]), TFT_WIDTH / 2, INFOPOSY);
    M5.Lcd.drawString(input.substring(strIndex[splitpos], maxIndex), TFT_WIDTH / 2, INFOPOSY + 9);
  }
}

void printBuffer(const char* msg, uint16_t color) {
  updateField(INFOMARG, INFOPOSY + DATEHIGH + 7, 60, DATEHIGH, color);
  M5.Lcd.printf("B:%s", msg);
}

void printParity(const char* msg, uint16_t color) {
  updateField(TFT_WIDTH - 60, INFOPOSY + DATEHIGH + 7, 60, DATEHIGH, color);
  M5.Lcd.printf("P:%s", msg);
}

void printYear() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.fillRect(DATEPOSX, DATEPOSY, 60, DATEHIGH, BLACK);
  M5.Lcd.setTextDatum(TC_DATUM);
  M5.Lcd.drawString(String(year()), TFT_WIDTH/2, DATEPOSY);
}

void printTime(int posy, uint16_t color) {
  M5.Lcd.setTextSize(2);
  updateField(DATEMARG, posy, M5.Lcd.width() - DATEMARG, DATEHIGH, color);
  M5.Lcd.printf("%02d:%02d:%02d", hour(), minute(), second());
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

void timeLoop() {
  static uint_fast64_t tts = 0;  // timestamp for GUI refresh
  if (millis() - tts > 1000) {
    tts = millis(); 
    printYear();
    printTime(DATEPOSY + DATEHIGH, TFT_WHITE);
  }
}