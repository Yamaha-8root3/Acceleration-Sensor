#include <M5Stack.h>
// #define RAD_TO_DEG 57.295779513082320876798154814105

float pitch, roll, yaw;  // 姿勢
// float pitch2, roll2;

float PRYOffset[3] = {0, 0, 0};  // 姿勢オフセット[pitch,roll,yaw]

int mode = 0;

void OffsetMpu6886();
void GetData();

void setup() {
  M5.begin();
  M5.Mpu6886.Init();
  M5.Mpu6886.setAccelFsr(M5.Mpu6886.AFS_4G);
  M5.Mpu6886.setGyroFsr(M5.Mpu6886.GFS_1000DPS);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
}

void OffsetMpu6886() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  int i = 0;
  float pitch_, roll_, yaw_;  // 姿勢(offset)
  while (i < 600) {
    M5.Mpu6886.getAhrsData(&pitch_, &roll_, &yaw_);
    PRYOffset[0] += pitch;
    PRYOffset[1] += roll_;
    PRYOffset[2] += yaw_;
    i++;
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("calibration:%d/600", i);
    delay(50);
  }
  PRYOffset[0] /= i;
  PRYOffset[1] /= i;
  PRYOffset[2] /= i;
  M5.Lcd.fillScreen(BLACK);
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    OffsetMpu6886();
  } else if (M5.BtnB.wasPressed()) {
    M5.Lcd.fillScreen(BLACK);
    mode = (mode + 1) % 2;
  }

  GetData();

  if (mode == 0) {
    M5.lcd.setCursor(0, 30);
    // M5.lcd.printf("Accel X:%7.2f\n      Y:%7.2f\n      Z:%7.2f\n", accX, accY, accZ);
    // M5.lcd.printf("Gyro  X:%7.2f\n      Y:%7.2f\n      Z:%7.2f\n", gyroX, gyroY, gyroZ);
    M5.lcd.printf("Ahrs  Pitch:%7.2f\n      Roll :%7.2f\n      Yaw  :%7.2f\n\n", pitch, roll, yaw);
    // M5.Lcd.printf("      Pitch:%7.2f\n      Roll :%7.2f", pitch2, roll2);
  } else if (mode == 1) {
    M5.lcd.fillScreen(BLACK);
    M5.lcd.drawCircle(160, 120, 90, WHITE);
    M5.lcd.drawCircle(160, 120, 60, WHITE);
    M5.lcd.drawCircle(160, 120, 30, WHITE);
    M5.lcd.drawLine(70, 120, 250, 120, WHITE);
    M5.lcd.drawLine(160, 30, 160, 210, WHITE);
    M5.Lcd.fillCircle(pitch * 30 + 160, roll * 30 + 120, 3, WHITE);
  }

  delay(10);
}

void GetData() {
  float PRY1[3];
  int i = 0;
  while (i < 10) {
    M5.Mpu6886.getAhrsData(&pitch, &roll, &yaw);
    PRY1[0] += (pitch - PRYOffset[0]);
    PRY1[1] += (roll - PRYOffset[1]);
    PRY1[2] += (yaw - PRYOffset[2]);
    i++;
    delay(10);
  }
  pitch = PRY1[0] / i;
  roll = PRY1[1] / i;
  yaw = PRY1[2] / i;
}