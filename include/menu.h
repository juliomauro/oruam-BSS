#pragma once
#include <M5Cardputer.h>
#include <WiFi.h>
#include "config.h"
#include "input.h"
#include "modules/config.h"
#include "modules/network.h"

// ============================================================
//  Menu principal — ORUAM BSS
//  Display: 240 × 135 (M5Cardputer ADV)
//  Navegação: ◄ ► = coluna · ▲ = alterna linha · ok/Enter = selecionar
// ============================================================

struct MenuItem {
    const char* label;
};

const MenuItem MENU_ITEMS[] = {
    { "WIFI"      },
    { "NETWORK"   },
    { "BLUETOOTH" },
    { "HARDWARE"  },
    { "CRYPTO"    },
    { "CONFIG"    },
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
    lcd.fillRect(0, 0, DISPLAY_WIDTH, 16, CLR_TOPBAR);
    lcd.drawFastHLine(0, 16, DISPLAY_WIDTH, CLR_GREEN_DIM);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(CLR_GREEN);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString("  ORUAM BSS  v" BSS_VERSION, 2, 8);

    // indicador WiFi
    if (WiFi.status() == WL_CONNECTED) {
        lcd.setTextDatum(middle_right);
        lcd.setTextColor(CLR_GREEN);
        lcd.drawString("WiFi \xb7 ", DISPLAY_WIDTH, 8);
    }

    // tiles 3×2
    for (int i = 0; i < MENU_COUNT; i++) {
        int col = i % 3;
        int row = i / 3;
        int x   = TILE_PAD_X + col * (TILE_W + TILE_GAP_X);
        int y   = TILE_PAD_Y + row * (TILE_H + TILE_GAP_Y);

        uint16_t borderColor = (i == selectedItem) ? CLR_GREEN : 0x2104;
        uint16_t bgColor     = 0x0841;
        uint16_t txtColor    = (i == selectedItem) ? CLR_WHITE : 0x630C;

        lcd.fillRoundRect(x, y, TILE_W, TILE_H, 4, bgColor);
        lcd.drawRoundRect(x, y, TILE_W, TILE_H, 4, borderColor);

        lcd.setTextDatum(middle_center);
        lcd.setTextColor(txtColor);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(MENU_ITEMS[i].label, x + TILE_W / 2, y + TILE_H / 2);
    }

    // statusbar
    lcd.drawFastHLine(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, CLR_TOPBAR);
    lcd.fillRect(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, 13, CLR_STATUSBAR);
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
                case 0x2C: col = (col + 2) % 3; nav = true; break; // , → esquerda
                case 0x2F: col = (col + 1) % 3; nav = true; break; // / → direita
                case 0x3B: row = 1 - row;        nav = true; break; // ; → cima
                case 0x2E: row = 1 - row;        nav = true; break; // . → baixo
            }
        }

        if (nav) {
            selectedItem = row * 3 + col;
            drawMenuStub();
        }

        if (status.enter) {
            switch (selectedItem) {
                case 1: runNetwork(); break;
                case 5: runConfig();  break;
                default:
                    showMessage(MENU_ITEMS[selectedItem].label, "coming soon...", nullptr, CLR_GRAY);
                    break;
            }
            drawMenuStub();
        }
    }

    // BtnA (botão lateral) = navega próximo item como fallback
    if (M5Cardputer.BtnA.wasPressed()) {
        selectedItem = (selectedItem + 1) % MENU_COUNT;
        drawMenuStub();
    }
}
