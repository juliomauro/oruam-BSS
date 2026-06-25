#pragma once

// ============================================================
//  ORUAM BSS — Basic Security Suite
//  Version & build info
// ============================================================

#define BSS_NAME        "ORUAM BSS"
#define BSS_VERSION     "1.0.0"
#define BSS_AUTHOR      "Oruam Oliuj"
#define BSS_CODENAME    "KIRK"

// Display — M5Cardputer (ST7789, 1.14", landscape 240×135)
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  135

// Colors (RGB565)
#define CLR_BG          0x0000  // preto
#define CLR_GREEN       0x07E0  // verde primário
#define CLR_GREEN_DIM   0x03E0  // verde escuro
#define CLR_WHITE       0xFFFF
#define CLR_GRAY        0x4208  // cinza médio
#define CLR_DARK        0x0841  // cinza muito escuro (tile bg)
#define CLR_TOPBAR      0x2104  // cinza escuro neutro (topbar/statusbar)
#define CLR_STATUSBAR   0x1082  // cinza quase preto (statusbar bg)

// Hardware
#define LED_PIN         21      // NeoPixel RGB (WS2812) do M5Cardputer
