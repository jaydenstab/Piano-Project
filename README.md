# Piano Project - MIDI to Solenoid Controller

This project converts MIDI input to control 6 (for now bc more didn't arrive) solenoids via an Arduino using a 74HC595 shift register.

## Project Overview
- **Arduino Code** (`solenoid_controller.ino`): Receives MIDI messages via serial and controls 6 solenoids
- **Python Bridge** (`bridge.py`): **Current/Correct** - Connects MIDI input (IAC Driver) to Arduino via serial
- **Python Midi Converter** (`Python Midi Converter`): **Old code** - Do not use

## Hardware Setup

### Components
- Arduino (Uno/Nano/etc.)
- 74HC595 Shift Register
- 6 Solenoids with XY-MOS modules
- MIDI input source (via IAC Driver on macOS)

### Arduino Connections
- **DATA_PIN (DS)**: Pin 11
- **LATCH_PIN (ST_CP)**: Pin 10
- **CLOCK_PIN (SH_CP)**: Pin 13
- **Serial**: USB connection to computer

### MIDI Note Mapping
The Arduino maps the following MIDI notes to solenoids 1-6:
- Solenoid 1: C4 (MIDI note 60)
- Solenoid 2: D4 (MIDI note 62)
- Solenoid 3: E4 (MIDI note 64)
- Solenoid 4: F4 (MIDI note 65)
- Solenoid 5: G4 (MIDI note 67)
- Solenoid 6: A4 (MIDI note 69)

## Software Setup

### Prerequisites
1. **Python 3** with the following packages:
   ```bash
   pip install mido pyserial
   ```

2. **Arduino IDE** (for uploading code to Arduino)

3. **macOS IAC Driver** (for MIDI routing):
   - Open "Audio MIDI Setup" (Applications > Utilities)
   - Window > Show MIDI Studio
   - Double-click "IAC Driver"
   - Enable at least one bus (e.g., "Bus 1")

### Installation Steps

1. **Upload Arduino Code**:
   - Option A: Use Arduino Cloud Editor at https://app.arduino.cc/sketches/e282aeae-1537-4f76-addc-c14642411d76
   - Option B: Open `solenoid_controller.ino` in Arduino IDE
   - Select your Arduino board and port
   - Upload the code

2. **Find Your Arduino Port**:
   - On macOS: Usually `/dev/cu.usbmodemXXXX` or `/dev/cu.usbserialXXXX`
   - Check Arduino IDE or run: `ls /dev/cu.*` in terminal
   - **Note**: Update the port in the Python script (see below)

3. **Update Python Script Configuration**:
   - Edit `bridge.py` to match your Arduino port
   - Update the `ARDUINO_PORT` variable (currently set to `/dev/cu.usbmodem1301`)

4. **Run the Python Bridge**:
   ```bash
   python3 bridge.py
   ```
   **Note**: `Python Midi Converter` is old code and should not be used.

## Usage

1. **Start the Python bridge script** (see above)
2. **Send MIDI to IAC Driver** from your MIDI source:
   - **For testing**: Use the virtual keyboard at https://signalmidi.app/edit
   - Or use your DAW, MIDI keyboard, or other MIDI source
3. **The solenoids will activate** when the mapped notes (C4-A4) are played on MIDI channel 1

## Current Problems / Known Issues

### 1. **Baud Rate Mismatch**
   - `bridge.py` uses **9600 baud**
   - `solenoid_controller.ino` expects **115200 baud**
   - **Solution**: Update `bridge.py` to use 115200 baud to match the Arduino code

### 2. **IAC Driver Bus**
   - `bridge.py` looks for "IAC Driver" (any bus)
   - Ensure at least one IAC Driver bus is enabled in Audio MIDI Setup

### 3. **Hardcoded Arduino Port**
   - `bridge.py` hardcodes `/dev/cu.usbmodem1301`
   - This port may not exist on all systems
   - **Solution**: Update the `ARDUINO_PORT` variable in `bridge.py` to match your Arduino's port

### 4. **MIDI Channel**
   - Arduino only accepts MIDI channel 1 (channel 0 in code)
   - Other channels are ignored
   - **Note**: This may be intentional, but limits flexibility

### 5. **Error Handling**
   - Limited error handling for serial connection failures
   - No graceful handling if IAC Driver is not found
   - **Recommendation**: Add better error messages and retry logic

## Problems

### 1. **Incorrect C Note Activation**
   - When pressing other notes, the C note (solenoid 1) incorrectly activates
   - This causes unwanted solenoid activation that doesn't match the MIDI input
   - **Status**: Not yet fixed - needs investigation into MIDI message parsing or note mapping logic

### 2. **Keys Getting Stuck**
   - Solenoids sometimes remain activated after a note is released
   - This suggests note_off messages may not be properly received or processed
   - **Status**: Not yet fixed - may be related to serial communication timing or MIDI message handling

## Future Improvements

- [ ] Fix baud rate mismatch in `bridge.py` (update to 115200)
- [ ] Add command-line arguments for port and baud rate
- [ ] Support multiple MIDI channels
- [ ] Add configuration file for note mapping
- [ ] Improve error handling and logging
- [ ] Add support for velocity-sensitive solenoid control
- [ ] Cross-platform serial port detection
