import pygame
import time

# Initialize Pygame and the joystick module
pygame.init()
pygame.joystick.init()

# Check for connected joysticks
if pygame.joystick.get_count() == 0:
    print("No joystick found! Please connect your Logitech F710.")
    exit()

# Get the first joystick
joystick = pygame.joystick.Joystick(0)
joystick.init()

print(f"Connected to joystick: {joystick.get_name()}")

# Main loop to read inputs
try:
    while True:
        # Handle events
        for event in pygame.event.get():
            if event.type == pygame.JOYBUTTONDOWN:
                print(f"Button {event.button} pressed")
            elif event.type == pygame.JOYBUTTONUP:
                print(f"Button {event.button} released")
            elif event.type == pygame.JOYAXISMOTION:
                axis = event.axis
                value = joystick.get_axis(axis)
                print(f"Axis {axis} moved to {value:.2f}")
            elif event.type == pygame.JOYHATMOTION:
                hat = event.hat
                value = joystick.get_hat(hat)
                print(f"Hat {hat} moved to {value}")

        time.sleep(0.1)

except KeyboardInterrupt:
    print("\nExiting...")

finally:
    # Clean up
    joystick.quit()
    pygame.quit()
