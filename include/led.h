#pragma once
#include <Adafruit_NeoPixel.h>
#include "config.h"

// ============================================================
//  LED RGB (NeoPixel WS2812) — M5Cardputer pin 21
// ============================================================

static Adafruit_NeoPixel _pixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);
static bool _pixelReady = false;

static void _ledInit() {
    if (_pixelReady) return;
    _pixel.begin();
    _pixel.setBrightness(200);
    _pixelReady = true;
}

static void _ledOn(uint8_t r, uint8_t g, uint8_t b) {
    _ledInit();
    _pixel.setPixelColor(0, _pixel.Color(r, g, b));
    _pixel.show();
}

static void _ledOff() {
    _ledInit();
    _pixel.setPixelColor(0, 0);
    _pixel.show();
}

// Morse: . = dot, - = dash
static void _morseLetter(const char* pattern, uint8_t r, uint8_t g, uint8_t b,
                          int unit = 80) {
    for (const char* p = pattern; *p; p++) {
        _ledOn(r, g, b);
        delay(*p == '.' ? unit : unit * 3);
        _ledOff();
        delay(unit); // gap entre elementos
    }
    delay(unit * 2); // gap entre letras (total 3 unidades)
}

// Pisca "CONNECTED" em Morse no LED verde
void ledMorseConnected() {
    _morseLetter("-.-.", 0, 180, 0); // C
    _morseLetter("---",  0, 180, 0); // O
    _morseLetter("-.",   0, 180, 0); // N
    _morseLetter("-.",   0, 180, 0); // N
    _morseLetter(".",    0, 180, 0); // E
    _morseLetter("-.-.", 0, 180, 0); // C
    _morseLetter("-",    0, 180, 0); // T
    _morseLetter(".",    0, 180, 0); // E
    _morseLetter("-..",  0, 180, 0); // D
    _ledOff();
}

// Pisca 3x vermelho — falha de conexão
void ledBlinkFailed() {
    for (int i = 0; i < 3; i++) {
        _ledOn(180, 0, 0);
        delay(200);
        _ledOff();
        delay(200);
    }
}
