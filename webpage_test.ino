#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Junho A24";
const char* password = "11111111";

Adafruit_MPU6050 mpu;
AsyncWebServer server(80);

float angleX = 0.0, angleY = 0.0;
unsigned long lastTime = 0;

float Kp = 1.0;
float Ki = 0.0;
float Kd = 0.0;

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>MPU6050 Angle & PID</title>
  <meta charset="UTF-8">
  <script src="/chart.js"></script>
</head>
<body>
  <h2>MPU6050 Angle Measurement</h2>
  <p>X-axis angle: <span id="ax">0</span></p>
  <p>Y-axis angle: <span id="ay">0</span></p>

  <canvas id="angleChart" width="400" height="200"></canvas>

  <h3>PID Control</h3>
  <label>P: <input type="number" step="0.1" id="kp" value="1.0"></label><br>
  <label>I: <input type="number" step="0.01" id="ki" value="0.0"></label><br>
  <label>D: <input type="number" step="0.01" id="kd" value="0.0"></label><br>
  <button onclick="setPID()">Apply PID</button>  

  <script>
    console.log("Script loaded");
    const ctx = document.getElementById('angleChart').getContext('2d');
    const chartData = {
      labels: [],
      datasets: [
        {
          label: 'X Angle',
          borderColor: 'red',
          data: [],
          fill: false,
        },
        {
          label: 'Y Angle',
          borderColor: 'blue',
          data: [],
          fill: false,
        }
      ]
    };

    const angleChart = new Chart(ctx, {
      type: 'line',
      data: chartData,
      options: {
        animation: false,
        responsive: true,
        scales: {
          x: { display: false },
          y: {
            min: -90,
            max: 90
          }
        }
      }
    });

    function updateChart(ax, ay) {
      if (chartData.labels.length > 100) {
        chartData.labels.shift();
        chartData.datasets[0].data.shift();
        chartData.datasets[1].data.shift();
      }
      chartData.labels.push('');
      chartData.datasets[0].data.push(ax);
      chartData.datasets[1].data.push(ay);
      angleChart.update();
    }

    setInterval(() => {
      fetch('/gyro')
        .then(res => res.json())
        .then(data => {
          document.getElementById('ax').textContent = data.angleX.toFixed(2);
          document.getElementById('ay').textContent = data.angleY.toFixed(2);

          console.log(data.angleX, data.angleY);
          updateChart(data.angleX, data.angleY);
        })
        .catch(err => console.error("Fetch error: ", err));
    }, 200);

    function setPID() {
      const kp = document.getElementById('kp').value;
      const ki = document.getElementById('ki').value;
      const kd = document.getElementById('kd').value;
      fetch(`/setpid?kp=${kp}&ki=${ki}&kd=${kd}`)
        .then(() => alert("PID values are applied"));
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  if (!mpu.begin()) {
    Serial.println("MPU6050 초기화 실패");
    while (1);
  }

  mpu.setAccelerometerRange((mpu6050_accel_range_t)2);
  mpu.setGyroRange((mpu6050_gyro_range_t)1);
  mpu.setFilterBandwidth((mpu6050_bandwidth_t)5);

  WiFi.begin(ssid, password);
  Serial.print("WiFi 연결 중...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi 연결됨!");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlPage);
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(204);
  });

  server.on("/gyro", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.printf("Sending angles - X: %.2f, Y: %.2f\n", angleX, angleY);

    String json = "{";
    json += "\"angleX\":" + String(angleX, 2) + ",";
    json += "\"angleY\":" + String(angleY, 2);
    json += "}";
    request->send(200, "application/json", json);
  });

  server.on("/setpid", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (request->hasParam("kp")) Kp = request->getParam("kp")->value().toFloat();
    if (request->hasParam("ki")) Ki = request->getParam("ki")->value().toFloat();
    if (request->hasParam("kd")) Kd = request->getParam("kd")->value().toFloat();

    Serial.printf("Modified PID value - Kp: %.2f, Ki: %.2f, Kd: %.2f\n", Kp, Ki, Kd);
    request->send(200, "text/plain", "PID updated");
  });

  server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/javascript", chartJs);
  });

  server.begin();
  lastTime = millis();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  float accelAngleX = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  float accelAngleY = atan2(-a.acceleration.x, a.acceleration.z) * 180 / PI;

  float gyroRateX = g.gyro.x * 180 / PI;
  float gyroRateY = g.gyro.y * 180 / PI;

  angleX = 0.98 * (angleX + gyroRateX * dt) + 0.02 * accelAngleX;
  angleY = 0.98 * (angleY + gyroRateY * dt) + 0.02 * accelAngleY;

  //Serial.printf("X: %.2f Y: %.2f\n", angleX, angleY);
  delay(20); // 50Hz
}
