// ============================================================
//  ORUAM BSS — Basic Security Suite
//  M5Cardputer · Firmware v1.0.0
//
//  Author  : Oruam Oliuj
//  License : Apache 2.0
//  Repo    : https://github.com/juliomauro/oruam-BSS
// ============================================================

#include <Arduino.h>
#include <M5Cardputer.h>
#include "config.h"
#include "splash.h"
#include "menu.h"

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);

    Serial.begin(115200);
    Serial.println("\n\n⚡ ORUAM BSS v" BSS_VERSION " — booting...");
    Serial.println("   Author  : " BSS_AUTHOR);
    Serial.println("   Codename: " BSS_CODENAME);
    Serial.println("   Board   : M5Cardputer");

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setColorDepth(16);

    drawSplash();
    drawMenuStub();

    Serial.println("   Status  : READY\n");
}

void loop() {
    handleMenuInput();
    delay(20);
}
