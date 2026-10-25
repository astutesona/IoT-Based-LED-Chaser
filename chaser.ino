/*
  =============================================================================
  Project: IoT Based LED Chaser (Running Lights)
  Target:  Arduino Uno R3 (ATmega328P)
  Baud:    115200 bps
  Logic:   Active-LOW Relay Module (LOW = ON, HIGH = OFF)
  Timing:  Non-blocking millis() state machine (Zero delay() calls)
  =============================================================================
*/

// --- Hardware Pin Allocations ---
const uint8_t RELAY_PINS[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // Uno D2 - D9 -> Relay IN1 - IN8
const uint8_t BUZZER_PIN    = 10;                        // Uno D10 -> Active 5V Buzzer (+)
const uint8_t NUM_LEDS      = 8;

// --- Relay Logic Definitions ---
// Most 8-channel optocoupled relay modules are Active-LOW:
// LOW  pulls the optocoupler cathode to GND -> energizes relay coil -> NO connects to COM
// HIGH turns off the optocoupler LED        -> de-energizes relay   -> NO opens from COM
#define RELAY_ACTIVE   LOW
#define RELAY_INACTIVE HIGH

// --- State Machine & Control Variables ---
bool isRunning = true;
int currentMode = 1;                // 1: Forward, 2: Reverse, 3: Bounce, 4: All-Blink
unsigned long stepInterval = 200;   // Chaser speed in milliseconds per step (default 200ms)
unsigned long lastStepTime = 0;

// Mode execution state tracking
int stepIndex = 0;
int bounceDirection = 1;            // +1 for forward, -1 for reverse (Mode 3)
bool blinkState = false;            // Toggle state for Mode 4

// Non-blocking Buzzer control
unsigned long buzzerStartTime = 0;
const unsigned long BUZZER_BEEP_DURATION = 25; // 25ms short click beep
bool isBuzzerActive = false;

// Serial Buffer
String inputString = "";
bool stringComplete = false;

// --- Forward Declarations ---
void turnAllRelaysOff();
void triggerBuzzer();
void updateBuzzer();
void executeChaserStep();
void processCommand(String cmd);
void printStatusJson();
void printHelp();

void setup() {
  // Pre-initialize pins HIGH before setting as OUTPUT to avoid relay clicking during boot
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    digitalWrite(RELAY_PINS[i], RELAY_INACTIVE);
    pinMode(RELAY_PINS[i], OUTPUT);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize Hardware UART at 115200 baud
  Serial.begin(115200);
  inputString.reserve(64);

  // Startup announcement
  Serial.println(F("\n=================================================="));
  Serial.println(F("  IoT Based LED Chaser (Running Lights) Firmware  "));
  Serial.println(F("  Hardware: Arduino Uno R3 + 8-Channel Relay Mod  "));
  Serial.println(F("  Baud Rate: 115200 | Timing: millis() Engine     "));
  Serial.println(F("=================================================="));
  printHelp();
  printStatusJson();

  // Initial step execution
  lastStepTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Maintain non-blocking buzzer pulse
  updateBuzzer();

  // 2. Check for incoming Serial commands from Web IoT Dashboard / Serial Monitor
  if (stringComplete) {
    inputString.trim();
    if (inputString.length() > 0) {
      processCommand(inputString);
    }
    inputString = "";
    stringComplete = false;
  }

  // 3. Chaser Animation Engine (Non-blocking timer)
  if (isRunning) {
    if (currentMillis - lastStepTime >= stepInterval) {
      lastStepTime = currentMillis;
      executeChaserStep();
    }
  }
}

// --- Serial Event Listener ---
// Reads characters into buffer until newline is encountered
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        stringComplete = true;
      }
    } else {
      inputString += inChar;
    }
  }
}

// --- Command Processor ---
void processCommand(String cmd) {
  cmd.toUpperCase();

  if (cmd == "START") {
    isRunning = true;
    lastStepTime = millis();
    Serial.println(F("[ACK] START: Chaser running"));
    printStatusJson();
  }
  else if (cmd == "STOP") {
    isRunning = false;
    turnAllRelaysOff();
    Serial.println(F("[ACK] STOP: Chaser stopped, all relays opened"));
    printStatusJson();
  }
  else if (cmd.startsWith("SPEED:")) {
    long newSpeed = cmd.substring(6).toInt();
    if (newSpeed >= 30 && newSpeed <= 3000) {
      stepInterval = (unsigned long)newSpeed;
      Serial.print(F("[ACK] SPEED updated to: "));
      Serial.print(stepInterval);
      Serial.println(F(" ms"));
      printStatusJson();
    } else {
      Serial.println(F("[ERR] Invalid SPEED. Range: 30 to 3000 ms"));
    }
  }
  else if (cmd.startsWith("MODE:")) {
    int newMode = cmd.substring(5).toInt();
    if (newMode >= 1 && newMode <= 4) {
      currentMode = newMode;
      stepIndex = 0;
      bounceDirection = 1;
      blinkState = false;
      turnAllRelaysOff();
      Serial.print(F("[ACK] MODE switched to: "));
      Serial.println(currentMode);
      printStatusJson();
    } else {
      Serial.println(F("[ERR] Invalid MODE. Valid values: 1, 2, 3, 4"));
    }
  }
  else if (cmd == "STATUS") {
    printStatusJson();
  }
  else if (cmd == "HELP") {
    printHelp();
  }
  else {
    Serial.print(F("[ERR] Unknown command: "));
    Serial.println(cmd);
    Serial.println(F("Available: START, STOP, SPEED:<ms>, MODE:<1-4>, STATUS, HELP"));
  }
}

// --- Chaser Animation Logic ---
void executeChaserStep() {
  turnAllRelaysOff();
  int activeRelay = -1;

  switch (currentMode) {
    case 1: // Mode 1: Forward Chaser (1 -> 8)
      activeRelay = stepIndex;
      digitalWrite(RELAY_PINS[activeRelay], RELAY_ACTIVE);
      triggerBuzzer();
      stepIndex = (stepIndex + 1) % NUM_LEDS;
      break;

    case 2: // Mode 2: Reverse Chaser (8 -> 1)
      activeRelay = (NUM_LEDS - 1) - stepIndex;
      digitalWrite(RELAY_PINS[activeRelay], RELAY_ACTIVE);
      triggerBuzzer();
      stepIndex = (stepIndex + 1) % NUM_LEDS;
      break;

    case 3: // Mode 3: Bounce / Ping-Pong (1 -> 8 -> 1)
      activeRelay = stepIndex;
      digitalWrite(RELAY_PINS[activeRelay], RELAY_ACTIVE);
      triggerBuzzer();
      stepIndex += bounceDirection;
      if (stepIndex >= NUM_LEDS - 1) {
        stepIndex = NUM_LEDS - 1;
        bounceDirection = -1;
      } else if (stepIndex <= 0) {
        stepIndex = 0;
        bounceDirection = 1;
      }
      break;

    case 4: // Mode 4: All-Blink (All ON -> All OFF)
      blinkState = !blinkState;
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        digitalWrite(RELAY_PINS[i], blinkState ? RELAY_ACTIVE : RELAY_INACTIVE);
      }
      if (blinkState) {
        triggerBuzzer();
        activeRelay = 99; // Sentinel representing all ON
      } else {
        activeRelay = -1; // All OFF
      }
      break;

    default:
      currentMode = 1;
      stepIndex = 0;
      break;
  }

  // Real-time telemetry output stream for Serial Monitor / Web IoT UI
  Serial.print(F("{\"telemetry\":{\"running\":"));
  Serial.print(isRunning ? F("true") : F("false"));
  Serial.print(F(",\"mode\":"));
  Serial.print(currentMode);
  Serial.print(F(",\"speed\":"));
  Serial.print(stepInterval);
  Serial.print(F(",\"activePin\":"));
  if (activeRelay >= 0 && activeRelay < NUM_LEDS) {
    Serial.print(RELAY_PINS[activeRelay]);
  } else if (activeRelay == 99) {
    Serial.print(F("\"ALL\""));
  } else {
    Serial.print(F("\"NONE\""));
  }
  Serial.print(F(",\"relayIndex\":"));
  Serial.print(activeRelay);
  Serial.println(F("}}"));
}

// --- Helper Functions ---

void turnAllRelaysOff() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    digitalWrite(RELAY_PINS[i], RELAY_INACTIVE);
  }
}

void triggerBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerStartTime = millis();
  isBuzzerActive = true;
}

void updateBuzzer() {
  if (isBuzzerActive) {
    if (millis() - buzzerStartTime >= BUZZER_BEEP_DURATION) {
      digitalWrite(BUZZER_PIN, LOW);
      isBuzzerActive = false;
    }
  }
}

void printStatusJson() {
  Serial.print(F("{\"status\":{\"running\":"));
  Serial.print(isRunning ? F("true") : F("false"));
  Serial.print(F(",\"mode\":"));
  Serial.print(currentMode);
  Serial.print(F(",\"speed\":"));
  Serial.print(stepInterval);
  Serial.print(F(",\"baud\":115200"));
  Serial.println(F("}}"));
}

void printHelp() {
  Serial.println(F("--- Available Serial Commands ---"));
  Serial.println(F("  START         : Start LED chaser animation"));
  Serial.println(F("  STOP          : Stop chaser & deactivate all relays"));
  Serial.println(F("  SPEED:<ms>    : Set step delay in ms (30 - 3000)"));
  Serial.println(F("  MODE:<1-4>    : 1=Forward, 2=Reverse, 3=Bounce, 4=All-Blink"));
  Serial.println(F("  STATUS        : Output JSON state telemetry"));
  Serial.println(F("  HELP          : Show this instruction guide"));
  Serial.println(F("---------------------------------"));
}
