#pragma once
#include <M5Cardputer.h>
#include "config.h"

// ============================================================
//  Utilitários reutilizáveis de UI — sub-menu e entrada de texto
// ============================================================

static void _drawTopBar(const char* title) {
    auto& lcd = M5Cardputer.Display;
    lcd.fillRect(0, 0, DISPLAY_WIDTH, 16, CLR_TOPBAR);
    lcd.drawFastHLine(0, 16, DISPLAY_WIDTH, CLR_GREEN_DIM);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(CLR_GREEN);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString(String("  ") + title, 2, 8);
}

static void _drawStatusBar(const char* hint) {
    auto& lcd = M5Cardputer.Display;
    lcd.drawFastHLine(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, CLR_TOPBAR);
    lcd.fillRect(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, 13, CLR_STATUSBAR);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(0x0300);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString(hint, 4, DISPLAY_HEIGHT - 6);
}

// Exibe uma tela com título e até duas linhas de mensagem.
// Retorna quando o usuário pressionar qualquer tecla.
void showMessage(const char* title, const char* line1,
                 const char* line2 = nullptr, uint16_t color = CLR_GREEN) {
    auto& lcd = M5Cardputer.Display;
    lcd.fillScreen(CLR_BG);
    _drawTopBar(title);

    lcd.setTextDatum(middle_center);
    lcd.setTextColor(color);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString(line1, DISPLAY_WIDTH / 2, line2 ? 58 : 67);
    if (line2) {
        lcd.setTextColor(CLR_WHITE);
        lcd.drawString(line2, DISPLAY_WIDTH / 2, 74);
    }

    _drawStatusBar("  any key to go back");

    // aguarda soltar todas as teclas antes de aceitar input novo
    delay(80);
    while (M5Cardputer.Keyboard.isPressed()) { M5Cardputer.update(); delay(20); }

    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) return;
        if (M5Cardputer.BtnA.wasPressed()) return;
        delay(20);
    }
}

// Sub-menu vertical. Retorna índice selecionado ou -1 (voltar com ',').
int drawSubMenu(const char* title, const char* items[], int count) {
    int selected = 0;

    auto draw = [&]() {
        auto& lcd = M5Cardputer.Display;
        lcd.fillScreen(CLR_BG);
        _drawTopBar(title);

        for (int i = 0; i < count; i++) {
            int y = 28 + i * 18;
            if (i == selected) {
                lcd.fillRect(0, y - 7, DISPLAY_WIDTH, 16, 0x0841);
                lcd.drawFastVLine(0, y - 7, 16, CLR_GREEN);
                lcd.setTextColor(CLR_WHITE);
            } else {
                lcd.setTextColor(0x630C);
            }
            lcd.setTextDatum(middle_left);
            lcd.setFont(&fonts::DejaVu9);
            lcd.drawString(String(i == selected ? "> " : "  ") + items[i], 8, y);
        }

        _drawStatusBar("  ▲ up  ▼ down  ok select  ESC back");
    };

    draw();

    while (true) {
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            auto st = M5Cardputer.Keyboard.keysState();

            for (auto ch : st.word) {
                uint8_t code = (uint8_t)ch;
                if (code == 0x3B && selected > 0)                    { selected--; draw(); } // ; cima
                if (code == 0x2E && selected < count - 1)            { selected++; draw(); } // . baixo
                if (code == 0x60 || code == 0x1B || code == 0x2C)   return -1;              // ` ESC , volta
            }

            if (st.enter) return selected;
        }

        if (M5Cardputer.BtnA.wasPressed()) return -1;
        delay(20);
    }
}

// Campo de entrada de texto. mask=true mostra asteriscos (senha).
// Retorna string vazia se o usuário confirmar sem digitar nada.
String readTextInput(const char* label, bool mask = false) {
    String value = "";

    auto draw = [&]() {
        auto& lcd = M5Cardputer.Display;
        lcd.fillScreen(CLR_BG);
        _drawTopBar(label);

        lcd.drawRect(8, 28, DISPLAY_WIDTH - 16, 20, CLR_GREEN_DIM);
        lcd.fillRect(9, 29, DISPLAY_WIDTH - 18, 18, 0x0841);

        String masked = "";
        for (int i = 0; i < (int)value.length(); i++) masked += '*';
        String display = mask ? masked : value;
        display += "_";
        lcd.setTextDatum(middle_left);
        lcd.setTextColor(CLR_WHITE);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(display, 12, 38);

        _drawStatusBar("  type  del backspace  enter ok  esc cancel");
    };

    draw();

    while (true) {
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            auto st = M5Cardputer.Keyboard.keysState();

            if (st.del && value.length() > 0) { value.remove(value.length() - 1); draw(); }
            if (st.enter) return value;

            for (auto ch : st.word) {
                uint8_t code = (uint8_t)ch;
                if (code == 0x60 || code == 0x1B) return "";        // ` ou ESC cancela
                if (code >= 0x20 && code < 0x7F) { value += ch; draw(); }
            }
        }

        delay(20);
    }
}
