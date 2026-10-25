/*
  =============================================================================
  Project: IoT Based LED Chaser (Running Lights)
  Target:  Arduino Uno R3 (ATmega328P) - Wokwi Online Simulation
  Baud:    115200 bps
  Logic:   Active-LOW Relay Module (LOW = ON, HIGH = OFF)
  Timing:  Non-blocking millis() state machine
  =============================================================================
*/

const uint8_t RELAY_PINS[8] = {2, 3, 4, 5, 6, 7, 8, 9};
const uint8_t BUZZER_PIN    = 10;
const uint8_t NUM_LEDS      = 8;

#define RELAY_ACTIVE   LOW
#define RELAY_INACTIVE HIGH

bool isRunning = true;
int currentMode = 1;
unsigned long stepInterval = 200;
unsigned long lastStepTime = 0;

int stepIndex = 0;
int bounceDirection = 1;
bool blinkState = false;

unsigned long buzzerStartTime = 0;
const unsigned long BUZZER_BEEP_DURATION = 25;
bool isBuzzerActive = false;

String inputString = "";
bool stringComplete = false;

void turnAllRelaysOff();
void triggerBuzzer();
void updateBuzzer();
void executeChaserStep();
void processCommand(String cmd);
void printStatusJson();
void printHelp();

void setup() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    digitalWrite(RELAY_PINS[i], RELAY_INACTIVE);
    pinMode(RELAY_PINS[i], OUTPUT);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  inputString.reserve(64);

  Serial.println(F("\n=================================================="));
  Serial.println(F("  IoT Based LED Chaser (Running Lights) - Wokwi   "));
  Serial.println(F("  Hardware: Arduino Uno R3 + 8-Ch Relay Module    "));
  Serial.println(F("  Baud Rate: 115200 | millis() Engine Active      "));
  Serial.println(F("=================================================="));
  printHelp();
  printStatusJson();

  lastStepTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  updateBuzzer();

  if (stringComplete) {
    inputString.trim();
    if (inputString.length() > 0) {
      processCommand(inputString);
    }
    inputString = "";
    stringComplete = false;
  }

  if (isRunning) {
    if (currentMillis - lastStepTime >= stepInterval) {
      lastStepTime = currentMillis;
      executeChaserStep();
    }
  }
}

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
  }
}

void executeChaserStep() {
  turnAllRelaysOff();
  int activeRelay = -1;

  switch (currentMode) {
    case 1:
      activeRelay = stepIndex;
      digitalWrite(RELAY_PINS[activeRelay], RELAY_ACTIVE);
      triggerBuzzer();
      stepIndex = (stepIndex + 1) % NUM_LEDS;
      break;

    case 2:
      activeRelay = (NUM_LEDS - 1) - stepIndex;
      digitalWrite(RELAY_PINS[activeRelay], RELAY_ACTIVE);
      triggerBuzzer();
      stepIndex = (stepIndex + 1) % NUM_LEDS;
      break;

    case 3:
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

    case 4:
      blinkState = !blinkState;
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        digitalWrite(RELAY_PINS[i], blinkState ? RELAY_ACTIVE : RELAY_INACTIVE);
      }
      if (blinkState) {
        triggerBuzzer();
        activeRelay = 99;
      } else {
        activeRelay = -1;
      }
      break;

    default:
      currentMode = 1;
      stepIndex = 0;
      break;
  }

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
