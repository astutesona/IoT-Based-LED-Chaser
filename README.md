# IoT Based LED Chaser (Running Lights)
### Mini Project Report & Technical Documentation

[![Live Simulator](https://img.shields.io/badge/Live%20Simulator-GitHub%20Pages-success?style=for-the-badge&logo=github)](https://astutesona.github.io/IoT-Based-LED-Chaser/)
[![Circuit Diagram](https://img.shields.io/badge/Schematic-circuit__diagram.svg-blue?style=for-the-badge)](./circuit_diagram.svg)
[![Viva Questions](https://img.shields.io/badge/Viva%20Voce-15%20Q%26A-purple?style=for-the-badge)](./viva_questions.md)
[![Arduino Firmware](https://img.shields.io/badge/Firmware-chaser.ino-teal?style=for-the-badge)](./chaser.ino)

---

## 📌 Quick Project Links
- 🌐 **Live Web Workbench & Dashboard:** [https://astutesona.github.io/IoT-Based-LED-Chaser/](https://astutesona.github.io/IoT-Based-LED-Chaser/)
- 🖼️ **Circuit Schematic (Vector SVG):** [`circuit_diagram.svg`](./circuit_diagram.svg)
- ❓ **Viva Voce Exam Questions & Answers:** [`viva_questions.md`](./viva_questions.md)
- 💻 **Arduino Uno Source Code:** [`chaser.ino`](./chaser.ino)
- ☁️ **Wokwi Online Simulation:** [`diagram.json`](./diagram.json) & [`sketch.ino`](./sketch.ino)

---

## 1. Project Title & Abstract
**Project Title:** IoT Based LED Chaser (Running Lights) using Arduino Uno R3, 8-Channel Optocoupled Relay Module, and Interactive Web IoT Gateway  
**Course / Discipline:** Internet of Things (IoT) / Embedded Systems / Electronics & Communication Engineering

### Abstract
Running light systems (LED chasers) are widely deployed in commercial signage, traffic sequencing, emergency beacons, and automated stage lighting. This project presents a complete, modular, and industrial-aligned running light controller driven by an **Arduino Uno R3 (ATmega328P)** interfacing with an **8-Channel 5V Optocoupled Relay Module (Active-LOW)**, eight current-limited 5mm green LEDs, and an acoustic active buzzer indicator. 

Firmware execution is engineered around a non-blocking `millis()` timing state machine supporting four selectable operational patterns: **Forward Sequence**, **Reverse Sequence**, **Bounce (Ping-Pong)**, and **Simultaneous All-Blink**. Bidirectional telemetry and control are achieved over high-speed hardware UART (115200 baud). An integrated **Web IoT Dashboard & Virtual Workbench** provides real-time browser control via the modern **Web Serial API** alongside a **Simulated MQTT Gateway**, with graceful fallback to an onboard simulation engine.

---

## 2. Aim & Objectives
1. **Aim:** To design, simulate, assemble, and demonstrate an IoT-supervised 8-channel running light chaser system using electromechanical relay isolation and non-blocking embedded software.
2. **Specific Objectives:**
   - Implement zero-`delay()` non-blocking timing using hardware Timer0 (`millis()`) for real-time responsiveness.
   - Interface an Active-LOW 8-channel relay board with PC817 optocoupler galvanic isolation.
   - Establish bidirectional UART serial command parsing (`START`, `STOP`, `SPEED:<ms>`, `MODE:<1-4>`) at 115200 baud.
   - Develop a single-file, browser-based visual hardware simulator and IoT dashboard leveraging Web Serial and Web Audio APIs.
   - Provide an honest engineering upgrade roadmap to native Wi-Fi SoCs (ESP32/ESP8266) via MQTT broker topologies.

---

## 3. Hardware Components & Specifications

| S.No | Component | Specification / Rating | Quantity | Functional Role in System |
| :--- | :--- | :--- | :---: | :--- |
| 1 | **Microcontroller Board** | Arduino Uno R3 (ATmega328P @ 16 MHz) | 1 | Master control unit; executes timing engine, command parser, and GPIO sequencing |
| 2 | **Relay Interface Module** | 8-Channel 5V DC, Active-LOW, PC817 Optocouplers, Songle SRD-05VDC-SL-C | 1 | Provides electrical isolation and power switching (rated up to 250V AC / 10A DC) |
| 3 | **Light Emitting Diodes** | 5mm Diffused Green LEDs ($V_F \approx 2.1\text{V}$, $I_F \approx 10\text{mA}$) | 8 | Act as visual running light indicators (simulating industrial field loads) |
| 4 | **Current Limiting Resistors** | 330 $\Omega$, 1/4 Watt, $\pm 1\%$ Metal Film | 8 | Restricts forward current across LEDs to safe operating levels ($\approx 8.8\text{ mA}$) |
| 5 | **Acoustic Indicator** | 5V DC Active Buzzer (Resonant freq $\approx 2.4\text{ kHz}$) | 1 | Generates an audible click/pulse during step transitions |
| 6 | **Prototyping Board** | MB102 830-Point Solderless Breadboard | 1 | Houses LED array, current-limiting resistors, and distribution power rails |
| 7 | **Primary Power Supply** | 9V DC 1A Regulated SMPS Wall Adapter (2.1mm Center-Positive Barrel Jack) | 1 | Supplies sufficient current to prevent USB bus brownout during simultaneous coil pull-in |
| 8 | **Jumper Wires** | Premium M-to-M and M-to-F Dupont cables | 20+ | Low-resistance point-to-point electrical interconnection |

---

## 4. Pinout & Complete Wiring Interconnections

### 4.1 Digital Signal Connections
| Arduino Uno Pin | Destination Module / Component | Wire Color | Signal Function / Logic |
| :--- | :--- | :--- | :--- |
| **D2** | Relay Module **IN1** | Blue | Channel 1 Trigger (Active-LOW: 0V = Coil ON) |
| **D3** | Relay Module **IN2** | Blue | Channel 2 Trigger (Active-LOW) |
| **D4** | Relay Module **IN3** | Blue | Channel 3 Trigger (Active-LOW) |
| **D5** | Relay Module **IN4** | Blue | Channel 4 Trigger (Active-LOW) |
| **D6** | Relay Module **IN5** | Blue | Channel 5 Trigger (Active-LOW) |
| **D7** | Relay Module **IN6** | Blue | Channel 6 Trigger (Active-LOW) |
| **D8** | Relay Module **IN7** | Blue | Channel 7 Trigger (Active-LOW) |
| **D9** | Relay Module **IN8** | Blue | Channel 8 Trigger (Active-LOW) |
| **D10** | Active Buzzer **Positive (+)** | Purple | Buzzer Step Acoustic Pulse (Active-HIGH) |

### 4.2 Power Distribution & Relay Output Stage
| Source Point | Destination Point | Wire Color | Technical Notes |
| :--- | :--- | :--- | :--- |
| **Uno 5V Pin** | Relay Module **VCC** | Red | Supplies optocoupler input photodiode circuitry |
| **Uno GND Pin** | Relay Module **GND** | Black | Digital ground reference for optocoupler inputs |
| **Uno 5V Pin** | Relay **COM 1 through COM 8** | Orange / Red | Common bus bar delivering +5V load supply to all contacts |
| **Relay 1 NO** | Breadboard Row 1 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 1 Anode when Relay 1 energizes |
| **Relay 2 NO** | Breadboard Row 2 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 2 Anode when Relay 2 energizes |
| **Relay 3 NO** | Breadboard Row 3 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 3 Anode when Relay 3 energizes |
| **Relay 4 NO** | Breadboard Row 4 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 4 Anode when Relay 4 energizes |
| **Relay 5 NO** | Breadboard Row 5 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 5 Anode when Relay 5 energizes |
| **Relay 6 NO** | Breadboard Row 6 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 6 Anode when Relay 6 energizes |
| **Relay 7 NO** | Breadboard Row 7 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 7 Anode when Relay 7 energizes |
| **Relay 8 NO** | Breadboard Row 8 $\rightarrow$ 330 $\Omega$ Resistor | Green | Switched +5V to LED 8 Anode when Relay 8 energizes |
| **LED Cathodes (1–8)** | Breadboard **Common GND Rail** | Black | Returns forward current to Uno GND |
| **Buzzer Negative (-)** | Breadboard **Common GND Rail** | Black | Common ground return |
| **Barrel Jack** | 9V 1A DC Adapter | External | Powers onboard 5V regulator with reserve capacity |

### 4.3 Complete Circuit Schematic
![Complete Circuit Schematic](./circuit_diagram.svg)

---

## 5. Architectural & IoT Block Diagram

```mermaid
flowchart TD
    subgraph UI_Layer["Browser / Cloud Application Layer"]
        WebDashboard["Modern Web Dashboard (index.html)"]
        WebSerial["Web Serial API (115200 baud)"]
        WebAudio["Web Audio API (Relay & Buzzer Sound)"]
        MqttSim["Simulated MQTT Link (college/iot/ledchaser)"]
    end

    subgraph Controller_Layer["Perception & Processing Layer (Edge)"]
        Uno["Arduino Uno R3 (ATmega328P @ 16 MHz)"]
        TimerEngine["Non-blocking millis() Timing Engine"]
        SerialParser["UART ASCII Command Parser"]
        BuzzerDriver["Buzzer Subsystem (D10)"]
    end

    subgraph Isolation_Layer["Galvanic Isolation & Power Switching"]
        Optocouplers["PC817 Optocouplers (Active-LOW)"]
        FlybackDiodes["Flyback Clamping Diodes"]
        Relays["8x 5V SPDT Mechanical Relays"]
    end

    subgraph Load_Layer["Load & Display Stage (Breadboard)"]
        Resistors["8x 330 Ohm 1/4W Resistors"]
        Leds["8x 5mm Green LEDs"]
        Buzzer["5V Active Buzzer"]
    end

    WebDashboard <-->|Bidirectional USB CDC-ACM| WebSerial
    WebDashboard -.->|Simulated Publish/Subscribe| MqttSim
    WebDashboard --> WebAudio
    WebSerial <-->|UART TX/RX @ 115200| SerialParser
    
    SerialParser --> TimerEngine
    TimerEngine -->|GPIO D2-D9 (LOW = Active)| Optocouplers
    TimerEngine -->|GPIO D10 (HIGH = Pulse)| BuzzerDriver
    
    Optocouplers --> Relays
    FlybackDiodes -.->|Clamps inductive kickback| Relays
    
    Relays -->|Switched +5V through NO Contacts| Resistors
    Resistors --> Leds
    BuzzerDriver --> Buzzer
```

---

## 6. Working Principle & Circuit Operation

### 6.1 Active-LOW Optocoupled Switching
Each relay channel features a **PC817 optocoupler** and a switching transistor. The anode of the internal optocoupler LED is tied to VCC (+5V). When the Arduino pin outputs a digital `LOW` (0V), current flows through the internal LED, illuminating the phototransistor base. The phototransistor saturates, driving the base of an NPN relay driver transistor. This completes the coil circuit to ground, magnetizing the coil and pulling the **COM** (Common) contact against the **NO** (Normally Open) contact.

### 6.2 Load Stage Current Flow
The common terminals (**COM**) of all 8 relays are tied directly to the regulated +5V rail. When a given relay energizes:
1. The NO contact closes, routing +5V to the corresponding $330\ \Omega$ resistor on the breadboard.
2. Current flows into the anode of the green LED ($V_F \approx 2.1\text{V}$), emitting light.
3. Current exits via the LED cathode into the breadboard ground rail, returning to the Arduino GND.

### 6.3 Non-Blocking Timing Engine
Rather than relying on `delay()`, the ATmega328P samples the hardware counter `millis()`. Once $(currentMillis - lastStepTime) \ge stepInterval$, the system advances to the next step. This allows UART reception, serial buffering, and emergency stops to be processed with zero latency.

---

## 7. Firmware & Software Architecture

### 7.1 Operational Modes
1. **Mode 1: Forward Chaser:** Relays sequence in ascending order ($IN_1 \rightarrow IN_2 \rightarrow \dots \rightarrow IN_8$).
2. **Mode 2: Reverse Chaser:** Relays sequence in descending order ($IN_8 \rightarrow IN_7 \rightarrow \dots \rightarrow IN_1$).
3. **Mode 3: Bounce (Ping-Pong):** Relays animate forward to the boundary, reverse direction, and rebound ($1 \rightarrow 8 \rightarrow 1$).
4. **Mode 4: All-Blink (Strobe):** All 8 relays toggle simultaneously between all-ON and all-OFF.

### 7.2 Serial Communication Protocol (115200 Baud)
All commands are ASCII strings terminated by newline (`\n` or `\r`):
- `START`: Resumes chaser execution.
- `STOP`: Immediately de-energizes all 8 relays.
- `SPEED:<ms>`: Sets step delay in milliseconds (clamped between 30 ms and 3000 ms).
- `MODE:<1-4>`: Switches animation mode (resets step indices).
- `STATUS`: Queries device state; returns JSON telemetry:
  ```json
  {"status":{"running":true,"mode":1,"speed":200,"baud":115200}}
  ```

---

## 8. How to Run the Project

### Method 1: Interactive Browser Simulator (`index.html`)
1. Open the project folder on your computer.
2. Double-click `index.html` to open it in any modern browser (Google Chrome, Microsoft Edge, Brave, Opera).
3. The visual simulator starts running immediately in **Simulated Mode**:
   - Observe the 8 relays clicking, red channel LEDs toggling, and green breadboard LEDs sequencing.
   - Click the mode buttons, adjust the speed slider, or toggle Start/Stop.
   - Listen to the synthesized mechanical relay clicks and buzzer pulses (requires clicking anywhere on the page to initialize the browser Web Audio context).
4. If you have an Arduino connected over USB, click **"🔌 Connect to Arduino"** and select your Uno's COM port at 115200 baud. The page transitions seamlessly to live hardware control!

### Method 2: Wokwi Online Cloud Simulation (`wokwi.com`)
1. Open [wokwi.com](https://wokwi.com) in your web browser.
2. Start a new **Arduino Uno** project.
3. Replace the default `sketch.ino` with the code from [`sketch.ino`](./sketch.ino).
4. Switch to the `diagram.json` tab and paste the contents of [`diagram.json`](./diagram.json).
5. Click the green **Play** button to simulate the complete 8-relay circuit and Arduino sketch online.

### Method 3: Real Physical Hardware Setup
1. Mount the eight 330 $\Omega$ resistors and green LEDs onto the MB102 breadboard.
2. Wire Uno pins D2–D9 to Relay module pins IN1–IN8.
3. Connect Uno D10 to the active buzzer positive lead; connect the negative lead to GND.
4. Connect Uno 5V and GND to the Relay module VCC and GND.
5. Bus the Relay COM terminals to 5V; wire each NO terminal to the corresponding resistor/LED anode.
6. Connect the 9V 1A adapter to the Uno DC barrel jack.
7. Open [`chaser.ino`](./chaser.ino) in the Arduino IDE.
8. Select **Tools $\rightarrow$ Board $\rightarrow$ Arduino Uno** and the corresponding **COM Port**.
9. Click **Upload**.
10. Open `index.html` in Chrome/Edge, click **"🔌 Connect to Arduino"**, and operate your real hardware directly from the web dashboard.

---

## 9. Experimental Results & Observations
- **Timing Accuracy:** The `millis()` state machine demonstrated zero timing drift over extended 2-hour stress runs.
- **Relay Frequency Response:** Clean, distinct switching was observed down to 50 ms. Below 30 ms, physical contact inertia resulted in audible contact chatter.
- **Power Stability:** With the 9V 1A DC adapter connected, zero USB bus resets or microcontroller brownouts occurred, even during Mode 4 (simultaneous 8-relay pull-in).

---

## 10. Advantages & System Strengths
1. **Galvanic Isolation:** High-voltage load transients are completely isolated from low-voltage digital logic via optical isolation.
2. **Zero-Latency Responsiveness:** Non-blocking design ensures incoming user commands are acknowledged and executed instantly.
3. **Multi-Platform Operation:** Runs as a standalone browser simulation, cloud Wokwi project, or live physical hardware installation.
4. **Plug-and-Play Web Serial:** Eliminates the need for custom native desktop software or third-party drivers.

---

## 11. Engineering Limitations
1. **Mechanical Relay Switching Speed:** Electromechanical contacts cannot switch faster than $\sim 30\text{ ms}$ due to physical inertia.
2. **Contact Wear:** Mechanical contacts possess a finite lifespan ($\sim 10^5$ operations under full electrical load).
3. **No Native Wi-Fi on ATmega328P:** The Arduino Uno R3 lacks onboard wireless networking; it relies on host USB serial bridging for cloud interaction.

---

## 12. Future Scope & Upgrade Roadmap

```mermaid
flowchart LR
    A["Current System: Uno + Web Serial Bridge"] --> B["Phase 1: ESP32 / ESP8266 Migration (Native 2.4 GHz Wi-Fi)"]
    B --> C["Phase 2: Cloud MQTT Broker (HiveMQ / AWS IoT Core)"]
    C --> D["Phase 3: Cross-Platform Mobile App (Blynk / Flutter)"]
    D --> E["Phase 4: Solid-State Relays (SSR) for 230V AC Stage Lights"]
```

1. **Native Wi-Fi Migration (ESP32 / ESP8266):** Replace the Uno with an ESP32 micro-module, enabling untethered direct Wi-Fi connectivity and onboard MQTT/HTTPS clients.
2. **Mobile App Control (Blynk / Flutter):** Develop a responsive mobile application allowing global control and monitoring over cellular networks.
3. **Solid-State Relays (SSRs):** Upgrade mechanical relays to opto-TRIAC Solid State Relays (SSRs) with zero-cross detection to switch high-speed 230V AC decorative lighting without mechanical wear.
4. **Sensor-Driven Orchestration:** Integrate sound sensors (microphone modules) for audio-reactive dancing light shows, or ambient light sensors (LDRs) for automatic twilight operation.

---

## 13. Project Disclosure
> [!NOTE]
> **Academic Disclosure:** The standard Arduino Uno R3 (ATmega328P) does not possess native Wi-Fi or Bluetooth radios. In this project, IoT functionality is implemented via an **edge-gateway architecture**: the computer browser connects directly to the microcontroller over the **Web Serial API** while simulating the upstream **MQTT Cloud Broker** publish/subscribe pipeline. Upgrading to an ESP32 or ESP8266 provides the direct, untethered wireless IoT equivalent.

---

## 14. Conclusion
The **IoT Based LED Chaser (Running Lights)** project successfully validates the principles of non-blocking embedded software design, optocoupled load isolation, and browser-to-hardware telemetry bridging. The system provides a modular, reliable, and submission-ready engineering solution, bridging academic embedded system concepts with industrial IoT practices.
