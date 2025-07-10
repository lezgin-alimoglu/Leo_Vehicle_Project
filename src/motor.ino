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
const int maxPulse     = 2000;
const int minPulse     = 1000;
const int pulseStep    = 100;  // Geçici, test aşamasında küçük tutabilirsin

void setup() {
  Serial.begin(9600);
  motorFL.attach(motorFLPin);
  motorFR.attach(motorFRPin);
  motorBL.attach(motorBLPin);
  motorBR.attach(motorBRPin);

  // ─── ESC ARMING SEQUENCE ───────────────────────────────
  Serial.println("Arming ESCs...");
  motorFL.writeMicroseconds(minPulse);
  motorFR.writeMicroseconds(minPulse);
  motorBL.writeMicroseconds(minPulse);
  motorBR.writeMicroseconds(minPulse);
  delay(3000);  // 3 saniye minimum throttle
    
  motorFL.writeMicroseconds(maxPulse);
  motorFR.writeMicroseconds(maxPulse);
  motorBL.writeMicroseconds(maxPulse);
  motorBR.writeMicroseconds(maxPulse);
  delay(2000);  // 2 saniye maximum throttle

  pulseFL = neutralPulse;
  pulseFR = neutralPulse;
  pulseBL = neutralPulse;
  pulseBR = neutralPulse;
  updateMotorSpeeds();
  delay(1000);  // 1 saniye neutral bekle
  Serial.println("ESCs armed. Controller ready.");
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
      // ─── inside your switch(cmd) ─────────────────────────────────
      
      case 'l':  // Turn left: left motors slow down, right motors speed up
        pulseFL = constrain(pulseFL , minPulse, maxPulse);
        pulseBL = constrain(pulseBL , minPulse, maxPulse);
        pulseFR = constrain(pulseFR + pulseStep, minPulse, maxPulse);
        pulseBR = constrain(pulseBR + pulseStep, minPulse, maxPulse);
        break;
      
      case 'r':  // Turn right: right motors slow down, left motors speed up
        pulseFL = constrain(pulseFL - pulseStep, minPulse, maxPulse);
        pulseBL = constrain(pulseBL - pulseStep, minPulse, maxPulse);
        pulseFR = constrain(pulseFR , minPulse, maxPulse);
        pulseBR = constrain(pulseBR , minPulse, maxPulse);
        break;


      case 's':  // Smooth stop
        pulseFL = rampToNeutral(pulseFL);
        pulseFR = rampToNeutral(pulseFR);
        pulseBL = rampToNeutral(pulseBL);
        pulseBR = rampToNeutral(pulseBR);
        break;

      default:
        break;
    }
    updateMotorSpeeds();
    delay(20);
  }

  // Debug: her loop sonunda pulse değerlerini yazdır
  Serial.print("DEBUG PULSES → ");
  Serial.print(pulseFL); Serial.print(',');
  Serial.print(pulseFR); Serial.print(',');
  Serial.print(pulseBL); Serial.print(',');
  Serial.println(pulseBR);
}

void updateMotorSpeeds() {
  motorFL.writeMicroseconds(pulseFL);
  motorFR.writeMicroseconds(pulseFR);
  motorBL.writeMicroseconds(pulseBL);
  motorBR.writeMicroseconds(pulseBR);
}

int rampToNeutral(int current) {
  if (current < neutralPulse) {
    return constrain(current + pulseStep, minPulse, neutralPulse);
  } else if (current > neutralPulse) {
    return constrain(current - pulseStep, neutralPulse, maxPulse);
  } else {
    return current;
  }
}
