#include <Wire.h>
#include <MPU6050.h>
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
// BLUETOOTH
#define RXD2       44
#define TXD2       43
// ETC
#define BAT_mon    4
#define buzzer     18
#define LED        8


// READY FOR DISPLAY, GYRO
MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);

// ETC
float loop_time = 10;
unsigned long currentT, previousT_1;
// RAW ANGLE VALUES
int16_t rawAccX, rawAccY, rawAccZ;
int16_t rawGyroX, rawGyroY, rawGyroZ;
// ANGLE VALUES
float gyroY;
float robot_angle = 0;
float acc_angle = 0;
float accelOffsetY, gyroOffsetY;
// ABOUT ANGLE
int32_t motor_speed = 0;
float speed = 0;
float gyroYfilt = 0;
float elapsedTime = 10;
// SPECIFIC VALUES
float alpha = 0.40;
float Gyro_amount = 0.996;
float X1 = 75.0; 
float X2 = 5.25;   
float X3 = 0.00;  


void calibrateGyroOffsets(int samples);
void angle_calc();
int Tuning();
void printValues();


void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 

  // MPU6050
  Wire.begin(MPU_SDA, MPU_SCL);

  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_500);
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  
  if (!mpu.testConnection()) {
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
  pinMode(LED, OUTPUT);

  // 보정
  calibrateGyroOffsets(1024);
}

void loop() {
  //Tuning();
  angle_calc();

  digitalWrite(M2_START, LOW); // motor on

  gyroY = rawGyroY / 131.0; // Convert to deg/s
      
  gyroYfilt = alpha * gyroY + (1 - alpha) * gyroYfilt;
  speed = constrain(X1 * robot_angle + X2 * gyroYfilt + X3 * -motor_speed, -255, 255); 

  
  Serial.print("  speed : ");
  Serial.println(speed);


  motor_speed += speed;

  if (speed <= 0) {
    speed = -speed;
    digitalWrite(M2_DIR, LOW);
    ledcWriteChannel(1, speed);
  } else {
    digitalWrite(M2_DIR, HIGH);
    ledcWriteChannel(1, speed);
  }

  delay(10);
}
