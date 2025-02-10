import serial
import time

# ls /dev/ttyUSB*
# sudo usermod -a -G dialout $USER

# Initialize the serial connection
arduino = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
time.sleep(2)  # Wait for the connection to stabilize

# Check if the connection is open
if arduino.is_open:
    print("Connection established!")
else:
    print("Failed to connect.")
  
# Initialize Pygame
pygame.init()
pygame.joystick.init()

joystick = pygame.joystick.Joystick(0)  # Logitech F710
joystick.init()

def send_command(command):
    arduino.write(f"{command};".encode('utf-8'))
    time.sleep(0.01)  # Short delay to ensure data is sent

try:
    while True:
        pygame.event.pump()  # Process events
        
        # Get joystick axes (e.g., left stick Y-axis for RPM)
        axis_value = joystick.get_axis(1)  # Adjust based on joystick configuration
        rpm = axis_value * 10000  # Scale axis to RPM range
        
        # Send RPM command to Arduino for all motors
        send_command(f"rpm:104,{rpm}")
        send_command(f"rpm:103,{rpm}")
        send_command(f"rpm:102,{rpm}")
        send_command(f"rpm:101,{rpm}")
        
        # Optional: Handle buttons (e.g., send position commands)
        if joystick.get_button(0):  # Button A
            send_command("pos:104,100.0")  # Example position
        if joystick.get_button(1):  # Button B
            send_command("pos:104,0.0")
        
        time.sleep(0.1)  # Control loop delay
finally:
    pygame.quit()
    arduino.close()
