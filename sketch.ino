#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
// Initialize MPU6050 and 16x2 I2C LCD (address 0x27)
Adafruit_MPU6050 mpu;
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Pin definitions
const int GREEN_LED = 7;
const int RED_LED   = 8;
const int BUZZER    = 9;
// Baseline acceleration values
float baseX = 0.0, baseY = 0.0, baseZ = 0.0;
// Earthquake trigger threshold in m/s^2 (adjust for sensitivity)
const float THRESHOLD = 2.5;
void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Earthquake Alert");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  // Initialize MPU6050
  if (!mpu.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MPU6050 Error!");
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(500);
  // Record baseline resting position
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  baseX = a.acceleration.x;
  baseY = a.acceleration.y;
  baseZ = a.acceleration.z;
  lcd.clear();
}
void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // Calculate acceleration change from baseline along X, Y, Z
  float dx = a.acceleration.x - baseX;
  float dy = a.acceleration.y - baseY;
  float dz = a.acceleration.z - baseZ;
  // Net vibration magnitude (m/s^2)
  float vibration = sqrt((dx * dx) + (dy * dy) + (dz * dz));
  // Print values to Serial Monitor for debugging
  Serial.print("Vibration Magnitude: ");
  Serial.print(vibration);
  Serial.println(" m/s^2");
  // Check if vibration exceeds threshold
  if (vibration >= THRESHOLD) {
    // ALERT STATE
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 1000); // 1 kHz alarm tone
    lcd.setCursor(0, 0);
    lcd.print("!! EARTHQUAKE !!");
    lcd.setCursor(0, 1);
    lcd.print("Vib: ");
    lcd.print(vibration, 1);
    lcd.print(" m/s2   ");
    delay(250);
    // Flash Red LED and alternate buzzer pitch for urgency
    digitalWrite(RED_LED, LOW);
    tone(BUZZER, 1500);
    delay(250);
  } else {
    // SAFE STATE
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    lcd.setCursor(0, 0);
    lcd.print("Status: SAFE    ");
    lcd.setCursor(0, 1);
    lcd.print("Vib: ");
    lcd.print(vibration, 1);
    lcd.print(" m/s2   ");
    delay(200);
  }
}