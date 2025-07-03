// === PIN ASSIGNMENTS ===
// Sol Ön
const uint8_t A_PWM = 5;
const uint8_t A_IN1 = 2;
const uint8_t A_IN2 = 3;

// Sağ Ön
const uint8_t B_PWM = 6;
const uint8_t B_IN1 = 7;
const uint8_t B_IN2 = 8;

// Sol Arka
const uint8_t C_PWM = 9;
const uint8_t C_IN1 = 10;
const uint8_t C_IN2 = 11;

// Sağ Arka
const uint8_t D_PWM = 12;
const uint8_t D_IN1 = 13;
const uint8_t D_IN2 = 4;  // Boştaki bir pinle değiştirilebilir

int forwardSpeed = 0;
int turnSpeed = 0;

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
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, 0);
  }
}

void handleCommand(const String &cmd) {
  int c1 = cmd.indexOf(':');
  int c2 = cmd.indexOf(',');
  if (c1 < 0 || c2 < 0) return;

  String dir = cmd.substring(c1 + 1, c2);
  int val = cmd.substring(c2 + 1).toInt();

  if (dir == "up")       forwardSpeed =  val;
  else if (dir == "down")forwardSpeed = -val;
  else if (dir == "right")turnSpeed   =  val;
  else if (dir == "left") turnSpeed   = -val;

  if (forwardSpeed == 0) turnSpeed = 0;

  Serial.print("Fwd: "); Serial.print(forwardSpeed);
  Serial.print("  Turn: "); Serial.println(turnSpeed);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  pinMode(A_IN1, OUTPUT); pinMode(A_IN2, OUTPUT); pinMode(A_PWM, OUTPUT);
  pinMode(B_IN1, OUTPUT); pinMode(B_IN2, OUTPUT); pinMode(B_PWM, OUTPUT);
  pinMode(C_IN1, OUTPUT); pinMode(C_IN2, OUTPUT); pinMode(C_PWM, OUTPUT);
  pinMode(D_IN1, OUTPUT); pinMode(D_IN2, OUTPUT); pinMode(D_PWM, OUTPUT);

  Serial.println("4-Motor Kontrolcü Hazır 🚙");
}

void loop() {
  static String buffer;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == ';') {
      handleCommand(buffer);
      buffer = "";
    } else if (c != '\r' && c != '\n') {
      buffer += c;
      if (buffer.length() > 32) buffer = "";
    }
  }

  int leftOutput  = constrain(forwardSpeed + turnSpeed, -255, 255);
  int rightOutput = constrain(forwardSpeed - turnSpeed, -255, 255);

  // 4 motoru senkron sür
  driveMotor(A_IN1, A_IN2, A_PWM, leftOutput);   // Sol Ön
  driveMotor(B_IN1, B_IN2, B_PWM, rightOutput);  // Sağ Ön
  driveMotor(C_IN1, C_IN2, C_PWM, leftOutput);   // Sol Arka
  driveMotor(D_IN1, D_IN2, D_PWM, rightOutput);  // Sağ Arka
}
