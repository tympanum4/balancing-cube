#define RXD2 44  // HC-06 TX → ESP32 RX
#define TXD2 43  // HC-06 RX ← ESP32 TX

void setup() {
  Serial.begin(115200);  // PC 모니터 확인용
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  // HC-06 통신 (보통 9600)

  Serial.println("Bluetooth Serial Bridge 시작");
}

void loop() {
  // 스마트폰 → HC-06 → ESP32
  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.write(c);  // PC 시리얼 모니터에 출력

    // 여기서 원하는 처리를 할 수도 있음
    // 예: 명령어 해석, LED 제어 등
  }

  // ESP32 → HC-06 → 스마트폰
  if (Serial.available()) {
    char c = Serial.read();
    Serial2.write(c);  // 스마트폰으로 전송
  }
}
