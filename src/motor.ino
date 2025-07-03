// ====================================================================
//  Differential‐Drive Motor Control over Serial
//  - Expects commands like "move:up,120;" , "move:right,60;"
//  - forwardSpeed ∈ [ -255 .. +255 ] (up = +, down = -)
//  - turnSpeed    ∈ [ -255 .. +255 ] (right = +, left = -)
//  - leftMotor  = forwardSpeed + turnSpeed
//  - rightMotor = forwardSpeed - turnSpeed
//  - Only turns when forwardSpeed != 0
// ====================================================================

// === PIN ASSIGNMENT ===
// Motor A (left wheel)
const uint8_t A_PWM   = 5;  // ENA  (must be a PWM pin)
const uint8_t A_IN1   = 2;  // Direction pin 1
const uint8_t A_IN2   = 3;  // Direction pin 2

// Motor B (right wheel)
const uint8_t B_PWM   = 6;  // ENB  (must be a PWM pin)
const uint8_t B_IN1   = 7;  // Direction pin 1
const uint8_t B_IN2   = 8;  // Direction pin 2

// ----------------------------------------------------------------------------
// State variables
int forwardSpeed = 0;   // −255…+255  (negative = backward)
int turnSpeed    = 0;   // −255…+255  (negative = turn left)

// ----------------------------------------------------------------------------
// Helper: apply motor output [-255..+255]
void driveMotor(uint8_t in1, uint8_t in2, uint8_t pwmPin, int16_t value) {
  value = constrain(value, -255, 255);
  if (value > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, value);
  } else if (value < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwmPin, -value);
  } else {
    // stop
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, 0);
  }
}

// ----------------------------------------------------------------------------
// Parse incoming "move:<dir>,<val>;" strings
void handleCommand(const String &cmd) {
  // expected format: move:_<dir>_,_<val>_
  int c1 = cmd.indexOf(':');
  int c2 = cmd.indexOf(',');
  if (c1 < 0 || c2 < 0) return;
  String dir = cmd.substring(c1+1, c2);
  int val   = cmd.substring(c2+1).toInt();
  
  if (dir == "up") {
    forwardSpeed =  val;   // forward
  }
  else if (dir == "down") {
    forwardSpeed = -val;   // backward
  }
  else if (dir == "right") {
    turnSpeed =  val;      // turn right
  }
  else if (dir == "left") {
    turnSpeed = -val;      // turn left
  }
  
  // if no forward motion, zero turn
  if (forwardSpeed == 0) {
    turnSpeed = 0;
  }
  
  // echo for debug
  Serial.print("Fwd: "); Serial.print(forwardSpeed);
  Serial.print("  Turn: "); Serial.println(turnSpeed);
}

// ----------------------------------------------------------------------------
// Setup
void setup() {
  Serial.begin(115200);
  while (!Serial) { /* wait for USB Serial */ }

  pinMode(A_IN1, OUTPUT);
  pinMode(A_IN2, OUTPUT);
  pinMode(A_PWM,  OUTPUT);
  
  pinMode(B_IN1, OUTPUT);
  pinMode(B_IN2, OUTPUT);
  pinMode(B_PWM,  OUTPUT);

  Serial.println("Motor controller ready.");
}

// ----------------------------------------------------------------------------
// Loop
void loop() {
  static String buffer;
  
  // --- Serial receive ---
  while (Serial.available()) {
    char c = Serial.read();
    if (c == ';') {
      handleCommand(buffer);
      buffer = "";
    } 
    else if (c != '\r' && c != '\n') {
      buffer += c;
      // guard max length
      if (buffer.length() > 32) buffer = "";
    }
  }

  // --- Compute differential outputs ---
  int leftOutput  = forwardSpeed + turnSpeed;
  int rightOutput = forwardSpeed - turnSpeed;

  // constrain final outputs
  leftOutput  = constrain(leftOutput,  -255, 255);
  rightOutput = constrain(rightOutput, -255, 255);

  // --- Drive Motors ---
  driveMotor(A_IN1, A_IN2, A_PWM, leftOutput);
  driveMotor(B_IN1, B_IN2, B_PWM, rightOutput);
}
