#pragma once
#include <M5Cardputer.h>
#include "config.h"

// ============================================================
//  Menu principal — ORUAM BSS
//  Display: 240 × 135 (M5Cardputer ADV)
//  Navegação: ◄ ► = coluna · ▲ = alterna linha · ok/Enter = selecionar
// ============================================================

struct MenuItem {
    const char* label;
    uint16_t    color;
};

const MenuItem MENU_ITEMS[] = {
    { "WIFI",      CLR_GREEN  },
    { "NETWORK",   CLR_BLUE   },
    { "BLUETOOTH", CLR_BLUE   },
    { "HARDWARE",  CLR_ORANGE },
    { "CRYPTO",    CLR_GREEN  },
    { "CONFIG",    CLR_GRAY   },
};

const int MENU_COUNT = 6;
int selectedItem = 0;

// Grid 3 colunas × 2 linhas
// tileW=77  tileH=50  padX=2  padY=18  gapX=2  gapY=2
const int TILE_W = 77;
const int TILE_H = 50;
const int TILE_PAD_X = 2;
const int TILE_PAD_Y = 18;
const int TILE_GAP_X = 2;
const int TILE_GAP_Y = 2;

void drawMenuStub() {
    auto& lcd = M5Cardputer.Display;
    lcd.fillScreen(CLR_BG);

    // topbar
    lcd.fillRect(0, 0, DISPLAY_WIDTH, 16, 0x0010);
    lcd.drawFastHLine(0, 16, DISPLAY_WIDTH, CLR_GREEN_DIM);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(CLR_GREEN);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString("  ORUAM BSS  v" BSS_VERSION, 2, 8);

    // tiles 3×2
    for (int i = 0; i < MENU_COUNT; i++) {
        int col = i % 3;
        int row = i / 3;
        int x   = TILE_PAD_X + col * (TILE_W + TILE_GAP_X);
        int y   = TILE_PAD_Y + row * (TILE_H + TILE_GAP_Y);

        uint16_t borderColor = (i == selectedItem) ? MENU_ITEMS[i].color : 0x0841;
        uint16_t bgColor     = (i == selectedItem) ? 0x0841 : 0x0420;
        uint16_t txtColor    = (i == selectedItem) ? MENU_ITEMS[i].color : CLR_GRAY;

        lcd.fillRoundRect(x, y, TILE_W, TILE_H, 4, bgColor);
        lcd.drawRoundRect(x, y, TILE_W, TILE_H, 4, borderColor);

        lcd.setTextDatum(middle_center);
        lcd.setTextColor(txtColor);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(MENU_ITEMS[i].label, x + TILE_W / 2, y + TILE_H / 2);
    }

    // statusbar
    lcd.drawFastHLine(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, 0x0010);
    lcd.fillRect(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, 13, 0x0008);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(0x0300);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString("  < > nav  ^ row  ok select", 2, DISPLAY_HEIGHT - 6);
}

void handleMenuInput() {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();

        int col = selectedItem % 3;
        int row = selectedItem / 3;
        bool nav = false;

        for (auto ch : status.word) {
            uint8_t code = (uint8_t)ch;
            // Códigos das setas físicas do Cardputer ADV via M5Unified.
            // Se a navegação não funcionar, adicione no setup():
            //   Serial.printf("key: 0x%02X\n", code);
            // e ajuste os valores abaixo com os códigos reais.
            switch (code) {
                case 0xB4: col = (col + 2) % 3; nav = true; break; // seta esquerda
                case 0xB7: col = (col + 1) % 3; nav = true; break; // seta direita
                case 0xB5: row = 1 - row;        nav = true; break; // seta cima
                case 0xB6: row = 1 - row;        nav = true; break; // seta baixo
            }
        }

        if (nav) {
            selectedItem = row * 3 + col;
            drawMenuStub();
        }

        if (status.enter) {
            auto& lcd = M5Cardputer.Display;
            lcd.fillScreen(CLR_BG);
            lcd.setTextDatum(middle_center);
            lcd.setTextColor(CLR_GREEN);
            lcd.setFont(&fonts::DejaVu9);
            lcd.drawString("[ " + String(MENU_ITEMS[selectedItem].label) + " ]",
                           DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 8);
            lcd.setTextColor(CLR_GRAY);
            lcd.drawString("coming soon...", DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 8);
            delay(1200);
            drawMenuStub();
        }
    }

    // BtnA (botão lateral) = navega próximo item como fallback
    if (M5Cardputer.BtnA.wasPressed()) {
        selectedItem = (selectedItem + 1) % MENU_COUNT;
        drawMenuStub();
    }
}
