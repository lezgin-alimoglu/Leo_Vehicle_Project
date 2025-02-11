#include <mcp2515_can.h>
#include <SPI.h>

// SAMD çekirdek uyumluluğu
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

// CS pini
mcp2515_can CAN(10);

// Sabitler
const float MAX_RPM = 100000.0f;  // Maksimum RPM sınırı
float rpm = 0.0;                 // Başlangıç RPM değeri

// CAN mesaj kimlik hesaplama
uint32_t canId(int id, int mode) {
  return uint32_t(id | mode << 8);
}

// CAN veri gönderme fonksiyonu
void comm_can_transmit_eid(uint32_t id, const uint8_t* data, uint8_t len) {
  if (len > 8) len = 8;  // CAN verisi maksimum 8 bayt olabilir
  CAN.sendMsgBuf(id, 1, len, data);
}

// 32-bit integeri buffer'a ekle
void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t* index) {
  buffer[(*index)++] = number >> 24;
  buffer[(*index)++] = number >> 16;
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}

// RPM ayarı için CAN mesajı gönder
void comm_can_set_rpm(uint8_t controller_id, float rpm_value) {
  int32_t send_index = 0;
  uint8_t buffer[4];
  buffer_append_int32(buffer, (int32_t)rpm_value, &send_index);
  comm_can_transmit_eid(canId(controller_id, 3), buffer, send_index); // 3, Velocity modunu temsil ediyor
}

void setup() {
  // Seri haberleşme başlat
  Serial.begin(115200);
  while (!Serial) {}

  // CAN modülünü başlat
  while (CAN_OK != CAN.begin(CAN_1000KBPS)) {
    Serial.println("CAN BUS modülü başlatılamadı!");
    delay(100);
  }
  Serial.println("CAN modülü başlatıldı.");
}

void loop() {
  // Raspberry Pi'den veri oku
  if (Serial.available()) {
    char received = Serial.read();

    // Gamepad girişine göre RPM ayarla
    if (received == 'x') {
      rpm += 2500.0;
      if (rpm > MAX_RPM) rpm = MAX_RPM; // RPM'i maksimum sınırda tut
      Serial.println("RPM artırıldı.");
    } else if (received == 's') {
      rpm -= 2500.0;
      if (rpm < 0) rpm = 0; // RPM'i sıfırdan düşük tutma
      Serial.println("RPM azaltıldı.");
    }
  }

  // CAN mesajlarını gönder
  comm_can_set_rpm(104, rpm); // Motor ID'si 104
  comm_can_set_rpm(103, rpm); // Motor ID'si 103
  comm_can_set_rpm(102, rpm); // Motor ID'si 102
  comm_can_set_rpm(101, rpm); // Motor ID'si 101

  delay(10); // Motor kontrolünü stabil tutmak için kısa gecikme
}
