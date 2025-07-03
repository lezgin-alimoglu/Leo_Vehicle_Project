import glob
import pygame
import serial
import time

def find_arduino_port():
    # Potansiyel seri port dosyaları
    ports = glob.glob('/dev/ttyUSB*') + glob.glob('/dev/ttyACM*')
    for p in ports:
        try:
            s = serial.Serial(p, 115200, timeout=1)
            s.close()
            return p
        except (OSError, serial.SerialException):
            continue
    raise IOError("Arduino portu bulunamadı. (/dev/ttyUSB* veya /dev/ttyACM* içinde)")

# Auto-detect port
arduino_port = find_arduino_port()
print(f"[INFO] Arduino bulundu: {arduino_port}")

# Initialize serial communication
arduino = serial.Serial(arduino_port, 115200)
time.sleep(2)  # Allow Arduino to initialize

# Initialize Pygame
pygame.init()
pygame.joystick.init()

joystick = pygame.joystick.Joystick(0)  # Logitech F710
joystick.init()

def send_command(command):
    arduino.write(f"{command};".encode('utf-8'))
    time.sleep(0.01)  # Short delay to ensure data is sent

def scale_value(value, deadzone=0.1, scale=10000):
    if abs(value) < deadzone:
        return 0
    return int(value * scale)

try:
    while True:
        pygame.event.pump()

        horizontal_value = joystick.get_axis(3)
        vertical_value   = joystick.get_axis(4)

        horizontal_rpm = scale_value(horizontal_value)
        vertical_rpm   = scale_value(vertical_value)

        # Dikey hareket
        if   vertical_rpm > 0:
            send_command(f"move:down,{vertical_rpm}")
        elif vertical_rpm < 0:
            send_command(f"move:up,{abs(vertical_rpm)}")

        # Yatay hareket
        if   horizontal_rpm > 0:
            send_command(f"move:right,{horizontal_rpm}")
        elif horizontal_rpm < 0:
            send_command(f"move:left,{abs(horizontal_rpm)}")

        time.sleep(0.1)

finally:
    pygame.quit()
    arduino.close()
