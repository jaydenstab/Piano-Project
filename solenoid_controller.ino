// ======= 6-Solenoid MIDI Controller (Hold Active) =======
// Arduino reads framed MIDI bytes (F0...F7) from python midi_to_serial.py
// 74HC595 controls 6 solenoids via XY-MOS modules

const int DATA_PIN  = 11;  // DS
const int LATCH_PIN = 10;  // ST_CP
const int CLOCK_PIN = 13;  // SH_CP

// --- MIDI Settings ---
const uint8_t ACCEPT_CHANNEL = 0;  // MIDI channel 1
// Map C4–A4 to solenoids 1–6
const uint8_t noteMap[6] = {60, 62, 64, 65, 67, 69};
uint8_t solenoidMask = 0; // 8-bit pattern sent to 74HC595

// --- Input Buffer ---
bool inMsg = false;
uint8_t msg[3];
uint8_t idx = 0;

// --- Hardware Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  clearAll();
}

// --- Main Loop ---
void loop() {
  while (Serial.available()) {
    uint8_t b = Serial.read();

    if (b == 0xF0) { inMsg = true; idx = 0; }          // Start marker
    else if (b == 0xF7 && inMsg) {                     // End marker
      if (idx == 3) handleMIDI(msg);
      inMsg = false;
      idx = 0;
    }
    else if (inMsg && idx < 3) msg[idx++] = b;         // Collect bytes
  }
}

// --- MIDI Handler ---
void handleMIDI(uint8_t *m) {
  uint8_t status = m[0];
  uint8_t cmd    = status & 0xF0;
  uint8_t chan   = status & 0x0F;
  uint8_t note   = m[1];
  uint8_t vel    = m[2];

  if (chan != ACCEPT_CHANNEL) return;

  if (cmd == 0x90 && vel > 0) {  // Note ON
    setSolenoid(note, true);
  } else if (cmd == 0x80 || (cmd == 0x90 && vel == 0)) { // Note OFF
    setSolenoid(note, false);
  }
}

// --- Activate/Deactivate Solenoid by Note ---
void setSolenoid(uint8_t note, bool on) {
  for (uint8_t i = 0; i < 6; i++) {
    if (note == noteMap[i]) {
      if (on) solenoidMask |=  (1 << i);  // turn on bit
      else    solenoidMask &= ~(1 << i);  // turn off bit
      updateShiftRegister();
      return;
    }
  }
}

// --- Update 74HC595 Outputs ---
void updateShiftRegister() {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, solenoidMask);
  digitalWrite(LATCH_PIN, HIGH);
}

// --- Turn All Off ---
void clearAll() {
  solenoidMask = 0;
  updateShiftRegister();
}

