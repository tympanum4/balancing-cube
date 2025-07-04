#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;

#define SDA_PIN 15
#define SCL_PIN 7
#define MPU_INT_PIN 45

void setup() {
  //Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  
  mpu.initialize();
  //Serial.println(mpu.testConnection() ? "MPU6050 OK" : "MPU6050 FAIL");
  pinMode(MPU_INT_PIN, INPUT);
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.printf("A: %d %d %d  G: %d %d %d\n", ax, ay, az, gx, gy, gz);
  delay(500);
}
