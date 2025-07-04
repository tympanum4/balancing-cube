#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>

// MOTORS
#define M1_PWM     42 // 48 11
#define M1_DIR     40 // 21 9
#define M1_START   41 // 47 10
#define M2_PWM     48
#define M2_DIR     21
#define M2_START   47
#define M3_PWM     11
#define M3_DIR     9
#define M3_START   10
// I2C
#define MPU_SCL    7
#define MPU_SDA    15
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCL        5
#define SDA        6
// ETC
#define BAT_mon    4
#define buzzer     18

// READY FOR DISPLAY, GYRO
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);

// ETC
float loop_time = 10;
unsigned long currentT, previousT_1, previousT_2 = 0;

// ANGLE VALUES
float pitch = 0.0, roll = 0.0, yaw = 0.0;
float accelOffsetX, accelOffsetY;
float gyroOffsetX, gyroOffsetY;
float gx, gy, gz, accelAngleX, accelAngleY;
//
int pwm = 0;
int32_t motor_speed = 0;
float gyroZfilt = 0;
bool vertical = false; 
// SPECIFIC VALUES
float alpha = 0.9;
float X1 = 40.0; 
float X2 = 4.25;   
float X3 = 0.04;  

void calibrateAccelAngleOffsets(int samples);
void calibrateGyroOffsets(int samples);
void angle_calc();
int Tuning();
void printValues();


void setup() {
  Serial.begin(115200);

  // MPU6050
  Wire.begin(MPU_SDA, MPU_SCL);
  if (!mpu.begin(0x68, &Wire)) {
    Serial.println("MPU6050 연결 실패");
    while (1);
  }
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
  //
  ledcAttachChannel(M1_PWM, 1000, 8, 0);
  ledcAttachChannel(M2_PWM, 1000, 8, 1);
  ledcAttachChannel(M3_PWM, 1000, 8, 2);
  // BREAK
  digitalWrite(M1_START, HIGH);
  digitalWrite(M2_START, HIGH);
  digitalWrite(M3_START, HIGH);

  // ETC
  pinMode(BAT_mon, INPUT);
  pinMode(buzzer, OUTPUT);

  // 보정
  calibrateAccelAngleOffsets(500);
  calibrateGyroOffsets(500);
}

void loop() {
  Tuning(); 
  angle_calc();

  digitalWrite(M1_START, LOW);
  float speed = angleY;

  //Serial.print("  speed : ");
  //Serial.println(speed);

  if (speed <= 0) {
    pwm = -speed;
    digitalWrite(M1_DIR, LOW);
    ledcWriteChannel(0, pwm);
  } else {
    pwm = speed;
    digitalWrite(M1_DIR, HIGH);
    ledcWriteChannel(0, pwm);
  }

  delay(20);
}
