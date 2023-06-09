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

void updateField(int32_t posx, int32_t posy, int32_t w, int32_t h, uint16_t color) {
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
    int splitpos = ceil(found / 2);
    M5.Lcd.drawString(input.substring(0, strIndex[splitpos]), TFT_WIDTH / 2, INFOPOSY);
    M5.Lcd.drawString(input.substring(strIndex[splitpos]), TFT_WIDTH / 2, INFOPOSY + 9);
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
  M5.Lcd.setTextDatum(TC_DATUM);
  M5.Lcd.fillRect(0, DATEPOSY, TFT_WIDTH, DATEHIGH*2, BLACK);
  M5.Lcd.drawString(String(year()), TFT_WIDTH/2, DATEPOSY);
}

void printTime() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(DATEMARG,DATEPOSY+DATEHIGH);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.printf("%02d:%02d:%02d", hour(), minute(), second());
}

void printLastTimeUpdate() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(DATEMARG, DATEPOSY+DATEHIGH*2, TFT_WIDTH-DATEMARG, DATEHIGH, BLACK);
  M5.Lcd.setCursor(DATEMARG,DATEPOSY+DATEHIGH*2);
  M5.Lcd.setTextColor(TFT_YELLOW);
  M5.Lcd.printf("%02d:%02d:%02d", hour(), minute(), second());
}

void serialPrintClock() {
  Serial.printf("%02d:%02d:%02d %02d.%02d.%02d\r\n", hour(), minute(), second(), year(), month(), day());
}
