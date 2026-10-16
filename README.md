# Laboratory Activity 8: Wired Communication

Verifying the three wired interfaces of an ESP32 (WROOM-32) board: **UART**, **I²C**, and **SPI**.

**Materials:** ESP32 (WROOM-32), USB cable, breadboard, jumper wires, one I²C module (4-pin SSD1306 OLED, pins labeled GND / VDD / SCK / SDA), computer with the Arduino IDE.

Sketches:

| Folder | What it does |
|---|---|
| `uart_loopback/` | Sends `PING` through `Serial2` once per second and prints every received byte |
| `i2c_scanner/` | Scans addresses 0x01–0x7E once at startup |
| `spi_loopback/` | Sends `0xA5` once per second with CS framing and prints the received byte |

---

## 1. Pin table

| Signal | GPIO used | Why this pin |
|---|---|---|
| UART TX | **17** | `Serial2` TX default; normal I/O pin, not strapping, not flash |
| UART RX | **16** | `Serial2` RX default; normal I/O pin (reserved for PSRAM on WROVER, so a WROOM-32 is required) |
| I²C SDA | **32** | General-purpose, output-capable, not strapping |
| I²C SCL | **33** | General-purpose, output-capable, not strapping |
| SPI SCK | **14** | HSPI clock pin, not flash, not input-only |
| SPI MOSI | **13** | HSPI MOSI pin, not flash, not input-only |
| SPI MISO | **26** | General-purpose; used as the loopback input |
| SPI CS | **4** | General-purpose output, not strapping |

Avoided on purpose: GPIO 0, 2, 5, 12, 15 (strapping), GPIO 6–11 (onboard flash), GPIO 34–39 (input-only, cannot send signals). Every signal has its own pin, with no sharing.

---

## 2. Wiring

```
UART loopback                    SPI loopback
  ESP32 GPIO17 (UART TX) ──┐       ESP32 GPIO13 (SPI MOSI) ──┐
                           │ jumper                          │ jumper
  ESP32 GPIO16 (UART RX) ──┘       ESP32 GPIO26 (SPI MISO) ──┘

                                   ESP32 GPIO14 (SPI SCK)  ── (not connected, loopback only)
                                   ESP32 GPIO4  (SPI CS)   ── (not connected, loopback only)

I²C module (OLED)
  OLED GND ──────── ESP32 GND
  OLED VDD ──────── ESP32 3V3        (powered from 3.3 V, not 5 V)
  OLED SCK (= SCL) ─ ESP32 GPIO33 (I²C SCL)
  OLED SDA ──────── ESP32 GPIO32 (I²C SDA)
```

> The OLED's pin labeled **SCK** is the I²C clock (SCL). It goes to GPIO33, **not** to the SPI SCK pin (GPIO14).

**Wiring photo / diagram:** _insert your labeled photo here, e.g. `![wiring](wiring.jpg)`_

---

## 3. Expected vs. observed

Predictions were written before running each test.

| Test | Expected | Observed |
|---|---|---|
| UART with jumper | `PING` printed once per second | _fill in_ |
| UART without jumper | No new lines appear | _fill in_ |
| I²C with module | `Found device at 0x3C` (typical for SSD1306 OLED; could be 0x3D) | _fill in_ |
| I²C without module | `No I2C devices found` | _fill in_ |
| SPI with jumper | `Sent 0xA5, received 0xA5` every second | _fill in_ |
| SPI without jumper | `received 0xFF` (floating input, may vary) | _fill in_ |
| Sketch at 9600, monitor at 115200 | Garbled / unreadable characters (baud mismatch) | _fill in_ |
| Sketch at 9600, monitor at 9600 | `PING` readable again once per second | _fill in_ |

---

## 4. Serial Monitor output (115200 baud)

### UART with jumper
```
(paste output here)
```

### UART without jumper
```
(paste output here)
```

### I²C with module
```
(paste output here)
```

### I²C without module
```
(paste output here)
```

### SPI with jumper
```
(paste output here)
```

### SPI without jumper
```
(paste output here)
```

### Baud-rate mismatch (sketch `Serial.begin(9600)`, monitor 115200)
```
(paste output here)
```

### Baud rate matched (sketch 9600, monitor 9600)
```
(paste output here)
```

Sketch 1 was restored to `Serial.begin(115200)` afterwards.

---

## 5. Questions

**1. Why does UART need both sides to use the same baud rate, while SPI uses a clock line?**
UART is asynchronous: there is no shared clock, so each side times the bits with its own timer and must agree in advance on how long one bit lasts. SPI's master sends a clock along with the data, so the receiver samples exactly on the clock edges and no speed agreement is needed.

**2. Why can two devices with the same address not share one I²C bus?**
Every device on the bus hears every address byte, and the controller picks a device only by its address. If two devices share an address, both acknowledge and both drive SDA at the same time, so the controller cannot tell them apart and the data gets corrupted.

**3. Why is CS set HIGH between SPI transfers?**
CS HIGH deselects the slave, so it ignores the clock and releases the MISO line instead of driving it. The slave also resets its internal shift register, so the next CS-LOW starts a fresh, byte-aligned transfer.
