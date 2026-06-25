#pragma once
#include <SD.h>
#include <SPI.h>
#include "config.h"

// ============================================================
//  SD Card — M5Cardputer pins (via Bruce firmware reference)
//  CS=12  SCK=40  MISO=39  MOSI=14
// ============================================================

#define SD_PIN_CS   12
#define SD_PIN_SCK  40
#define SD_PIN_MISO 39
#define SD_PIN_MOSI 14
#define BSS_SD_DIR  "/BSS"

static bool _sdMounted = false;

static bool sdBegin() {
    if (_sdMounted) return true;
    SPI.begin(SD_PIN_SCK, SD_PIN_MISO, SD_PIN_MOSI, SD_PIN_CS);
    _sdMounted = SD.begin(SD_PIN_CS, SPI, 25000000);
    return _sdMounted;
}

// Escreve rows em CSV numerado. Retorna o caminho gerado ou "" em erro.
static String sdWriteCsv(const char* prefix, const char* header,
                         String rows[], int count) {
    if (!sdBegin()) return "";
    if (!SD.exists(BSS_SD_DIR)) SD.mkdir(BSS_SD_DIR);

    char path[48];
    int n = 1;
    do {
        snprintf(path, sizeof(path), "%s/%s_%03d.csv", BSS_SD_DIR, prefix, n++);
    } while (SD.exists(path) && n < 999);

    File f = SD.open(path, FILE_WRITE);
    if (!f) return "";
    f.println(header);
    for (int i = 0; i < count; i++) f.println(rows[i]);
    f.close();
    return String(path);
}
