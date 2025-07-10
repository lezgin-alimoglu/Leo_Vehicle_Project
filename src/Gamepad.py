#!/usr/bin/env python3
import os
os.environ["SDL_VIDEODRIVER"] = "dummy"

import serial
import pygame
import time

# --- Serial Setup ---
arduino_port = "/dev/ttyUSB0"
baud_rate     = 9600

try:
    arduino = serial.Serial(arduino_port, baud_rate, timeout=1)
    time.sleep(2)
    print(f"[INFO] Connected to Arduino on {arduino_port}")
except serial.SerialException:
    print(f"[ERROR] Could not open {arduino_port}.")
    exit(1)

# --- Pygame Setup ---
pygame.init()
pygame.joystick.init()
if pygame.joystick.get_count() == 0:
    print("[ERROR] No joystick detected.")
    arduino.close()
    pygame.quit()
    exit(1)

joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"[INFO] Joystick: {joystick.get_name()}")

# --- Helpers ---
DEADZONE = 0.3
last_cmd = None

def send_command(cmd):
    global last_cmd
    if cmd != last_cmd:
        try:
            arduino.write(cmd.encode())
            print(f"[SEND] {cmd}")
        except Exception as e:
            print(f"[ERROR] Send failed: {e}")
        last_cmd = cmd

def get_direction(val):
    if val > DEADZONE:  return 1
    if val < -DEADZONE: return -1
    return 0

print("Ctrl+C ile çık, joystick ile yönlendir, A stop.")

running = True
try:
    while running:
        # --- sadece burada pygame.event çağırıyoruz ---
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Joystick eksenleri
        pitch = -joystick.get_axis(1)
        roll  =  joystick.get_axis(2)

        dy = get_direction(pitch)
        dx = get_direction(roll)

        # Buton A (0) ile stop öncelikli
        if joystick.get_button(0):
            cmd = 's'
        elif dy == 1:
            cmd = 'u'
        elif dy == -1:
            cmd = 'd'
        elif dx == -1:
            cmd = 'l'
        elif dx == 1:
            cmd = 'r'
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
    pygame.joystick.quit()
    pygame.quit()
    print("[INFO] Motorlar nötr, serial ve pygame kapatıldı.")
