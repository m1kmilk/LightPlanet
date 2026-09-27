/*
  虚拟星球光影系统 - Arduino Uno 程序
  接线：模拟光线传感器 OUT -> A0，VCC -> 5V，GND -> GND
  串口：9600 波特率；每一行只发送一个 0~1023 的数字。
*/

const byte LIGHT_PIN = A0;
const unsigned long SEND_INTERVAL = 50; // 20 次/秒，网页动画足够平滑

float filteredValue = 0;                // 指数滑动平均后的数值
int lastSentValue = -1;
unsigned long lastSendTime = 0;

void setup() {
  pinMode(LIGHT_PIN, INPUT);
  Serial.begin(9600);

  // 先读一次作为滤波起点，避免上电后的数值突然跳变。
  filteredValue = analogRead(LIGHT_PIN);
}

void loop() {
  int rawValue = analogRead(LIGHT_PIN); // 原始 ADC 值：0~1023

  // 简单指数滤波：新读数占 20%，历史读数占 80%，可明显减小抖动。
  filteredValue = filteredValue * 0.80 + rawValue * 0.20;
  int stableValue = constrain((int)(filteredValue + 0.5), 0, 1023);

  // 固定节奏发送；数值变化很小时沿用上次值，避免网页显示轻微跳动。
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    if (lastSentValue < 0 || abs(stableValue - lastSentValue) >= 2) {
      lastSentValue = stableValue;
    }
    Serial.println(lastSentValue); // 严格只输出数字，方便 Web Serial 按行解析
    lastSendTime = millis();
  }
}
