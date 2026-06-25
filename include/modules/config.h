#pragma once
#include <M5Cardputer.h>
#include <WiFi.h>
#include "../config.h"
#include "../input.h"
#include "../led.h"

// ============================================================
//  Módulo CONFIG — WiFi Connect · About
// ============================================================

static void _configWifiConnect() {
    String ssid = readTextInput("WiFi SSID");
    if (ssid.isEmpty()) return;

    String password = readTextInput("WiFi Password", true);

    // tela de connecting
    auto& lcd = M5Cardputer.Display;
    lcd.fillScreen(CLR_BG);
    lcd.fillRect(0, 0, DISPLAY_WIDTH, 16, CLR_TOPBAR);
    lcd.drawFastHLine(0, 16, DISPLAY_WIDTH, CLR_GREEN_DIM);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(CLR_GREEN);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString("  WiFi Connect", 2, 8);

    lcd.setTextDatum(middle_center);
    lcd.setTextColor(0x630C);
    lcd.drawString("Connecting to", DISPLAY_WIDTH / 2, 50);
    lcd.setTextColor(CLR_WHITE);
    lcd.drawString(ssid, DISPLAY_WIDTH / 2, 65);

    if (password.isEmpty()) WiFi.begin(ssid.c_str());
    else                    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long start = millis();
    int dots = 0;
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        String progress = "";
        for (int i = 0; i < (dots % 4); i++) progress += ".";
        lcd.fillRect(0, 78, DISPLAY_WIDTH, 14, CLR_BG);
        lcd.setTextDatum(middle_center);
        lcd.setTextColor(CLR_GREEN_DIM);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(progress, DISPLAY_WIDTH / 2, 84);
        dots++;
        delay(400);
    }

    if (WiFi.status() == WL_CONNECTED) {
        String ip = WiFi.localIP().toString();
        showMessage("WiFi Connect", "Connected!", ip.c_str(), CLR_GREEN);
        ledMorseConnected();
    } else {
        WiFi.disconnect(true);
        ledBlinkFailed();
        showMessage("WiFi Connect", "Connection failed", ssid.c_str(), 0xF800);
    }
}

static void _configAbout() {
    auto& lcd = M5Cardputer.Display;
    lcd.fillScreen(CLR_BG);
    lcd.fillRect(0, 0, DISPLAY_WIDTH, 16, CLR_TOPBAR);
    lcd.drawFastHLine(0, 16, DISPLAY_WIDTH, CLR_GREEN_DIM);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(CLR_GREEN);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString("  About", 2, 8);

    auto row = [&](int y, const char* label, String value, uint16_t col = CLR_WHITE) {
        lcd.setTextDatum(middle_left);
        lcd.setFont(&fonts::DejaVu9);
        lcd.setTextColor(0x630C);
        lcd.drawString(label, 10, y);
        lcd.setTextColor(col);
        lcd.drawString(value, 95, y);
    };

    row(28,  "Firmware :", BSS_NAME " v" BSS_VERSION);
    row(41,  "Codename :", BSS_CODENAME, CLR_GREEN);
    row(54,  "Author   :", BSS_AUTHOR);
    row(67,  "Board    :", "M5Cardputer ADV");
    row(80,  "Chip     :", "ESP32-S3");
    row(93,  "Free heap:", String(ESP.getFreeHeap() / 1024) + " KB");

    if (WiFi.status() == WL_CONNECTED) {
        row(106, "IP       :", WiFi.localIP().toString(), CLR_GREEN);
    }

    lcd.drawFastHLine(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, CLR_TOPBAR);
    lcd.fillRect(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, 13, CLR_STATUSBAR);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(0x0300);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString("  any key to go back", 4, DISPLAY_HEIGHT - 6);

    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) return;
        if (M5Cardputer.BtnA.wasPressed()) return;
        delay(20);
    }
}

static void _configBrightness() {
    auto& lcd = M5Cardputer.Display;
    uint8_t brightness = lcd.getBrightness();
    uint8_t original   = brightness;

    auto draw = [&]() {
        lcd.fillScreen(CLR_BG);
        lcd.fillRect(0, 0, DISPLAY_WIDTH, 16, CLR_TOPBAR);
        lcd.drawFastHLine(0, 16, DISPLAY_WIDTH, CLR_GREEN_DIM);
        lcd.setTextDatum(middle_left);
        lcd.setTextColor(CLR_GREEN);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString("  Brightness", 2, 8);

        // valor percentual
        int pct = (int)(brightness / 255.0f * 100);
        lcd.setTextDatum(middle_center);
        lcd.setTextColor(CLR_WHITE);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(String(pct) + "%", DISPLAY_WIDTH / 2, 44);

        // slider
        const int sx = 20, sy = 58, sw = 200, sh = 10;
        int fill = (int)(brightness / 255.0f * sw);
        lcd.drawRect(sx - 1, sy - 1, sw + 2, sh + 2, CLR_GREEN_DIM);
        lcd.fillRect(sx, sy, sw, sh, CLR_DARK);
        if (fill > 0) lcd.fillRect(sx, sy, fill, sh, CLR_GREEN);

        // cursor
        int cx = sx + fill;
        lcd.fillRect(cx - 1, sy - 3, 3, sh + 6, CLR_WHITE);

        lcd.drawFastHLine(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, CLR_TOPBAR);
        lcd.fillRect(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, 13, CLR_STATUSBAR);
        lcd.setTextDatum(middle_left);
        lcd.setTextColor(0x0300);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString("  ► +5  ◄ -5  ▲ +1  ▼ -1  enter ok  ESC cancel", 4, DISPLAY_HEIGHT - 6);

        lcd.setBrightness(brightness);
    };

    draw();

    while (true) {
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            auto st = M5Cardputer.Keyboard.keysState();

            if (st.enter) return;

            for (auto ch : st.word) {
                uint8_t code = (uint8_t)ch;
                if (code == 0x60 || code == 0x1B) {
                    lcd.setBrightness(original);
                    return;
                }
                if (code == 0x2F && brightness <= 250) { brightness += 5;  draw(); } // / +5
                if (code == 0x2C && brightness >= 5)   { brightness -= 5;  draw(); } // , -5
                if (code == 0x3B && brightness <= 254) { brightness += 1;  draw(); } // ; +1
                if (code == 0x2E && brightness >= 1)   { brightness -= 1;  draw(); } // . -1
            }
        }
        delay(20);
    }
}

void runConfig() {
    const char* items[] = { "WiFi Connect", "Brightness", "About" };
    while (true) {
        int sel = drawSubMenu("CONFIG", items, 3);
        if (sel < 0) return;
        switch (sel) {
            case 0: _configWifiConnect(); break;
            case 1: _configBrightness();  break;
            case 2: _configAbout();       break;
        }
    }
}
