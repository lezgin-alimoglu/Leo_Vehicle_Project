//Required libraries
#include <mcp2515_can.h>
#include <SPI.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

// CS pin
mcp2515_can CAN(10);

const uint8_t MOTOR_1 = 101;
const uint8_t MOTOR_2 = 102;
const uint8_t MOTOR_3 = 103;
const uint8_t MOTOR_4 = 104;

const float MAX_RPM = 10000.0f;

// Some constant
const float MAX_PWM = 100000.0f;
const float MAX_CURRENT = 60000.0f;
const float MAX_VELOCITY = 100000.0f;
const float MAX_POSITION = 360000000.0f;
const float MAX_POSITION_VELOCITY = 32767.0f;
const float MIN_POSITION_VELOCITY = -32768.0f;
const float MAX_ACCELERATION = 400000.0f;

enum AKMode {
  AK_PWM = 0,
  AK_CURRENT,
  AK_CURRENT_BRAKE,
  AK_VELOCITY,
  AK_POSITION,
  AK_ORIGIN,
  AK_POSITION_VELOCITY,
};



uint32_t canId(int id, AKMode Mode_set) {
  uint32_t mode;
  mode = Mode_set;
  return uint32_t(id | mode << 8);
}

void comm_can_transmit_eid(uint32_t id, const uint8_t* data, uint8_t len) {
  uint8_t i = 0;
  if (len > 8) {
    len = 8;
  }
  uint8_t buf[len];
  for (i = 0; i < len; i++) {
    buf[i] = data[i];
  }
  CAN.sendMsgBuf(id, 1, len, buf); //Note that data frame is extended so I did write "1" in here
}

void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t* index) {
  buffer[(*index)++] = number >> 24;
  buffer[(*index)++] = number >> 16;
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}

void buffer_append_int16(uint8_t* buffer, int16_t number, int16_t* index) {
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}


void comm_can_set_duty(uint8_t controller_id, float duty) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(duty * 100000.0), &send_index);
  comm_can_transmit_eid(canId(controller_id, AKMode::AK_PWM), buffer, send_index);
}

void comm_can_set_current(uint8_t controller_id, float current) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
  comm_can_transmit_eid(canId(controller_id, AKMode::AK_CURRENT), buffer, send_index);
}

void comm_can_set_cb(uint8_t controller_id, float current) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
  comm_can_transmit_eid(canId(controller_id, AKMode::AK_CURRENT_BRAKE), buffer, send_index);
}

void comm_can_set_rpm(uint8_t controller_id, float rpm) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)rpm, &send_index);
  comm_can_transmit_eid(canId(controller_id, AKMode::AK_VELOCITY), buffer, send_index);
}

void comm_can_set_pos(uint8_t controller_id, float pos) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)(pos * 1000000.0), &send_index);
  comm_can_transmit_eid(canId(controller_id, AKMode::AK_POSITION), buffer, send_index);
}

void comm_can_set_origin(uint8_t controller_id, uint8_t set_origin_mode) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)set_origin_mode, &send_index);
  comm_can_transmit_eid(canId(controller_id, AKMode::AK_ORIGIN), buffer, send_index);
}

void comm_can_set_pos_spd(uint8_t controller_id, float pos, int16_t spd, int16_t RPA) {
  int32_t send_index = 0;
  int16_t send_index1 = 4;
  uint8_t buffer[8];
  buffer_append_int32(buffer, (int32_t)(pos * 10000.0), &send_index);
  //Check the manual from CubeMars. Maybe I should divide spd and RPA to 10.0
  //buffer_append_int16(buffer, spd/10.0, &send_index1);  
  //buffer_append_int16(buffer, RPA/10.0, &send_index1);
  buffer_append_int16(buffer, spd, &send_index1);
  buffer_append_int16(buffer, RPA, &send_index1);
  
  
  comm_can_transmit_eid(canId(controller_id, AKMode::AK_POSITION_VELOCITY), buffer, send_index);
}


void motor_receive(float* motor_pos, float* motor_spd, float* motor_cur, int8_t* motor_temp, int8_t* motor_error, uint8_t* rx_message) {
  byte len = 0;
  byte buf[8];
  unsigned long canId;

  CAN.readMsgBuf(&len, buf);
  int16_t pos_int = buf[0] << 8 | buf[1];
  int16_t spd_int = buf[2] << 8 | buf[3];
  int16_t cur_int = buf[4] << 8 | buf[5];
  *motor_pos = (float)(pos_int * 0.1f);
  *motor_spd = (float)(spd_int * 10.0f);
  *motor_cur = (float)(cur_int * 0.01f);
  *motor_temp = buf[6];
  *motor_error = buf[7];
}

void processCommand(String command) { 
  float rpm = MAX_RPM; // Default RPM for all motors
  
  // Split the command by the comma to check for RPM
  int commaIndex = command.indexOf(',');
  if (commaIndex != -1) {
    // If there is a comma, extract RPM value
    String rpmStr = command.substring(commaIndex + 1);
    rpm = rpmStr.toFloat();  // Convert RPM string to float
    command = command.substring(0, commaIndex);  // Get the base command (e.g., "move:up")
  }

  if (command == "move:up") {
    // Move up - All motors move in the same direction
    comm_can_set_rpm(MOTOR_1, rpm);
    comm_can_set_rpm(MOTOR_2, rpm);
    comm_can_set_rpm(MOTOR_3, rpm);
    comm_can_set_rpm(MOTOR_4, rpm);
  } else if (command == "move:down") {
    // Move down - Motors move in the opposite direction
    comm_can_set_rpm(MOTOR_1, -rpm);
    comm_can_set_rpm(MOTOR_2, -rpm);
    comm_can_set_rpm(MOTOR_3, -rpm);
    comm_can_set_rpm(MOTOR_4, -rpm);
  } else if (command == "move:left") {
    // Move left - Right motors forward, left motors backward
    comm_can_set_rpm(MOTOR_1, -rpm);
    comm_can_set_rpm(MOTOR_2, rpm);
    comm_can_set_rpm(MOTOR_3, -rpm);
    comm_can_set_rpm(MOTOR_4, rpm);
  } else if (command == "move:right") {
    // Move right - Left motors forward, right motors backward
    comm_can_set_rpm(MOTOR_1, rpm);
    comm_can_set_rpm(MOTOR_2, -rpm);
    comm_can_set_rpm(MOTOR_3, rpm);
    comm_can_set_rpm(MOTOR_4, -rpm);
  } else {
    // Invalid command - Stop all motors
    comm_can_set_rpm(MOTOR_1, 0);
    comm_can_set_rpm(MOTOR_2, 0);
    comm_can_set_rpm(MOTOR_3, 0);
    comm_can_set_rpm(MOTOR_4, 0);
  }
}

void setup() {

  Serial.begin(115200);
  while (!Serial) {};
  while (CAN_OK != CAN.begin(CAN_1000KBPS)) {  // init can bus : baudrate = 1000k
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN init ok!");
}


float rpm = 0.0;

void loop() {
  if (Serial.available()) {
    String command = SERIAL.readStringUntil(';'); // Komut ';' ile sonlanır
    Serial.println(command);
    processCommand(command);
  }
}
