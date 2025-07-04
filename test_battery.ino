#define BAT_MON_PIN  34  // 예시 ADC 핀

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // ESP32 12bit ADC
}

void loop() {
  int raw = analogRead(BAT_MON_PIN);
  float voltage = raw * (3.3 / 4095.0) * (10.0); // 예시 스케일
  Serial.print("Battery Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");
  
  delay(1000);
}
