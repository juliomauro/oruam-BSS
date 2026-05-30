# ORUAM BSS — Basic Security Suite

> A portable security toolkit for the **M5Cardputer ADV**, built on ESP32-S3.  
> Developed by [@juliomauro](https://github.com/juliomauro) · Codename: **KIRK**

---

## Preview

| Boot Splash | Main Menu |
|---|---|
| ![Boot Splash](assets/preview-splash.png) | ![Main Menu](assets/preview-menu.png) |

> Screenshots generated from `preview.html` — pixel-accurate render of the 240×135 display.

---

## About

**ORUAM BSS** is a custom firmware for the M5Cardputer ADV that turns the device into a compact, handheld security reconnaissance tool. The project explores what an ESP32-S3-based device can do in the field of network and wireless security — no Linux, no external SBCs, pure embedded C++.

The name carries a double meaning: **ORUAM** is the author's security codename, and **BSS** (*Basic Security Suite*) is also a reference to the 802.11 wireless term *Basic Service Set* — a nod to the WiFi-heavy nature of the tool.

**Stack:** C++ · PlatformIO · Arduino Core · M5Unified

---

## Hardware

| Component | Spec |
|---|---|
| Platform | M5Cardputer ADV |
| SoC | ESP32-S3FN8 (dual-core Xtensa LX7, 240MHz) |
| RAM | 512KB SRAM + 8MB PSRAM |
| Flash | 8MB |
| Display | 1.14" IPS TFT · 240×135 · ST7789 |
| Input | Full QWERTY keyboard + physical arrow keys |
| Wireless | WiFi 802.11 b/g/n (2.4GHz) · Bluetooth 5.0 / BLE |
| Extra | Microphone · IR transmitter · Module expansion slot |

---

## Navigation

| Key | Action |
|---|---|
| `◄` `▲` `►` | Navigate menu grid |
| `ok` | Select module |
| Side button | Next item (fallback) |

---

## Planned Modules

### WiFi
- [ ] Network Scanner — SSID, BSSID, channel, RSSI, encryption type
- [ ] Beacon Sniffer — passive 802.11 management frame capture (promiscuous mode)
- [ ] Deauth Detector — detect deauthentication attacks in the air
- [ ] Evil Twin Detector — identify rogue APs spoofing known SSIDs

### Network (TCP/IP)
- [ ] Host Discovery — ARP sweep on local subnet
- [ ] Port Scanner — TCP connect scan with configurable range
- [ ] Banner Grabber — HTTP, SSH, FTP, Telnet service fingerprinting
- [ ] SSL Inspector — TLS certificate info and cipher inspection
- [ ] DNS Lookup / Reverse DNS
- [ ] MAC Vendor Lookup — OUI database stored on SD card

### Bluetooth
- [ ] BLE Scanner — device discovery, RSSI, UUIDs, advertised name
- [ ] GATT Explorer — browse services and characteristics of BLE devices

### Hardware / Physical
- [ ] I2C Scanner — detect devices on the I2C bus
- [ ] UART Monitor — serial bus sniffer via GPIO pins

### Crypto / Utils
- [ ] Hash Calculator — MD5 / SHA1 / SHA256 (hardware accelerated)
- [ ] Base64 Encoder / Decoder
- [ ] Entropy Analyzer — detect encrypted or compressed data streams
- [ ] JWT Decoder — inspect JSON Web Token payloads

---

## Implementation Status

### Core / UI
- [x] PlatformIO project structure
- [x] M5Unified initialization (M5Cardputer board target)
- [x] Boot splash screen with animated loading bar
- [x] Graphical menu system — 3×2 icon grid with highlight and navigation
- [x] Arrow key navigation (◄ ▲ ►) + side button fallback
- [ ] Status bar (IP, battery, SD)
- [ ] SD card logging

### Modules
- [ ] WiFi — Network Scanner
- [ ] WiFi — Beacon Sniffer
- [ ] WiFi — Deauth Detector
- [ ] WiFi — Evil Twin Detector
- [ ] Network — Host Discovery (ARP)
- [ ] Network — Port Scanner
- [ ] Network — Banner Grabber
- [ ] Network — SSL Inspector
- [ ] Network — DNS Lookup
- [ ] Network — MAC Vendor Lookup
- [ ] Bluetooth — BLE Scanner
- [ ] Bluetooth — GATT Explorer
- [ ] Hardware — I2C Scanner
- [ ] Hardware — UART Monitor
- [ ] Crypto — Hash Calculator
- [ ] Crypto — Base64
- [ ] Crypto — Entropy Analyzer
- [ ] Crypto — JWT Decoder

---

## Development Setup

```bash
git clone https://github.com/juliomauro/oruam-BSS.git
cd oruam-BSS
code .
```

Requires [PlatformIO](https://platformio.org/) installed in VS Code.

```bash
# Build
pio run

# Upload to M5Cardputer
pio run --target upload

# Monitor serial output
pio device monitor
```

### platformio.ini

```ini
[env:m5stack-cardputer]
platform  = espressif32
board     = m5stack-stamps3
framework = arduino

build_flags =
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DBOARD_HAS_PSRAM

lib_deps =
    m5stack/M5Unified @ ^0.2.2
```

### Arrow key calibration

The physical arrow key codes depend on the M5Unified firmware version. If navigation does not respond, add this to `loop()` temporarily to read the real codes:

```cpp
if (M5.Keyboard.isChange() && M5.Keyboard.isPressed()) {
    for (auto ch : M5.Keyboard.keysState().word)
        Serial.printf("key: 0x%02X\n", (uint8_t)ch);
}
```

Then update the `switch` cases in `include/menu.h` with the values printed to serial.

---

## Disclaimer

This tool is intended for **authorized security assessments, educational purposes, and personal research only**. Do not use against networks or devices you do not own or have explicit permission to test. The author takes no responsibility for misuse.

---

## License

Apache License 2.0 — see [LICENSE](LICENSE)

---

*"Not every hack needs a Raspberry Pi."*
