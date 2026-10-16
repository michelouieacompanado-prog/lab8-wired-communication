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

### Connections

| # | From (ESP32 pin) | To | Purpose |
|---|---|---|---|
| 1 | GPIO17 (UART TX) | GPIO16 (UART RX) | UART loopback jumper |
| 2 | GPIO13 (SPI MOSI) | GPIO26 (SPI MISO) | SPI loopback jumper |
| 3 | GPIO32 (I²C SDA) | OLED **SDA** | I²C data |
| 4 | GPIO33 (I²C SCL) | OLED **SCK** (this is the I²C clock, SCL) | I²C clock |
| 5 | 3V3 | OLED **VDD** (VCC) | Power, 3.3 V |
| 6 | GND | OLED **GND** | Ground |
| – | GPIO14 (SPI SCK), GPIO4 (SPI CS) | no wire | Driven by the sketch, but nothing is attached in loopback mode |

> The OLED's pin labeled **SCK** is the I²C clock (SCL). It goes to GPIO33, **not** to the SPI SCK pin (GPIO14).

### Wiring diagram

![Labeled wiring diagram](images/wiring_diagram.svg)

### Labeled photo of my build

![Labeled photo of my circuit](images/wiring.jpg)

In the photo, every wire is labeled with its GPIO number and signal name: the UART jumper (GPIO17 TX to GPIO16 RX), the SPI jumper (GPIO13 MOSI to GPIO26 MISO), and the OLED wires (SDA to GPIO32, SCK/SCL to GPIO33, VDD to 3V3, GND to GND).

---

## 3. Expected vs. observed

Predictions were written before running each test.

| Test | Expected | Observed |
|---|---|---|
| UART with jumper | `PING` printed once per second | `PING` appeared on a new line about once every second, so the TX-to-RX jumper looped the data back correctly. |
| UART without jumper | No new lines appear | No new `PING` lines appeared after the jumper was removed. |
| I²C with module | `Found device at 0x3C` (typical for SSD1306 OLED; could be 0x3D) | `Found device at 0x3C`: the OLED acknowledged its address. |
| I²C without module | `No I2C devices found` | `No I2C devices found`: no address acknowledged. |
| SPI with jumper | `Sent 0xA5, received 0xA5` every second | `Sent 0xA5, received 0xA5` every second: the byte sent on MOSI came back on MISO unchanged. |
| SPI without jumper | `received 0xFF` (floating input, may vary) | `Sent 0xA5, received 0xFF`: MISO was floating and read as all ones, not the byte that was sent. |
| Sketch at 9600, monitor at 115200 | Garbled / unreadable characters (baud mismatch) | Unreadable garbage characters appeared instead of `PING`, because the sketch's 9600 baud did not match the monitor's 115200. |
| Sketch at 9600, monitor at 9600 | `PING` readable again once per second | `PING` was readable again, once per second, after the monitor was set to 9600. |

---

## 4. Serial Monitor output and what it shows (115200 baud unless stated)

### UART with jumper
```
<img width="1366" height="768" alt="uart_loopback - wired" src="https://github.com/user-attachments/assets/1f718c87-f2cc-4705-9cd9-5dbb74c62880" />
```
`Serial2` sends `PING` plus a line break at 9600 baud from TX (GPIO17) every second. The jumper feeds that signal straight into RX (GPIO16), so `Serial2.available()` becomes non-zero and the sketch copies every received byte to the USB serial port (115200 baud), where the monitor shows it. Two UARTs are in use here: `Serial2` (9600 baud, between the two pins) and `Serial` (115200 baud, to the computer). The output proves the board can both transmit and receive.

### UART without jumper
```
<img width="1366" height="768" alt="uart_loopback - NOTwired" src="https://github.com/user-attachments/assets/e23d2a06-77f9-45c0-b74f-dfba3833f27c" />
```
`Serial2` still transmits on GPIO17, but nothing reaches GPIO16, so no bytes are ever received and nothing is printed. This confirms the earlier `PING` lines really travelled through the jumper wire and were not generated inside the program.

### I²C with module
```
(paste output here)
```
The scanner tries every address from 0x01 to 0x7E. A device that is present pulls SDA low during the acknowledge bit, and `endTransmission()` then returns 0. Address `0x3C` (60 in decimal) is the usual address of an SSD1306 OLED. Only one address replied, so there is one device on the bus, and the wiring (SDA on GPIO32, SCL on GPIO33, power on 3V3) is correct.

### I²C without module
```
(paste output here)
```
With the module removed, no device acknowledges any address, so `endTransmission()` never returns 0, the counter stays at zero, and the sketch prints `No I2C devices found`. This is the expected "nothing connected" result.

### SPI with jumper
```
<img width="1366" height="768" alt="spi_loopback - wired" src="https://github.com/user-attachments/assets/cba82e5e-c686-468c-b95a-e71c21aaf8da" />
```
With CS pulled LOW, `SPI.transfer(0xA5)` shifts the byte out on MOSI (GPIO13), most-significant bit first, while SCK clocks eight pulses at 1 MHz. Because MOSI is wired to MISO (GPIO26), the same bits come straight back in during the same eight clocks, so the received byte is `0xA5` (binary `10100101`). Sent equals received, which shows SPI is full duplex.

### SPI without jumper
```
<img width="1366" height="768" alt="spi_loopback - NOTwired" src="https://github.com/user-attachments/assets/1e400131-df9d-470f-b4ae-636fe7da730f" />
```
With the jumper removed, MISO (GPIO26) is not driven by anything and floats. The input then reads as all ones or as noise, which is why `0xFF` (or another unexpected value) shows up instead of `0xA5`. A value that matches only when the jumper is present confirms the loopback is real.

### Baud-rate mismatch (sketch `Serial.begin(9600)`, monitor 115200)
```
<img width="1366" height="768" alt="uart_loopback - sketch(115200) baud(9600)" src="https://github.com/user-attachments/assets/9a090fe4-9d69-46c1-8b6b-b54397559890" />
```
The sketch now sends to the computer at 9600 baud, but the monitor listens at 115200, which is 12 times faster. The monitor samples each bit at the wrong moments and reads wrong values, so the text turns into garbage characters.

### Baud rate matched (sketch 9600, monitor 9600)
```
<img width="1363" height="765" alt="uart_loopback - sketch(9600) baud(9600)" src="https://github.com/user-attachments/assets/676641c3-66e5-4efb-bcb1-2b82e8e9aab4" />
```
Once both sides use 9600 baud, they agree on how long each bit lasts and `PING` is readable again. This demonstrates the answer to Question 1 below.

Sketch 1 was restored to `Serial.begin(115200)` afterwards.

---

## 5. Questions

**1. Why does UART need both sides to use the same baud rate, while SPI uses a clock line?**
UART is asynchronous: there is no shared clock, so each side times the bits with its own timer and must agree in advance on how long one bit lasts. SPI's master sends a clock along with the data, so the receiver samples exactly on the clock edges and no speed agreement is needed.

**2. Why can two devices with the same address not share one I²C bus?**
Every device on the bus hears every address byte, and the controller picks a device only by its address. If two devices share an address, both acknowledge and both drive SDA at the same time, so the controller cannot tell them apart and the data gets corrupted.

**3. Why is CS set HIGH between SPI transfers?**
CS HIGH deselects the slave, so it ignores the clock and releases the MISO line instead of driving it. The slave also resets its internal shift register, so the next CS-LOW starts a fresh, byte-aligned transfer.
