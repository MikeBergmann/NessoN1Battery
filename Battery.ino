#include <Arduino_Nesso_N1.h>

NessoBattery battery;
NessoDisplay display;

// --- Color constants (RGB565) ---
const uint16_t COL_BLACK = 0x0000;
const uint16_t COL_BLUE  = 0x001F;
const uint16_t COL_WHITE = 0xFFFF;

// --- Display geometry (landscape 240x135) ---
const int SCR_W = 240;
const int SCR_H = 135;

// Battery symbol dimensions
const int BAT_W      = 120;
const int BAT_H      = 60;
const int BAT_R      = 6;
const int BAT_NUB_W  = 8;
const int BAT_NUB_H  = 24;
const int BAT_BORDER = 3;
const int BAT_PAD    = 3;

// Position (centered, slightly toward the top)
const int BAT_X = (SCR_W - BAT_W - BAT_NUB_W) / 2;
const int BAT_Y = 20;
const int TEXT_Y = BAT_Y + BAT_H + 16;

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 2000;

void drawBatterySymbol(int level) {
  // todo: full-screen clear every 2s update flashes the whole display.
  // Fine at this refresh rate; upgrade to redrawing only changed regions
  // if it flickers — more code, so skip unless it's annoying.
  display.fillScreen(COL_BLACK);

  // Battery frame (3px thick, rounded)
  display.drawRoundRect(BAT_X, BAT_Y, BAT_W, BAT_H, BAT_R, COL_BLUE);
  display.drawRoundRect(BAT_X+1, BAT_Y+1, BAT_W-2, BAT_H-2, BAT_R-1, COL_BLUE);
  display.drawRoundRect(BAT_X+2, BAT_Y+2, BAT_W-4, BAT_H-4, BAT_R-1, COL_BLUE);

  // Positive-terminal nub on the right
  int nubX = BAT_X + BAT_W;
  int nubY = BAT_Y + (BAT_H - BAT_NUB_H) / 2;
  display.fillRoundRect(nubX, nubY, BAT_NUB_W, BAT_NUB_H, 2, COL_BLUE);

  // Compute and draw the fill level
  int innerX = BAT_X + BAT_BORDER + BAT_PAD;
  int innerY = BAT_Y + BAT_BORDER + BAT_PAD;
  int innerW = BAT_W - 2 * (BAT_BORDER + BAT_PAD);
  int innerH = BAT_H - 2 * (BAT_BORDER + BAT_PAD);

  if (level < 0)   level = 0;
  if (level > 100) level = 100;
  int fillW = (innerW * level) / 100;

  if (fillW > 0) {
    display.fillRect(innerX, innerY, fillW, innerH, COL_BLUE);
  }

  // Percentage readout in the center of the symbol
  display.setTextDatum(MC_DATUM);
  display.setTextSize(2);
  display.setTextColor(level > 50 ? COL_WHITE : COL_BLUE,
                       level > 50 ? COL_BLUE  : COL_BLACK);
  char buf[8];
  sprintf(buf, "%d%%", level);
  display.drawString(buf, BAT_X + BAT_W / 2, BAT_Y + BAT_H / 2);

  // Status text below
  display.setTextSize(1);
  display.setTextColor(COL_BLUE, COL_BLACK);
  display.setTextDatum(MC_DATUM);

  NessoBattery::ChargeStatus status = battery.getChargeStatus();
  const char* statusText;
  switch (status) {
    case NessoBattery::PRE_CHARGE:  statusText = "Pre-Charge";   break;
    case NessoBattery::CHARGING:    statusText = "Charging...";  break;
    case NessoBattery::FULL_CHARGE: statusText = "Full";         break;
    default:                        statusText = "Not charging"; break;
  }

  char infoLine[40];
  float voltage = battery.getVoltage();
  sprintf(infoLine, "%.2fV  %s", voltage, statusText);
  display.drawString(infoLine, SCR_W / 2, TEXT_Y);
}

void setup() {
  Serial.begin(115200);

  display.begin();
  display.setRotation(1);
  display.fillScreen(COL_BLACK);

  battery.begin();
  battery.enableCharge();

  // Splash screen
  display.setTextDatum(MC_DATUM);
  display.setTextColor(COL_BLUE, COL_BLACK);
  display.setTextSize(2);
  display.drawString("Nesso N1", SCR_W / 2, SCR_H / 2 - 10);
  display.setTextSize(1);
  display.drawString("Charging battery...", SCR_W / 2, SCR_H / 2 + 20);
  delay(1500);

  lastUpdate = 0;
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;
    uint16_t level = battery.getChargeLevel();
    char dbg[48];
    sprintf(dbg, "Level: %d%%  Voltage: %.2fV", level, battery.getVoltage());
    Serial.println(dbg);
    drawBatterySymbol(level);
  }
}
