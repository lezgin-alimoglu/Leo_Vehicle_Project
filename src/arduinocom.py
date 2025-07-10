#!/usr/bin/env python3
import os
# SSH üzerinden çalışırken video alt sistemini dummy’e yönlendir
os.environ["SDL_VIDEODRIVER"] = "dummy"

import serial
import pygame
import time
import sys

# --- Serial Setup ---
ARDUINO_PORT = "/dev/ttyUSB2"  # ls /dev/ttyACM* ile doğru portu bulun
BAUD_RATE    = 9600

try:
    arduino = serial.Serial(ARDUINO_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)  # Arduino reset için bekle
    print(f"[INFO] Connected to Arduino on {ARDUINO_PORT}")
except serial.SerialException:
    print(f"[ERROR] Could not open {ARDUINO_PORT}.")
    sys.exit(1)

# --- Pygame & Joystick Init (SADECE BİR KEZ) ---
pygame.init()
pygame.joystick.init()
if pygame.joystick.get_count() == 0:
    print("[ERROR] No joystick detected.")
    arduino.close()
    pygame.quit()
    sys.exit(1)

joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"[INFO] Joystick: {joystick.get_name()}")

# --- Helpers ---
DEADZONE = 0.3
last_cmd = None

def send_command(cmd):
    """Arduino’ya sadece komut değiştiğinde gönder."""
    global last_cmd
    if cmd != last_cmd:
        try:
            arduino.write(cmd.encode())
            print(f"[SEND] {cmd}")
        except Exception as e:
            print(f"[ERROR] Send failed: {e}")
        last_cmd = cmd

def get_direction(val):
    """Analog ekseni -1/0/+1 aralığına indirger."""
    if val > DEADZONE:  return 1
    if val < -DEADZONE: return -1
    return 0

print("Başladı: Ctrl+C ile çık, joystick ile yönlendir, A stop.")

# --- Main Loop ---
try:
    while True:
        # EVENT PUMP: pygame video subsistemini kapatmadan her tur
        pygame.event.pump()

        # Sağ çubuk: axis 3 = yatay, axis 4 = dikey (FC710)
        dir_x = get_direction(joystick.get_axis(2))
        dir_y = get_direction(joystick.get_axis(1))

        # A butonu (0) öncelikli stop
        if joystick.get_button(0):
            cmd = 's'
        elif dir_y == 1:
            cmd = 'd'  # stick aşağı → pitch backward
        elif dir_y == -1:
            cmd = 'u'  # stick yukarı → pitch forward
        elif dir_x == -1:
            cmd = 'l'  # sola roll
        elif dir_x == 1:
            cmd = 'r'  # sağa roll
        else:
            cmd = 's'

        send_command(cmd)

        time.sleep(0.05)

except KeyboardInterrupt:
    print("\n[INFO] KeyboardInterrupt, çıkılıyor...")

finally:
    # Çıkışta kesin nötr komut ve temizlik
    send_command('s')
    time.sleep(0.2)
    arduino.close()
    # pygame’ı ancak en son kapat
    pygame.joystick.quit()
    pygame.quit()
    print("[INFO] Motorlar nötr, serial ve pygame kapatıldı.")
