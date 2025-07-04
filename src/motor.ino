#include <Servo.h>

// ─── MOTOR PIN DEFINITIONS ─────────────────────────────
const byte motorFLPin = 3;   // Front-left ESC signal pin
const byte motorFRPin = 5;   // Front-right ESC signal pin
const byte motorBLPin = 11;  // Back-left ESC signal pin
const byte motorBRPin = 9;   // Back-right ESC signal pin

// ─── SERVO OBJECTS FOR ESCs ───────────────────────────
Servo motorFL;
Servo motorFR;
Servo motorBL;
Servo motorBR;

// ─── NEUTRAL AND COMMAND PULSES ───────────────────────
const int neutralPulse = 1500;
const int pulseStep    = 200;   // Change in µs for each command

void setup() {
  Serial.begin(9600);

  motorFL.attach(motorFLPin);
  motorFR.attach(motorFRPin);
  motorBL.attach(motorBLPin);
  motorBR.attach(motorBRPin);

  // Initialize all motors to neutral
  motorFL.writeMicroseconds(neutralPulse);
  motorFR.writeMicroseconds(neutralPulse);
  motorBL.writeMicroseconds(neutralPulse);
  motorBR.writeMicroseconds(neutralPulse);

  Serial.println("ESC controller ready. Use 'u', 'd', 'l', 'r', 's'.");
}

void loop() {
  if (Serial.available() > 0) {
    char message = Serial.read();

    switch (message) {
      case 'u':  // pitch forward (all motors forward)
        motorFL.writeMicroseconds(neutralPulse - pulseStep);
        motorFR.writeMicroseconds(neutralPulse + pulseStep);
        motorBL.writeMicroseconds(neutralPulse - pulseStep);
        motorBR.writeMicroseconds(neutralPulse + pulseStep);
        Serial.println("↑ Up");
        break;

      case 'd':  // pitch backward (all motors reverse)
        motorFL.writeMicroseconds(neutralPulse + pulseStep);
        motorFR.writeMicroseconds(neutralPulse - pulseStep);
        motorBL.writeMicroseconds(neutralPulse + pulseStep);
        motorBR.writeMicroseconds(neutralPulse - pulseStep);
        Serial.println("↓ Down");
        break;

      case 'l':  // roll left
        motorFL.writeMicroseconds(neutralPulse - pulseStep);
        motorBL.writeMicroseconds(neutralPulse + pulseStep);
        motorFR.writeMicroseconds(neutralPulse - 2 * pulseStep);
        motorBR.writeMicroseconds(neutralPulse + 2 * pulseStep);
        Serial.println("← Left");
        break;

      case 'r':  // roll right
        motorFL.writeMicroseconds(neutralPulse + 2 * pulseStep);
        motorBL.writeMicroseconds(neutralPulse - 2 * pulseStep);
        motorFR.writeMicroseconds(neutralPulse + pulseStep);
        motorBR.writeMicroseconds(neutralPulse + pulseStep);
        Serial.println("→ Right");
        break;

      case 's':  // stop / go to neutral
        motorFL.writeMicroseconds(neutralPulse);
        motorFR.writeMicroseconds(neutralPulse);
        motorBL.writeMicroseconds(neutralPulse);
        motorBR.writeMicroseconds(neutralPulse);
        Serial.println("■ Stop");
        break;

      default:
        // Ignore unknown commands
        break;
    }
  }
}
