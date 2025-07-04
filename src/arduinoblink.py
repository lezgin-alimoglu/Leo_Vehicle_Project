import serial
import time

# Use UART port on GPIO (ttyS0 or ttyAMA0 depends on Pi model)
port = '/dev/serial0'  # RPi3 default UART alias

# Set up serial
ser = serial.Serial(port, baudrate=9600, timeout=1)
time.sleep(2)  # Give Arduino time to boot

print("Sending test data to Arduino...")
ser.write(b'H')  # Send character 'H'

while True:
    if ser.in_waiting:
        line = ser.readline().decode('utf-8').strip()
        print(f"Received: {line}")
