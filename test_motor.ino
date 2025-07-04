#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>

#define M1_PWM     42 // 48 11
#define M1_DIR     40 // 21 9
#define M1_START   41 // 47 10

#define M2_PWM     48
#define M2_DIR     21
#define M2_START   47

#define M3_PWM     11
#define M3_DIR     9
#define M3_START   10

#define led        8
#define buzzer     18

#define MPU_SCL    7
#define MPU_SDA    15

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCL        5
#define SDA        6

#define BAT_mon    4
///////////////////////////////////

// MPU6050
Adafruit_MPU6050 mpu;

// 디스플레이
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);


float alpha = 0.98;

float pitch = 0.0, roll = 0.0, yaw = 0.0;
float zero_pitch = 0.0, zero_roll = 0.0;
float gyro_x_offset = 0, gyro_y_offset = 0, gyro_z_offset = 0;
unsigned long lastTime = 0;
unsigned long bootTime = 0;


void calibrateGyro() {
  const int samples = 500;
  float sum_x = 0, sum_y = 0, sum_z = 0;
  sensors_event_t a, g, t;

  //Serial.println("자이로 보정 중... 가만히 두세요!");

  for (int i = 0; i < samples; i++) {
    mpu.getEvent(&a, &g, &t);
    sum_x += g.gyro.x;
    sum_y += g.gyro.y;
    sum_z += g.gyro.z;
    delay(5);
  }

  gyro_x_offset = sum_x / samples;
  gyro_y_offset = sum_y / samples;
  gyro_z_offset = sum_z / samples;

  tone(buzzer, 200);
  delay(300);
  noTone(buzzer);
  //Serial.println("보정 완료!");
}

void calibrateInitialAngle() {
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  zero_pitch = atan2(-a.acceleration.x, a.acceleration.z) * 180.0 / PI;
  zero_roll  = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
}

void setup() {
  Serial.begin(9600);

  // MPU6050
  Wire.begin(MPU_SDA, MPU_SCL);
  if (!mpu.begin(0x68, &Wire)) {
    Serial.println("MPU6050 연결 실패");
    while (1);
  }

  mpu.setAccelerometerRange((mpu6050_accel_range_t)2);
  mpu.setGyroRange((mpu6050_gyro_range_t)1);
  mpu.setFilterBandwidth((mpu6050_bandwidth_t)5);

  // DISPLAY
  Wire1.begin(SDA, SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("디스플레이 실패");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  
  // MOTOR
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_START, OUTPUT);

  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_START, OUTPUT);

  pinMode(M3_PWM, OUTPUT);
  pinMode(M3_DIR, OUTPUT);
  pinMode(M3_START, OUTPUT);

  ledcAttachChannel(M1_PWM, 1000, 8, 0);
  ledcAttachChannel(M2_PWM, 1000, 8, 1);
  ledcAttachChannel(M3_PWM, 1000, 8, 2);
  ledcAttachChannel(led, 1000, 8, 3);
  
  digitalWrite(M1_START, HIGH);
  digitalWrite(M2_START, HIGH);
  digitalWrite(M3_START, HIGH);

  pinMode(BAT_mon, INPUT);
  pinMode(buzzer, OUTPUT);

  calibrateGyro(); // 자이로 보정
  //calibrateInitialAngle();  // 초기 기울기 기준 보정

  lastTime = millis();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  bool ignoreGyro = (now - bootTime < 2000);

  // 오프셋 제거
  float gx = g.gyro.x - gyro_x_offset;
  float gy = g.gyro.y - gyro_y_offset;
  float gz = g.gyro.z - gyro_z_offset;

  if (ignoreGyro) {
    gx = 0; gy = 0; gz = 0;
  }

  // 자이로 변화량 (deg/s → deg)
  float gyro_pitch = gx * 180.0 / PI * dt;
  float gyro_roll  = gy * 180.0 / PI * dt;
  float gyro_yaw   = gz * 180.0 / PI * dt;

  // 가속도 기반 Roll, Pitch
  float acc_pitch = atan2(-a.acceleration.x, a.acceleration.z) * 180.0 / PI - zero_pitch;
  float acc_roll  = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI - zero_roll;

  // 보완 필터 적용
  pitch = alpha * (pitch + gyro_pitch) + (1 - alpha) * acc_pitch;
  roll  = alpha * (roll  + gyro_roll ) + (1 - alpha) * acc_roll;
  yaw   += gyro_yaw;

  // 출력
  Serial.print("Pitch: "); Serial.print(pitch, 2);
  Serial.print(" deg,  Roll: "); Serial.print(roll, 2);
  Serial.print(" deg,  Yaw: "); Serial.println(yaw, 2);

  float vol = analogRead(BAT_mon);
  vol = vol / 1305 * 12;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Pitch: "); display.print(pitch, 1); display.println(" deg");
  display.print("Roll : "); display.print(roll, 1); display.println(" deg");
  display.print("Yaw  : "); display.print(yaw, 1); display.println(" deg");
  display.print("BATTERY : "); display.println(vol);
  display.display();

  delay(50);

////////////////////

/*
  digitalWrite(M1_START, LOW);
  digitalWrite(M2_START, LOW);
  digitalWrite(M3_START, LOW);

  digitalWrite(M1_DIR, HIGH);
  digitalWrite(M2_DIR, HIGH);
  digitalWrite(M3_DIR, HIGH);
  ledcWriteChannel(0, 20);
  ledcWriteChannel(1, 20);
  ledcWriteChannel(2, 20);
  delay(2000);

  digitalWrite(M1_DIR, LOW);
  digitalWrite(M2_DIR, LOW);
  digitalWrite(M3_DIR, LOW);
  ledcWriteChannel(0, 100);
  ledcWriteChannel(1, 100);
  ledcWriteChannel(2, 100);
  delay(2000);

  digitalWrite(M1_DIR, HIGH);
  digitalWrite(M2_DIR, HIGH);
  digitalWrite(M3_DIR, HIGH);
  ledcWriteChannel(0, 200);
  ledcWriteChannel(1, 200);
  ledcWriteChannel(2, 200);
  delay(2000);

  digitalWrite(M1_START, HIGH);

///////////////

  ledcWriteChannel(3, 128);
  delay(500);

  ledcWriteChannel(3, 0);
  delay(500);
*/
}
