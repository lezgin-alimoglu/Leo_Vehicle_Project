import serial
import pygame
import time

# --- Serial Setup ---
arduino_port = "/dev/ttyACM0"
baud_rate = 9600

try:
    arduino = serial.Serial(arduino_port, baud_rate, timeout=1)
    time.sleep(2)  # Allow Arduino to reset
    print("[INFO] Connected to Arduino on /dev/ttyACM0")
except serial.SerialException:
    print("[ERROR] Could not open /dev/ttyACM0. Check USB connection.")
    exit(1)

# --- Pygame Setup ---
pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:
    print("[ERROR] No joystick detected.")
    exit(1)

joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"[INFO] Joystick detected: {joystick.get_name()}")

# --- Send a single character command to Arduino ---
def send_command(char):
    try:
        arduino.write(char.encode("utf-8"))
        time.sleep(0.01)
    except:
        print("[ERROR] Failed to send command")

# --- Scale joystick input ---
def get_direction(axis_val, threshold=0.3):
    if axis_val > threshold:
        return 1
    elif axis_val < -threshold:
        return -1
    else:
        return 0

# --- Main Loop ---
try:
    while True:
        pygame.event.pump()

        # Read joystick axes
        x = joystick.get_axis(3)  # right stick horizontal
        y = joystick.get_axis(4)  # right stick vertical

        dir_x = get_direction(x)
        dir_y = get_direction(y)

        if dir_y == 1:
            send_command('d')  # down
        elif dir_y == -1:
            send_command('u')  # up
        elif dir_x == -1:
            send_command('l')  # left
        elif dir_x == 1:
            send_command('r')  # right
        else:
            send_command('s')  # stop / neutral

        time.sleep(0.1)

finally:
    print("[INFO] Exiting...")
    arduino.close()
    pygame.quit()
