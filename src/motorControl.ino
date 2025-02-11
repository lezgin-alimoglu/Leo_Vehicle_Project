#include <mcp2515_can.h>
#include <SPI.h>

// SAMD Core
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

// CS Pin
mcp2515_can CAN(10);

// Maksimum değerler
const float MAX_RPM = 10000.0f;

// Motor kimlikleri
const uint8_t MOTOR_1 = 101;
const uint8_t MOTOR_2 = 102;
const uint8_t MOTOR_3 = 103;
const uint8_t MOTOR_4 = 104;

// Fonksiyonlar
void comm_can_set_rpm(uint8_t controller_id, float rpm) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer[send_index++] = (int32_t)rpm >> 24;
  buffer[send_index++] = (int32_t)rpm >> 16;
  buffer[send_index++] = (int32_t)rpm >> 8;
  buffer[send_index++] = (int32_t)rpm;
  CAN.sendMsgBuf(canId(controller_id, AKMode::AK_VELOCITY), 1, send_index, buffer);
}

// Komutları işleme
void processCommand(String command) {
  float rpm = MAX_RPM; // Tüm motorlar için varsayılan RPM

  if (command == "move:up") {
    // Yukarı hareket - Tüm motorlar aynı yönde
    comm_can_set_rpm(MOTOR_1, rpm);
    comm_can_set_rpm(MOTOR_2, rpm);
    comm_can_set_rpm(MOTOR_3, rpm);
    comm_can_set_rpm(MOTOR_4, rpm);
  } else if (command == "move:down") {
    // Aşağı hareket - Ters yönde
    comm_can_set_rpm(MOTOR_1, -rpm);
    comm_can_set_rpm(MOTOR_2, -rpm);
    comm_can_set_rpm(MOTOR_3, -rpm);
    comm_can_set_rpm(MOTOR_4, -rpm);
  } else if (command == "move:left") {
    // Sola hareket - Sağ motorlar ileri, sol motorlar geri
    comm_can_set_rpm(MOTOR_1, -rpm);
    comm_can_set_rpm(MOTOR_2, rpm);
    comm_can_set_rpm(MOTOR_3, -rpm);
    comm_can_set_rpm(MOTOR_4, rpm);
  } else if (command == "move:right") {
    // Sağa hareket - Sol motorlar ileri, sağ motorlar geri
    comm_can_set_rpm(MOTOR_1, rpm);
    comm_can_set_rpm(MOTOR_2, -rpm);
    comm_can_set_rpm(MOTOR_3, rpm);
    comm_can_set_rpm(MOTOR_4, -rpm);
  } else {
    // Hatalı komut - Tüm motorları durdur
    comm_can_set_rpm(MOTOR_1, 0);
    comm_can_set_rpm(MOTOR_2, 0);
    comm_can_set_rpm(MOTOR_3, 0);
    comm_can_set_rpm(MOTOR_4, 0);
  }
}

void setup() {
  SERIAL.begin(115200);
  while (!SERIAL) {};

  if (CAN.begin(CAN_1000KBPS) != CAN_OK) {
    SERIAL.println("CAN BUS Shield init fail");
    while (1);
  }

  SERIAL.println("CAN init ok!");
}

void loop() {
  // Seri porttan komut oku
  if (SERIAL.available()) {
    String command = SERIAL.readStringUntil(';'); // Komut ';' ile sonlanır
    processCommand(command);
  }

  delay(10); // Ana döngü gecikmesi
}
