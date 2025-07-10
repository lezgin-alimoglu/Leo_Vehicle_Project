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
const int pulseStep = 100;      // Adım boyutu
const int maxPulse = 2000;
const int minPulse = 1000;

void setup() {
  Serial.begin(9600);
  motorFL.attach(motorFLPin);
  motorFR.attach(motorFRPin);
  motorBL.attach(motorBLPin);
  motorBR.attach(motorBRPin);
  updateMotorSpeeds();
  Serial.println("ESC controller ready. Use 'u','d','l','r','s'.");
}

void loop() {
  if (Serial.available() == 0) return;
  char cmd = Serial.read();

  for (int i = 0; i < 20; i++) {
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
        pulseFL = constrain(pulseFL + pulseStep, minPulse, maxPulse);
        pulseBL = constrain(pulseBL + pulseStep, minPulse, maxPulse);
        pulseFR = constrain(pulseFR + pulseStep, minPulse, maxPulse);
        pulseBR = constrain(pulseBR + pulseStep, minPulse, maxPulse);
        break;

      case 'r':  // Roll right
        pulseFL = constrain(pulseFL - pulseStep, minPulse, maxPulse);
        pulseBL = constrain(pulseBL - pulseStep, minPulse, maxPulse);
        pulseFR = constrain(pulseFR - pulseStep, minPulse, maxPulse);
        pulseBR = constrain(pulseBR - pulseStep, minPulse, maxPulse);
        break;

      case 's':  // Smooth stop: her mikser darbesini neutralPulse'a yaklaştır
        pulseFL = rampToNeutral(pulseFL);
        pulseFR = rampToNeutral(pulseFR);
        pulseBL = rampToNeutral(pulseBL);
        pulseBR = rampToNeutral(pulseBR);
        break;

      default:
        // Tanınmayan komut: hiçbir şey yapma
        break;
    }
    updateMotorSpeeds();
    delay(20);
  }

  Serial.print("FL: "); Serial.print(pulseFL);
  Serial.print(" FR: "); Serial.print(pulseFR);
  Serial.print(" BL: "); Serial.print(pulseBL);
  Serial.print(" BR: "); Serial.println(pulseBR);
}

void updateMotorSpeeds() {
  motorFL.writeMicroseconds(pulseFL);
  motorFR.writeMicroseconds(pulseFR);
  motorBL.writeMicroseconds(pulseBL);
  motorBR.writeMicroseconds(pulseBR);
}

// Mevcut darbe genişliğini neutralPulse'a doğru bir adım ilerletir
int rampToNeutral(int current) {
  if (current < neutralPulse) {
    return constrain(current + pulseStep, minPulse, neutralPulse);
  } else if (current > neutralPulse) {
    return constrain(current - pulseStep, neutralPulse, maxPulse);
  } else {
    return current;
  }
}
