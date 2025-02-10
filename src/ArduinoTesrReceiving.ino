#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

void setup() {
  // Initialize Serial communication at 115200 baud rate
  SERIAL.begin(115200);
  while (!SERIAL) {
    // Wait for Serial to initialize (important for some boards)
  }
  
  // Print a message to confirm Arduino is ready
  SERIAL.println("Arduino is ready to receive data!");
}

void loop() {
  // Check if data is available from Raspberry Pi
  if (SERIAL.available()) {
    // Read the incoming data as a String
    String receivedData = SERIAL.readStringUntil(';'); // Read until ';' terminator
    
    // Print the received data to the Serial Monitor
    SERIAL.print("Received: ");
    SERIAL.println(receivedData);

    // Optionally, send an acknowledgment back to Raspberry Pi
    SERIAL.println("Acknowledged!");
  }

  // Small delay to avoid spamming the Serial Monitor
  delay(50);
}
