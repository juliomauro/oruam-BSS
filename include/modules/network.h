#pragma once
#include <M5Cardputer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "../config.h"
#include "../input.h"
#include "../sd_utils.h"

// ============================================================
//  Módulo NETWORK — Host Discovery · Port Scan · DNS Lookup
// ============================================================

static bool _netWifiCheck() {
    if (WiFi.status() == WL_CONNECTED) return true;
    showMessage("NETWORK", "WiFi not connected", "CONFIG > WiFi Connect", 0xF800);
    return false;
}

static void _netTopBar(const char* title) {
    auto& lcd = M5Cardputer.Display;
    lcd.fillRect(0, 0, DISPLAY_WIDTH, 16, CLR_TOPBAR);
    lcd.drawFastHLine(0, 16, DISPLAY_WIDTH, CLR_GREEN_DIM);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(CLR_GREEN);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString(String("  ") + title, 2, 8);
}

static void _netStatusBar(const char* hint) {
    auto& lcd = M5Cardputer.Display;
    lcd.drawFastHLine(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, CLR_TOPBAR);
    lcd.fillRect(0, DISPLAY_HEIGHT - 13, DISPLAY_WIDTH, 13, CLR_STATUSBAR);
    lcd.setTextDatum(middle_left);
    lcd.setTextColor(0x0300);
    lcd.setFont(&fonts::DejaVu9);
    lcd.drawString(hint, 4, DISPLAY_HEIGHT - 6);
}

static bool _checkAbort() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        for (auto ch : M5Cardputer.Keyboard.keysState().word) {
            if ((uint8_t)ch == 0x60 || (uint8_t)ch == 0x1B) return true;
        }
    }
    return false;
}

// ── Host Discovery ──────────────────────────────────────────

static bool _tcpUp(IPAddress ip) {
    WiFiClient c;
    c.setTimeout(100);
    bool up = c.connect(ip, 80);
    if (up) { c.stop(); return true; }
    up = c.connect(ip, 22);
    c.stop();
    return up;
}

static void _netHostDiscovery() {
    if (!_netWifiCheck()) return;

    IPAddress local = WiFi.localIP();
    uint8_t b0 = local[0], b1 = local[1], b2 = local[2];
    String network = String(b0) + "." + b1 + "." + b2 + ".x/24";

    const int MAX_HOSTS = 32;
    IPAddress found[MAX_HOSTS];
    int foundCount = 0;
    bool aborted   = false;
    const int TOTAL = 254;

    auto& lcd = M5Cardputer.Display;

    auto drawScan = [&](int n) {
        lcd.fillScreen(CLR_BG);
        _netTopBar("HOST DISCOVERY");
        lcd.setTextDatum(middle_left);
        lcd.setTextColor(0x630C);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(network, 8, 26);

        const int bx = 8, by = 35, bw = 224, bh = 6;
        int fill = n * bw / TOTAL;
        lcd.drawRect(bx-1, by-1, bw+2, bh+2, CLR_GREEN_DIM);
        lcd.fillRect(bx, by, bw, bh, CLR_DARK);
        if (fill > 0) lcd.fillRect(bx, by, fill, bh, CLR_GREEN);

        lcd.setTextColor(CLR_WHITE);
        lcd.drawString(String(n) + "/" + TOTAL + "  found: " + foundCount, 8, 50);

        int from = max(0, foundCount - 4);
        for (int i = from; i < foundCount; i++) {
            lcd.setTextColor(CLR_GREEN);
            lcd.drawString(found[i].toString(), 12, 62 + (i - from) * 12);
        }

        _netStatusBar("  ESC abort");
    };

    drawScan(0);

    for (int h = 1; h <= TOTAL && !aborted; h++) {
        IPAddress target(b0, b1, b2, (uint8_t)h);
        if (target != local && _tcpUp(target) && foundCount < MAX_HOSTS)
            found[foundCount++] = target;
        drawScan(h);
        if (_checkAbort()) aborted = true;
    }

    if (foundCount == 0) {
        showMessage("Host Discovery", aborted ? "Scan aborted" : "No hosts found",
                    "TCP/80,22 only", CLR_GRAY);
        return;
    }

    int scroll = 0;
    const int VIS = 6;

    auto drawResults = [&]() {
        lcd.fillScreen(CLR_BG);
        _netTopBar("HOST DISCOVERY");
        lcd.setTextDatum(middle_left);
        lcd.setTextColor(0x630C);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString("Found " + String(foundCount) + (aborted ? " (aborted)" : "") + " — TCP/80,22", 8, 26);
        for (int i = scroll; i < min(foundCount, scroll + VIS); i++) {
            lcd.setTextColor(CLR_GREEN);
            lcd.drawString(found[i].toString(), 12, 38 + (i - scroll) * 13);
        }
        _netStatusBar("  ▲ up  ▼ down  s save  ESC back");
    };

    drawResults();

    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            for (auto ch : M5Cardputer.Keyboard.keysState().word) {
                uint8_t c = (uint8_t)ch;
                if (c == 0x60 || c == 0x1B)                        return;
                if (c == 0x3B && scroll > 0)                       { scroll--; drawResults(); }
                if (c == 0x2E && scroll < foundCount - VIS)        { scroll++; drawResults(); }
                if (c == 's') {
                    String rows[MAX_HOSTS];
                    for (int i = 0; i < foundCount; i++) rows[i] = found[i].toString();
                    String path = sdWriteCsv("hosts", "ip", rows, foundCount);
                    if (path.isEmpty()) showMessage("Save CSV", "SD card error", "Check SD card", 0xF800);
                    else                showMessage("Save CSV", "Saved!", path.c_str(), CLR_GREEN);
                    drawResults();
                }
            }
        }
        delay(20);
    }
}

// ── Port Scan ────────────────────────────────────────────────

struct _PortDef { uint16_t num; const char* name; };
const _PortDef _PORTS[] = {
    {21,"FTP"},{22,"SSH"},{23,"Telnet"},{25,"SMTP"},
    {53,"DNS"},{80,"HTTP"},{110,"POP3"},{143,"IMAP"},
    {443,"HTTPS"},{445,"SMB"},{3306,"MySQL"},{3389,"RDP"},
    {5900,"VNC"},{8080,"HTTP-Alt"},{8443,"HTTPS-Alt"}
};
const int _NPORTS = sizeof(_PORTS) / sizeof(_PORTS[0]);

static void _netPortScan() {
    if (!_netWifiCheck()) return;

    String targetStr = readTextInput("Target IP");
    if (targetStr.isEmpty()) return;

    IPAddress targetIP;
    if (!targetIP.fromString(targetStr)) {
        showMessage("Port Scan", "Invalid IP address", targetStr.c_str(), 0xF800);
        return;
    }

    uint16_t    openPort[_NPORTS];
    const char* openName[_NPORTS];
    int  openCount = 0;
    bool aborted   = false;

    auto& lcd = M5Cardputer.Display;

    auto drawScan = [&](int n) {
        lcd.fillScreen(CLR_BG);
        _netTopBar("PORT SCAN");
        lcd.setTextDatum(middle_left);
        lcd.setTextColor(0x630C);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(targetStr, 8, 26);

        const int bx = 8, by = 35, bw = 224, bh = 6;
        int fill = n * bw / _NPORTS;
        lcd.drawRect(bx-1, by-1, bw+2, bh+2, CLR_GREEN_DIM);
        lcd.fillRect(bx, by, bw, bh, CLR_DARK);
        if (fill > 0) lcd.fillRect(bx, by, fill, bh, CLR_GREEN);

        lcd.setTextColor(CLR_WHITE);
        lcd.drawString(String(n) + "/" + _NPORTS + "  open: " + openCount, 8, 50);

        int from = max(0, openCount - 4);
        for (int i = from; i < openCount; i++) {
            lcd.setTextColor(CLR_GREEN);
            lcd.drawString(String(openPort[i]) + "  " + openName[i], 12, 62 + (i - from) * 12);
        }

        _netStatusBar("  ESC abort");
    };

    drawScan(0);

    for (int i = 0; i < _NPORTS && !aborted; i++) {
        WiFiClient c;
        c.setTimeout(300);
        if (c.connect(targetIP, _PORTS[i].num)) {
            openPort[openCount] = _PORTS[i].num;
            openName[openCount] = _PORTS[i].name;
            openCount++;
            c.stop();
        }
        drawScan(i + 1);
        if (_checkAbort()) aborted = true;
    }

    if (openCount == 0) {
        showMessage("Port Scan", aborted ? "Scan aborted" : "No open ports found",
                    targetStr.c_str(), CLR_GRAY);
        return;
    }

    int scroll = 0;
    const int VIS = 5;

    auto drawResults = [&]() {
        lcd.fillScreen(CLR_BG);
        _netTopBar("PORT SCAN");
        lcd.setTextDatum(middle_left);
        lcd.setTextColor(0x630C);
        lcd.setFont(&fonts::DejaVu9);
        lcd.drawString(targetStr + " — " + openCount + " open" + (aborted ? " (aborted)" : ""), 8, 26);
        for (int i = scroll; i < min(openCount, scroll + VIS); i++) {
            lcd.setTextColor(CLR_GREEN);
            lcd.drawString(String(openPort[i]) + "  " + openName[i], 12, 38 + (i - scroll) * 14);
        }
        _netStatusBar("  ▲ up  ▼ down  s save  ESC back");
    };

    drawResults();

    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            for (auto ch : M5Cardputer.Keyboard.keysState().word) {
                uint8_t c = (uint8_t)ch;
                if (c == 0x60 || c == 0x1B)                     return;
                if (c == 0x3B && scroll > 0)                    { scroll--; drawResults(); }
                if (c == 0x2E && scroll < openCount - VIS)      { scroll++; drawResults(); }
                if (c == 's') {
                    String rows[_NPORTS];
                    String hdr = "target,port,service\n" + targetStr;
                    for (int i = 0; i < openCount; i++)
                        rows[i] = targetStr + "," + openPort[i] + "," + openName[i];
                    String path = sdWriteCsv("ports", "target,port,service", rows, openCount);
                    if (path.isEmpty()) showMessage("Save CSV", "SD card error", "Check SD card", 0xF800);
                    else                showMessage("Save CSV", "Saved!", path.c_str(), CLR_GREEN);
                    drawResults();
                }
            }
        }
        delay(20);
    }
}

// ── DNS Lookup ───────────────────────────────────────────────

static void _netDnsLookup() {
    if (!_netWifiCheck()) return;

    String hostname = readTextInput("Hostname");
    if (hostname.isEmpty()) return;

    IPAddress ip;
    if (WiFi.hostByName(hostname.c_str(), ip)) {
        showMessage("DNS Lookup", hostname.c_str(), ip.toString().c_str(), CLR_GREEN);
    } else {
        showMessage("DNS Lookup", "Resolution failed", hostname.c_str(), 0xF800);
    }
}

// ── Entry point ──────────────────────────────────────────────

void runNetwork() {
    const char* items[] = { "Host Discovery", "Port Scan", "DNS Lookup" };
    while (true) {
        int sel = drawSubMenu("NETWORK", items, 3);
        if (sel < 0) return;
        switch (sel) {
            case 0: _netHostDiscovery(); break;
            case 1: _netPortScan();      break;
            case 2: _netDnsLookup();     break;
        }
    }
}
