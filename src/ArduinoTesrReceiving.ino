void loop() {
  if (Serial.available()) {
    char received = Serial.read();
    if (received == 'x') {
      Serial.println("Arduino is connected!");
    }
  }
}
