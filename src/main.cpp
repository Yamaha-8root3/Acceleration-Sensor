#include <M5Stack.h>
#define RAD_TO_DEG 57.295779513082320876798154814105

float accX, accY, accZ;     // 加速度
float gyroX, gyroY, gyroZ;  // 角速度
float pitch, roll, yaw;     // 姿勢
float pitch2, roll2;

float AccOffset[3] = {0, 0, 0};   // 加速度オフセット[x,y,z]
float GyroOffset[3] = {0, 0, 0};  // 角速度オフセット[x,y,z]
float PRYOffset[3] = {0, 0, 0};   // 姿勢オフセット[pitch,roll,yaw]

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
  float accX_, accY_, accZ_;     // 加速度(offset)
  float gyroX_, gyroY_, gyroZ_;  // 角速度(offset)
  float pitch_, roll_, yaw_;     // 姿勢(offset)
  while (i < 600) {
    M5.Mpu6886.getAccelData(&accX_, &accY_, &accZ_);
    M5.Mpu6886.getGyroData(&gyroX_, &gyroY_, &gyroZ_);
    M5.Mpu6886.getAhrsData(&pitch_, &roll_, &yaw_);
    AccOffset[0] += accX_;
    AccOffset[1] += accY_;
    AccOffset[2] += accZ_;
    GyroOffset[0] += gyroX_;
    GyroOffset[1] += gyroY_;
    GyroOffset[2] += gyroZ_;
    PRYOffset[0] += pitch;
    PRYOffset[1] += roll_;
    PRYOffset[2] += yaw_;
    i++;
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("calibration:%d/600", i);
    delay(100);
  }
  AccOffset[0] /= i;
  AccOffset[1] /= i;
  AccOffset[2] /= i;
  GyroOffset[0] /= i;
  GyroOffset[1] /= i;
  GyroOffset[2] /= i;
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
    M5.lcd.printf("Accel X:%7.2f\n      Y:%7.2f\n      Z:%7.2f\n", accX, accY, accZ);
    M5.lcd.printf("Gyro  X:%7.2f\n      Y:%7.2f\n      Z:%7.2f\n", gyroX, gyroY, gyroZ);
    M5.lcd.printf("Ahrs  Pitch:%7.2f\n      Roll :%7.2f\n      Yaw  :%7.2f\n\n", pitch, roll, yaw);
    M5.Lcd.printf("      Pitch:%7.2f\n      Roll :%7.2f", pitch2, roll2);
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
  float acc[3], gyro[3];
  float PRY1[3], PRY2[2];
  int i = 0;
  while (i < 10) {
    M5.Mpu6886.getAccelData(&accX, &accY, &accZ);
    M5.Mpu6886.getGyroData(&gyroX, &gyroY, &gyroZ);
    M5.Mpu6886.getAhrsData(&pitch, &roll, &yaw);
    roll2 = atan(accY / accZ) * RAD_TO_DEG;
    pitch2 = atan(-accX / sqrtf(accY * accY + accZ * accZ)) * RAD_TO_DEG;

    acc[0] += (accX - AccOffset[0]);
    acc[1] += (accY - AccOffset[1]);
    acc[2] += (accZ - AccOffset[2]);
    gyro[0] += (gyroX - GyroOffset[0]);
    gyro[1] += (gyroY - GyroOffset[1]);
    gyro[2] += (gyroZ - GyroOffset[2]);
    PRY1[0] += (pitch - PRYOffset[0]);
    PRY1[1] += (roll - PRYOffset[1]);
    PRY1[2] += (yaw - PRYOffset[2]);
    PRY2[0] += (pitch2);
    PRY2[1] += (roll2);
    i++;
    delay(10);
  }
  accX = acc[0] / i;
  accY = acc[1] / i;
  accZ = acc[2] / i;
  gyroX = gyro[0] / i;
  gyroY = gyro[1] / i;
  gyroZ = gyro[2] / i;
  pitch = PRY1[0] / i;
  roll = PRY1[1] / i;
  yaw = PRY1[2] / i;
  pitch2 = PRY2[0] / i;
  roll2 = PRY2[1] / i;
}