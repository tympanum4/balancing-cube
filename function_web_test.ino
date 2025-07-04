

/*

<script src="https://cdn.jsdelivr.net/npm/chart.js@2.9.4"></script>

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

void writeTo(byte device, byte address, byte value) {
    Wire.beginTransmission(device);
    Wire.write(address);
    Wire.write(value);
    Wire.endTransmission(true);
}

void angle_setup() {
    delay(100);
    writeTo(MPU6050, PWR_MGMT_1, 0);
    writeTo(MPU6050, ACCEL_CONFIG, accSens << 3);
    writeTo(MPU6050, GYRO_CONFIG, gyroSens << 3);
    delay(100);

    for (int i = 0; i < 1024; i++) {
        angle_calc();
        GyZ_offset_sum += GyZ;
        delay(5);
    }
    GyZ_offset = GyZ_offset_sum >> 10;
    Serial.print("GyZ offset value = "); Serial.println(GyZ_offset);
}

void angle_calc() {
    Wire.beginTransmission(MPU6050);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050, 4, true);
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();

    Wire.beginTransmission(MPU6050);
    Wire.write(0x47);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050, 2, true);
    GyZ = Wire.read() << 8 | Wire.read();

    AcX += AcX_offset;
    AcY += AcY_offset;  
    AcZ += AcZ_offset;
    GyZ -= GyZ_offset;

}
*/


/*
<head>
    <title>Angle Display</title>
    <meta charset="UTF-8">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script> <!-- Chart.js 추가 -->
  </head>
  <body>
    <h2>MPU6050 angle measurement</h2>
    <p>X-axis angle: <span id="ax">0</span></p>
    <p>Y-axis angle: <span id="ay">0</span></p>

    <!-- 그래프용 캔버스 -->
    <canvas id="angleChart" width="400" height="200"></canvas>
*/

/*
// HTML 페이지
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Angle Display</title>
  <meta charset="UTF-8">
</head>
<body>
  <h2>MPU6050 angle measurement</h2>
  <p>X-axis angle: <span id="ax">0</span></p>
  <p>Y-axis angle: <span id="ay">0</span></p>

  <h3>LED Control</h3>
  <button onclick="toggleLED('on')">LED ON</button>
  <button onclick="toggleLED('off')">LED OFF</button>

  <h3>PID control</h3>
  <label>P: <input type="number" step="0.1" id="kp" value="1.0"></label><br>
  <label>I: <input type="number" step="0.01" id="ki" value="0.0"></label><br>
  <label>D: <input type="number" step="0.01" id="kd" value="0.0"></label><br>
  <button onclick="setPID()">Apply PID</button>  

  <script>
    setInterval(() => {
      fetch('/gyro')
        .then(res => res.json())
        .then(data => {
          document.getElementById('ax').textContent = data.angleX.toFixed(2);
          document.getElementById('ay').textContent = data.angleY.toFixed(2);
        });
    }, 200);

    function toggleLED(state) {
      fetch(`/led?state=${state}`);
    }

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
*/