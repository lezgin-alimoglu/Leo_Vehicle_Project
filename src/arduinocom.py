#include <mcp2515_can.h>
#include <SPI.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

mcp2515_can CAN(10);  // CS pin

const float MAX_RPM = 10000.0f;

// Function declarations
void comm_can_set_rpm(uint8_t controller_id, float rpm);

// Setup
void setup() {
  SERIAL.begin(115200);
  while (!SERIAL) {}

  if (CAN.begin(CAN_1000KBPS) != CAN_OK) {
    SERIAL.println("CAN BUS Shield init fail");
    while (1);
  }
  SERIAL.println("CAN init ok!");
}

// Main loop
void loop() {
  if (SERIAL.available()) {
    String command = SERIAL.readStringUntil(';');  // Read command until ';'
    parseCommand(command);  // Process the command
  }
}

// Parse and process the command
void parseCommand(String command) {
  if (command.startsWith("rpm:")) {
    // Parse RPM command
    int separator = command.indexOf(',');
    String id_str = command.substring(4, separator);
    String rpm_str = command.substring(separator + 1);

    uint8_t motor_id = id_str.toInt();    // Extract motor ID
    float rpm = rpm_str.toFloat();        // Extract RPM value

    if (rpm > MAX_RPM) rpm = MAX_RPM;     // Clamp RPM to max limit
    if (rpm < -MAX_RPM) rpm = -MAX_RPM;

    comm_can_set_rpm(motor_id, rpm);
  } else if (command.startsWith("pos:")) {
    // Handle position commands (optional)
    SERIAL.println("Position command received");
  }
}

// Send RPM command to motor via CAN
void comm_can_set_rpm(uint8_t controller_id, float rpm) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer[send_index++] = (int32_t)rpm >> 24;
  buffer[send_index++] = (int32_t)rpm >> 16;
  buffer[send_index++] = (int32_t)rpm >> 8;
  buffer[send_index++] = (int32_t)rpm;

  uint32_t can_id = controller_id | (AK_VELOCITY << 8);
  CAN.sendMsgBuf(can_id, 1, send_index, buffer);  // Extended CAN frame
}
