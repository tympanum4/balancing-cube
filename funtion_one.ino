// 가속도 기반 초기 각도 오프셋 보정
void calibrateAccelAngleOffsets(int samples = 500) {
  float sumX = 0.0, sumY = 0.0;
  Serial.println("가속도 각도 오프셋 보정 중...");

  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float accX = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
    float accY = atan2(-a.acceleration.x, a.acceleration.z) * 180.0 / PI;

    sumX += accX;
    sumY += accY;

    delay(5);
  }

  accelOffsetX = sumX / samples;
  accelOffsetY = sumY / samples;

  Serial.print("accelOffsetX: "); Serial.println(accelOffsetX, 2);
  Serial.print("accelOffsetY: "); Serial.println(accelOffsetY, 2);
}

// 자이로 바이어스 보정 (드리프트 제거용)
void calibrateGyroOffsets(int samples = 500) {
  float sumGX = 0.0, sumGY = 0.0;
  Serial.println("자이로 바이어스 보정 중...");

  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    sumGX += g.gyro.x;
    sumGY += g.gyro.y;

    delay(5);
  }

  gyroOffsetX = sumGX / samples;
  gyroOffsetY = sumGY / samples;

  Serial.print("gyroOffsetX: "); Serial.println(gyroOffsetX, 6);
  Serial.print("gyroOffsetY: "); Serial.println(gyroOffsetY, 6);
}

void angle_calc() {
  currentTime = millis() / 1000.0;
  elapsedTime = currentTime - previousTime;
  previousTime = currentTime;

  // MPU6050 센서 데이터 읽기 (정수형)
  mpu.getAcceleration(&rawAccX, &rawAccY, &rawAccZ);
  mpu.getRotation(&rawGyroX, &rawGyroY, &rawGyroZ);

  // float 단위로 변환 (감도 값 사용: 16384 for accel, 131 for gyro)
  accX = rawAccX / 16384.0;
  accY = rawAccY / 16384.0;
  accZ = rawAccZ / 16384.0;

  gyroX = rawGyroX / 131.0;
  gyroY = rawGyroY / 131.0;
  gyroZ = rawGyroZ / 131.0;

  // 가속도 기반 각도 계산 (X = Roll, Y = Pitch)
  accAngleX = atan2(accY, sqrt(accX * accX + accZ * accZ)) * 180 / PI;
  accAngleY = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * 180 / PI;

  // 자이로 누적값 (초 단위 시간 보정)
  gyroAngleX += gyroX * elapsedTime;
  gyroAngleY += gyroY * elapsedTime;

  // 보정 필터로 최종 각도 계산
  angleX = alpha * (angleX + gyroAngleX) + (1 - alpha) * accAngleX;
  angleY = alpha * (angleY + gyroAngleY) + (1 - alpha) * accAngleY;

  // 결과 출력
  Serial.print("Angle X (Roll): ");
  Serial.print(angleX);
  Serial.print(" | Angle Y (Pitch): ");
  Serial.println(angleY);

  delay(10);
}

void battVoltage(double voltage) {
  // 아직 필요없음
}

void setPWM(uint16_t dutyCycle) { // dutyCycle is a value between 0-ICR1
  // 필요할 지 모르겠음
}

void Motor_control(int pwm) {

}

int Tuning() {
  while (Serial.available() >= 2) {  // 두 글자 받을 수 있을 때만
    char param = Serial.read();
    char cmd = Serial.read();

    switch (param) {
      case 'p':
        if (cmd == '+')    X1 += 1;
        if (cmd == '-')    X1 -= 1;
        break;
      case 'i':
        if (cmd == '+')    X2 += 0.01;
        if (cmd == '-')    X2 -= 0.01;
        break;
      case 's':
        if (cmd == '+')    X3 += 0.005;
        if (cmd == '-')    X3 -= 0.005;
        break;
    }

    printValues();  // 튜닝할 때마다 출력
  }

  return 0;  // 계속 loop 돌게 하기
}

void printValues() {
  Serial.print("X1: "); Serial.print(X1);
  Serial.print(" X2: "); Serial.print(X2);
  Serial.print(" X3: "); Serial.println(X3, 3);
  delay(300); 
}