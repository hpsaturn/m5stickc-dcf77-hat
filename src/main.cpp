#include <Arduino.h>
#include <DCF77.h>

#include "gui.h"

#define DCF77_PON_PIN 0   // DCF77 operation pin
#define DCF_PIN 26        // Connection pin to DCF 77 device
#define DCF_INTERRUPT 26  // Interrupt number associated with pin

#define M5STICKCPLUS_LED GPIO_NUM_10

DCF77 DCF = DCF77(DCF_PIN, DCF_INTERRUPT, true);

#define BRIGHT_SIZE 5
int backlight[BRIGHT_SIZE] = {5, 15, 30, 50, 70};
int brightness = 1;  // MAX brightness is BRIGHT_SIZE

int signalpos = SIGNMARG;  // signal bar position
int bandposy = SIGNPOSY;   // signals band position

volatile unsigned char mySignal = 0;
volatile uint64_t myTimerInterval = 0;

typedef enum {
  BUFFER_MSG_NONE,
  BUFFER_MSG_EOM,
  BUFFER_MSG_BF,
} buffer_msg_t;

volatile buffer_msg_t myOnBufferMsg = BUFFER_MSG_NONE;

typedef enum {
  TIME_UPDATE_MSG_NONE,
  TIME_UPDATE_MSG_CLOSE_TO_INTERNAL_CLOCK,
  TIME_UPDATE_MSG_TIME_LAG_CONSISTENT,
  TIME_UPDATE_MSG_TIME_LAG_INCONSISTENT,
} time_update_t;

volatile time_update_t myOnTimeUpdateMsg = TIME_UPDATE_MSG_NONE;

typedef enum {
  PARITY_ERROR_NONE,
  PARITY_ERROR_ERR,
} parity_error_t;

volatile parity_error_t myOnParityError = PARITY_ERROR_NONE;

uint64_t millis64() {
  return (esp_timer_get_time() / 1000ULL);
}

class mDCF77EventsCallback : public DCF77EventsCallback {
  void onSignal(unsigned char signal) {
    mySignal = signal;
    digitalWrite(M5STICKCPLUS_LED, LOW);
    if (signal == 0)
      myTimerInterval = millis64() + 100;
    else
      myTimerInterval = millis64() + 200;
  };

  void onBufferMsg(const char* msg) {
    if (String(msg) == "EoM")
      myOnBufferMsg = BUFFER_MSG_EOM;
    else if (String(msg) == "BF")
      myOnBufferMsg = BUFFER_MSG_BF;
  };

  void onTimeUpdateMsg(const char* msg) {
    if (String(msg) == "Close to internal clock")
      myOnTimeUpdateMsg = TIME_UPDATE_MSG_CLOSE_TO_INTERNAL_CLOCK;
    else if (String(msg) == "Time lag consistent")
      myOnTimeUpdateMsg = TIME_UPDATE_MSG_TIME_LAG_CONSISTENT;
    else if (String(msg) == "Time lag inconsistent")
      myOnTimeUpdateMsg = TIME_UPDATE_MSG_TIME_LAG_INCONSISTENT;
  };

  void onParityError() {
    myOnParityError = PARITY_ERROR_ERR;
  };
};

void dcfLoop() {
  static uint_fast64_t tts = 0;  // timestamp for GUI refresh
  if (millis64() - tts > 1000) {
    tts = millis64();
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

void cbLoop() {
  if ((myTimerInterval > 0) && (millis64() > myTimerInterval)) {
    myTimerInterval = 0;
    digitalWrite(M5STICKCPLUS_LED, HIGH);

    if (mySignal == 0)
      M5.Lcd.fillRect(signalpos, bandposy, 2, SIGNHIGH, TFT_DARKGREY);
    else
      M5.Lcd.fillRect(signalpos, bandposy, 2, SIGNHIGH, TFT_GREEN);
    signalpos = signalpos + 2;
  }

  if (myOnBufferMsg != BUFFER_MSG_NONE) {
    signalpos = SIGNMARG;
    M5.Lcd.setTextSize(2);
    switch (myOnBufferMsg) {
      case BUFFER_MSG_EOM:
        printBuffer("EoM", TFT_RED);
        printParity("Ukn", TFT_CYAN);
        printStatus("Decoding..");
        bandposy = SIGNPOSY;
        break;
      case BUFFER_MSG_BF:
        printBuffer("BF", TFT_WHITE);
        printParity("Ok", TFT_WHITE);
        bandposy = bandposy + SIGNHIGH + 3;
        if (bandposy >= (SIGNPOSY + (SIGNHIGH + 3) * MAXBANDS)) {
          bandposy = SIGNPOSY;
        }
        break;
    }
    M5.Lcd.fillRect(SIGNMARG, bandposy, 121, SIGNHIGH, BLACK);
    myOnBufferMsg = BUFFER_MSG_NONE;
  }

  if (myOnTimeUpdateMsg != TIME_UPDATE_MSG_NONE) {
    switch (myOnTimeUpdateMsg) {
      case TIME_UPDATE_MSG_CLOSE_TO_INTERNAL_CLOCK:
        printStatus("Close to internal clock");
        break;
      case TIME_UPDATE_MSG_TIME_LAG_CONSISTENT:
        printStatus("Time lag consistent");
        break;
      case TIME_UPDATE_MSG_TIME_LAG_INCONSISTENT:
        printStatus("Time lag inconsistent");
        break;
      default:
        printStatus("Unknown time update msg");
        break;
    }
    myOnTimeUpdateMsg = TIME_UPDATE_MSG_NONE;
  }

  if (myOnParityError == PARITY_ERROR_ERR) {
    updateField(TFT_WIDTH - 60, INFOPOSY + DATEHIGH + 7, 60, DATEHIGH, TFT_CYAN);
    M5.Lcd.printf("P:Err");
    myOnParityError = PARITY_ERROR_NONE;
  }
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  pinMode(M5STICKCPLUS_LED, OUTPUT);
  digitalWrite(M5STICKCPLUS_LED, HIGH);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.drawLine(0, DATEPOSY + DATEHIGH * 3 + 2, TFT_WHITE, DATEPOSY + DATEHIGH * 2 + 2, TFT_WHITE);

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

void buttonLoop() {
  M5.update();
  if (M5.BtnA.wasReleased()) {  // If the button A is pressed.
    if (brightness++ >= BRIGHT_SIZE - 1) brightness = 0;
    M5.Axp.ScreenBreath(backlight[brightness]);
  }
}

void loop() {
  buttonLoop();
  dcfLoop();
  cbLoop();
  delay(80);
}
