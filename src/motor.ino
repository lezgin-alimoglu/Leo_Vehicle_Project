#include <Servo.h>

// ─── MOTOR PIN DEFINITIONS ─────────────────────────────
const byte motorFLPin = 3;
const byte motorFRPin = 5;
const byte motorBLPin = 11;
const byte motorBRPin = 9;

// ─── SERVO OBJECTS ─────────────────────────────────────
Servo motorFL, motorFR, motorBL, motorBR;

// ─── MOTOR STATE VARIABLES ─────────────────────────────
int pulseFL = 1500;
int pulseFR = 1500;
int pulseBL = 1500;
int pulseBR = 1500;

const int neutralPulse = 1500;
const int pulseStep = 100;      // Small steps for gradual change
const int maxPulse = 2000;     // Upper limit for ESC
const int minPulse = 1000;     // Lower limit for ESC

void setup() {
  Serial.begin(9600);
  motorFL.attach(motorFLPin);
  motorFR.attach(motorFRPin);
  motorBL.attach(motorBLPin);
  motorBR.attach(motorBRPin);
  updateMotorSpeeds();  // Set all to neutral
  Serial.println("ESC controller ready. Use 'u', 'd', 'l', 'r', 's'.");
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    for (int i = 0; i < 20; i++) {  // Adjust how smooth/fast it changes
      switch (cmd) {
        case 'u':  // Pitch forward
          pulseFL = constrain(pulseFL - pulseStep, minPulse, maxPulse);
          pulseFR = constrain(pulseFR + pulseStep, minPulse, maxPulse);
          pulseBL = constrain(pulseBL - pulseStep, minPulse, maxPulse);
          pulseBR = constrain(pulseBR + pulseStep, minPulse, maxPulse);
          break;

        case 'd':  // Pitch backward
          pulseFL = constrain(pulseFL + pulseStep, minPulse, maxPulse);
          pulseFR = constrain(pulseFR - pulseStep, minPulse, maxPulse);
          pulseBL = constrain(pulseBL + pulseStep, minPulse, maxPulse);
          pulseBR = constrain(pulseBR - pulseStep, minPulse, maxPulse);
          break;

        case 'l':  // Roll left
          pulseFL = constrain(pulseFL - pulseStep, minPulse, maxPulse);
          pulseBL = constrain(pulseBL + pulseStep, minPulse, maxPulse);
          pulseFR = constrain(pulseFR - 2 * pulseStep, minPulse, maxPulse);
          pulseBR = constrain(pulseBR + 2 * pulseStep, minPulse, maxPulse);
          break;

        case 'r':  // Roll right
          pulseFL = constrain(pulseFL + 2 * pulseStep, minPulse, maxPulse);
          pulseBL = constrain(pulseBL - 2 * pulseStep, minPulse, maxPulse);
          pulseFR = constrain(pulseFR + pulseStep, minPulse, maxPulse);
          pulseBR = constrain(pulseBR + pulseStep, minPulse, maxPulse);
          break;

        case 's':  // Stop / neutral
          pulseFL = neutralPulse;
          pulseFR = neutralPulse;
          pulseBL = neutralPulse;
          pulseBR = neutralPulse;
          break;
      }
      updateMotorSpeeds();
      delay(20);  // Adjust delay for smoother or quicker response
    }

    Serial.print("FL: "); Serial.print(pulseFL);
    Serial.print(" FR: "); Serial.print(pulseFR);
    Serial.print(" BL: "); Serial.print(pulseBL);
    Serial.print(" BR: "); Serial.println(pulseBR);
  }
}

void updateMotorSpeeds() {
  motorFL.writeMicroseconds(pulseFL);
  motorFR.writeMicroseconds(pulseFR);
  motorBL.writeMicroseconds(pulseBL);
  motorBR.writeMicroseconds(pulseBR);
}
