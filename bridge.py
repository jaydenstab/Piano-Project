import mido
import serial
import time

ARDUINO_PORT = '/dev/cu.usbmodem1301'
BAUD_RATE = 9600  

def main():
    try:
        print(f"Connecting to {ARDUINO_PORT} at {BAUD_RATE} baud...")
        ser = serial.Serial(ARDUINO_PORT, BAUD_RATE, timeout=0.01)
        time.sleep(2) 
        print("✅ Connected! Arduino is listening.")
    except Exception as e:
        print(f"❌ Connection Error: {e}")
        return

    iac_input = None
    print("Scanning for MIDI inputs...")
    for name in mido.get_input_names():
        if "IAC Driver" in name:
            iac_input = name

    if not iac_input:
        print("❌ Error: 'IAC Driver' not found.")
        return
    
    print(f"🎧 Listening on: {iac_input}")

    try:
        with mido.open_input(iac_input) as inp:
            for msg in inp:
                if msg.type in ("note_on", "note_off"):
                    raw_data = bytes(msg.bytes())
                    packet = b'\xF0' + raw_data + b'\xF7'
                    ser.write(packet)
                    print(f"Sent: {msg}")
    except KeyboardInterrupt:
        print("\nStopping...")
        ser.close()

if __name__ == "__main__":
    main()