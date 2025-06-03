#include <Servo.h>
#include <SimpleKalmanFilter.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Khởi tạo bộ lọc Kalman (Q, R, P)
SimpleKalmanFilter bo_loc(2, 2, 0.001);

// Khai báo các chân kết nối
const int mq2Pin = A0;
const int buzzerPin = 9;
const int servo1Pin = 10;
const int servo2Pin = 11;
const int relayPin = 12;

// Khởi tạo LCD I2C 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ngưỡng phát hiện khói
int smokeThreshold = 400;

// Khởi tạo đối tượng servo
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(9600);

  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("He thong");
  lcd.setCursor(0, 1);
  lcd.print("phat hien khi gas");

  // Thiết lập chân
  pinMode(mq2Pin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(180);
  servo2.write(90);

  delay(2000); // Chờ hệ thống ổn định
  lcd.clear();
}

void loop() {
  int rawSmoke = analogRead(mq2Pin);
  float smoke = bo_loc.updateEstimate(rawSmoke);

  // Hiển thị giá trị lên Serial
  Serial.print("Raw: ");
  Serial.print(rawSmoke);
  Serial.print(" | Filtered: ");
  Serial.println(smoke);

  // Hiển thị lên LCD
  lcd.setCursor(0, 0);
  lcd.print("Khi gas: ");
  lcd.print((int)smoke);
  lcd.print("     "); // Xóa dư ký tự

  // Nếu giá trị vượt ngưỡng
  if (smoke > smokeThreshold) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(relayPin, HIGH);
    servo1.write(90);
    servo2.write(180);
    lcd.setCursor(0, 1);
    lcd.print("CANH BAO! NGUY HIEM");
  } else {
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, LOW);
    servo1.write(180);
    servo2.write(90);
    lcd.setCursor(0, 1);
    lcd.print("Moi truong an toan ");
  }

  delay(500);
}
