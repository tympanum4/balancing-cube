#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  Serial.print("AcX = "); Serial.print(ax);
  Serial.print(" | AcY = "); Serial.print(ay);
  Serial.print(" | AcZ = "); Serial.println(az);

  delay(500);
}
