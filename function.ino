// 자이로 바이어스 보정 (드리프트 제거용)
void calibrateGyroOffsets(int samples = 1024) {
  float sumGY = 0.0;
  Serial.println("자이로 바이어스 보정 중...");

  for (int i = 0; i < samples; i++) {
    mpu.getRotation(&rawGyroX, &rawGyroY, &rawGyroZ);
    Serial.println(rawGyroY);

    sumGY += rawGyroY;
    delay(5);
  }

  gyroOffsetY = sumGY / samples;

  Serial.print("gyroOffsetY: "); Serial.println(gyroOffsetY, 2);
  digitalWrite(LED, HIGH);
  delay(3000);
  digitalWrite(LED, LOW);
}

void angle_calc() {
  // MPU6050 센서 데이터 읽기 (정수형)
  mpu.getAcceleration(&rawAccX, &rawAccY, &rawAccZ);
  mpu.getRotation(&rawGyroX, &rawGyroY, &rawGyroZ);

  rawGyroY -= gyroOffsetY;

  robot_angle = robot_angle + rawGyroY * elapsedTime / 1000 / 65.536;
  acc_angle = atan2(-rawAccX, rawAccZ) * 57.2958; // deg/rad
  acc_angle -= accelOffsetY;

  Serial.print(" | acc_angle: ");
  Serial.print(acc_angle);

  // 보정 필터로 최종 각도 계산
  robot_angle = robot_angle * Gyro_amount + acc_angle * (1 - Gyro_amount) ;

  //Serial.print(" | Robot Angle: ");
  //Serial.print(robot_angle);
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
  while (Serial2.available() >= 1) {  // 두 글자 받을 수 있을 때만  P=
    char param = Serial.read();
    //char cmd = Serial.read();

    switch (param) {
      case 'q':
        X1 += 1;
        printValues();
        break;
      case 'w':
        X1 -= 1;
        printValues();
        break;
      case 'e':
        X2 += 0.01;
        printValues();
        break;
      case 'r':
        X2 -= 0.01;
        printValues();
        break;
      case 't':
        X3 += 0.005;
        printValues();
        break;
      case 'y':
        X3 -= 0.005;
        printValues();
        break;
    }
  // 튜닝할 때마다 출력
  }

  return 0;  // 계속 loop 돌게 하기
}

void printValues() {
  Serial.print("X1: "); Serial.print(X1);
  Serial.print(" X2: "); Serial.print(X2);
  Serial.print(" X3: "); Serial.println(X3, 3);
  delay(300); 
}