# ⚡ ORUAM BSS — Basic Security Suite

> A portable security toolkit for the **M5Stack Core2 ADV**, built on ESP32.  
> Developed by [@juliomauro](https://github.com/juliomauro) · Codename: **Oruam Oliuj**

---

## 📖 About

**ORUAM BSS** is a custom firmware for the M5Stack Core2 ADV that turns the device into a compact, handheld security reconnaissance tool. The project explores the real limits of what an ESP32-based device can do in the field of network and wireless security — no Linux, no external SBCs, pure embedded C++.

The name carries a double meaning: **ORUAM** is the author's security codename, and **BSS** (*Basic Security Suite*) is also a reference to the 802.11 wireless term *Basic Service Set* — a nod to the WiFi-heavy nature of the tool.

**Stack:** C++ · PlatformIO · Arduino Core · M5Unified · LovyanGFX

---

## 🖥️ Hardware

| Component | Spec |
|---|---|
| Platform | M5Stack Core2 ADV |
| SoC | ESP32-D0WDQ6-V3 (dual-core Xtensa LX6, 240MHz) |
| RAM | 520KB SRAM + 8MB PSRAM |
| Flash | 16MB |
| Display | 2" IPS TFT · 320×240 · ILI9342C · Capacitive touch |
| Wireless | WiFi 802.11 b/g/n (2.4GHz) · Bluetooth 4.2 / BLE |
| Storage | MicroSD card |

---

## 🔧 Planned Modules

### 📡 WiFi
- [ ] Network Scanner — SSID, BSSID, channel, RSSI, encryption type
- [ ] Beacon Sniffer — passive 802.11 management frame capture (promiscuous mode)
- [ ] Deauth Detector — detect deauthentication attacks in the air
- [ ] Evil Twin Detector — identify rogue APs spoofing known SSIDs

### 🌐 Network (TCP/IP)
- [ ] Host Discovery — ARP sweep on local subnet
- [ ] Port Scanner — TCP connect scan with configurable range
- [ ] Banner Grabber — HTTP, SSH, FTP, Telnet, SNMP service fingerprinting
- [ ] SSL Inspector — TLS certificate info and cipher inspection
- [ ] DNS Lookup / Reverse DNS
- [ ] MAC Vendor Lookup — OUI database stored on SD card

### 🔵 Bluetooth
- [ ] BLE Scanner — device discovery, RSSI, UUIDs, advertised name
- [ ] GATT Explorer — browse services and characteristics of BLE devices
- [ ] BLE Advertiser Spoof Detector

### 🔌 Hardware / Physical
- [ ] I2C Scanner — detect devices on the I2C bus via Grove/GPIO
- [ ] UART Monitor — serial bus sniffer via GPIO pins
- [ ] GPIO Logic Probe — basic digital signal inspection

### 🔐 Crypto / Utils
- [ ] Hash Calculator — MD5 / SHA1 / SHA256 (hardware accelerated)
- [ ] Base64 Encoder / Decoder
- [ ] Entropy Analyzer — detect encrypted or compressed data streams
- [ ] JWT Decoder — inspect JSON Web Token payloads

---

## ✅ Implementation Status

### Core / UI
- [ ] PlatformIO project structure
- [ ] M5Unified initialization
- [ ] Graphical menu system (icon grid, highlight, navigation)
- [ ] Status bar (IP, battery, SD)
- [ ] Button handler (A / B / C)
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

## 🚀 Development Setup

```bash
# Clone the repository
git clone https://github.com/juliomauro/oruam-bss.git
cd oruam-bss

# Open in VS Code with PlatformIO extension installed
code .

# Build
pio run

# Upload to M5Stack Core2 ADV
pio run --target upload

# Monitor serial output
pio device monitor
```

### Dependencies

```ini
; platformio.ini
[env:m5stack-core2]
platform    = espressif32
board       = m5stack-core2
framework   = arduino

lib_deps =
    m5stack/M5Unified
    lovyan03/LovyanGFX
```

---

## ⚠️ Disclaimer

This tool is intended for **authorized security assessments, educational purposes, and personal research only**. Do not use against networks or devices you do not own or have explicit permission to test. The author takes no responsibility for misuse.

---

## 📄 License

Apache License 2.0 — see [LICENSE](LICEN

---

*"Not every hack needs a Raspberry Pi."*
