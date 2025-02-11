import pygame
import serial
import time

# Initialize serial communication
arduino = serial.Serial('/dev/ttyUSB0', 115200)  # Adjust the port as needed
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
    """
    Scale joystick input values to motor RPM range, applying a deadzone.
    Args:
        value (float): Raw joystick axis value (-1.0 to 1.0).
        deadzone (float): Threshold below which axis input is ignored.
        scale (int): Multiplier to scale the axis value.
    Returns:
        float: The scaled value, or 0 if within the deadzone.
    """
    if abs(value) < deadzone:
        return 0
    return value * scale

try:
    while True:
        pygame.event.pump()  # Process joystick events

        # Get right stick axes for movement
        horizontal_value = joystick.get_axis(2)  # Right stick X-axis
        vertical_value = joystick.get_axis(3)    # Right stick Y-axis

        # Scale axis values to RPM range
        horizontal_rpm = scale_value(horizontal_value)
        vertical_rpm = scale_value(vertical_value)

        # Determine motor commands based on direction
        if vertical_rpm > 0:  # Move up
            send_command(f"move:up,{vertical_rpm}")
        elif vertical_rpm < 0:  # Move down
            send_command(f"move:down,{abs(vertical_rpm)}")

        if horizontal_rpm > 0:  # Move right
            send_command(f"move:right,{horizontal_rpm}")
        elif horizontal_rpm < 0:  # Move left
            send_command(f"move:left,{abs(horizontal_rpm)}")

        # Add a small delay for the control loop
        time.sleep(0.1)

finally:
    pygame.quit()
    arduino.close()
