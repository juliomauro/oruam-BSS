#pragma once
#include <M5Unified.h>
#include "config.h"

// ============================================================
//  Splash screen de boot — ORUAM BSS
//  Display: 240 × 135 (M5Cardputer)
// ============================================================

void drawSplash() {
    auto& lcd = M5.Display;

    lcd.fillScreen(CLR_BG);

    // borda decorativa
    lcd.drawRect(2, 2, DISPLAY_WIDTH - 4, DISPLAY_HEIGHT - 4, CLR_GREEN_DIM);
    lcd.drawRect(3, 3, DISPLAY_WIDTH - 6, DISPLAY_HEIGHT - 6, 0x0200);

    // título principal
    lcd.setTextDatum(middle_center);
    lcd.setTextColor(CLR_GREEN);
    lcd.setFont(&fonts::Orbitron_Light_24);
    lcd.drawString("ORUAM BSS", DISPLAY_WIDTH / 2, 40);

    // subtítulo
    lcd.setTextColor(CLR_GRAY);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString("BASIC  SECURITY  SUITE", DISPLAY_WIDTH / 2, 62);

    // linha separadora
    lcd.drawFastHLine(20, 72, DISPLAY_WIDTH - 40, CLR_GREEN_DIM);

    // versão
    lcd.setTextColor(CLR_GREEN_DIM);
    lcd.drawString("v" BSS_VERSION, DISPLAY_WIDTH / 2, 82);

    // loading bar
    int barX = 20;
    int barY = 104;
    int barW = DISPLAY_WIDTH - 40;
    int barH = 5;

    lcd.drawRect(barX - 1, barY - 1, barW + 2, barH + 2, CLR_GREEN_DIM);

    for (int i = 0; i <= barW; i += 4) {
        lcd.fillRect(barX, barY, i, barH, CLR_GREEN);
        delay(10);
    }

    // ready
    lcd.setTextColor(CLR_GREEN);
    lcd.drawString("SYSTEM READY", DISPLAY_WIDTH / 2, 124);

    delay(800);
}
