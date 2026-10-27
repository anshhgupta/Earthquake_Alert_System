#Arduino-Based Earthquake Alert System

## Overview
##Objective: 
To design and simulate a real-time seismic vibration detection system capable of sensing ground movement and triggering immediate visual and audible alerts.
##Core Concept:
The system uses a 3-axis MEMS accelerometer (MPU6050) to continuously monitor structural or ground vibrations along the X, Y, and Z axes.
##Key Features:
1)  Automatic baseline calibration at startup to account for sensor tilt and gravity.
2)  Shared I2C communication bus (A4 and A5) for both the sensor and LCD screen, reducing wiring complexity.
3)  Multi-modal alerting mechanism combining an LCD status screen, status LEDs, and a dual-tone piezo alarm.
4)  Simulated and verified in a virtual prototyping environment (Wokwi).

## Components Used
1)  Arduino Uno (1×): Acts as the central microcontroller that reads sensor data, calculates net vibration magnitude, and triggers the output peripherals.
2)  MPU6050 Sensor (1×): A 6-axis MEMS accelerometer and gyroscope module that detects real-time seismic vibrations along the X, Y, and Z axes.
3)  16×2 I2C LCD Display (1×): Shows the live system status (SAFE or !! EARTHQUAKE !!) along with the real-time vibration reading in $\text{m/s}^2$ using only two communication pins (A4 and A5).
4)  Piezo Buzzer (1×): Produces an alternating high-frequency audio alarm (1 kHz to 1.5 kHz) when ground movement crosses the safety threshold.
5)  Green & Red LEDs (1× each): Provide instant visual feedback—the Green LED remains lit under normal conditions, while the Red LED flashes during an earthquake alert.
6)  220 Ω Resistors (2×): Limit the electrical current flowing through the Red and Green LEDs to prevent damage.
7)  Connecting Wires: Link the common power rails (5V and GND), shared I2C data/clock lines (SDA/SCL), and digital control pins (Pins 7, 8, and 9).

## Working
1) Initialization & Calibration:
a) When powered on, the Arduino initializes the MPU6050 and the 16x2 LCD via the I2C bus.
b) It captures the resting acceleration values ($x_0, y_0, z_0$) to establish a stable baseline and eliminate static gravitational offset.
2) Real-Time Vibration Sensing:
a) The MPU6050 continuously measures instantaneous acceleration ($a_x, a_y, a_z$) and sends data to the Arduino via pins A4 (SDA) and A5 (SCL).
3) Magnitude Computation:
a) The microcontroller calculates the net 3D vibration vector ($\Delta a$) relative to the baseline:
 $$\Delta a = \sqrt{(a_x - x_0)^2 + (a_y - y_0)^2 + (a_z - z_0)^2}$$
4) Threshold Comparison & Alert Triggering:
a) Normal State ($\Delta a \lt 2.5\text{ m/s}^2$): The Green LED stays ON, the Red LED and Buzzer remain OFF, and the LCD shows Status: SAFE alongside live vibration readings.
b) Alert State ($\Delta a \ge 2.5\text{ m/s}^2$): The Green LED turns OFF, the Red LED flashes rapidly, the Buzzer emits an alternating alarm tone, and the LCD displays !! EARTHQUAKE !!.

## Source Code
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

Adafruit_MPU6050 mpu;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int GREEN_LED = 7;
const int RED_LED   = 8;
const int BUZZER    = 9;

float baseX = 0.0, baseY = 0.0, baseZ = 0.0;
const float THRESHOLD = 2.5; // Sensitivity threshold in m/s^2

void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Earthquake Alert");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  if (!mpu.begin()) {
    lcd.clear();
    lcd.print("MPU6050 Error!");
    while (1) { delay(10); }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(500);

  // Calibrate resting baseline
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

  float dx = a.acceleration.x - baseX;
  float dy = a.acceleration.y - baseY;
  float dz = a.acceleration.z - baseZ;
  float vibration = sqrt((dx * dx) + (dy * dy) + (dz * dz));

  if (vibration >= THRESHOLD) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 1000);

    lcd.setCursor(0, 0);
    lcd.print("!! EARTHQUAKE !!");
    lcd.setCursor(0, 1);
    lcd.print("Vib: ");
    lcd.print(vibration, 1);
    lcd.print(" m/s2   ");
    delay(250);

    digitalWrite(RED_LED, LOW);
    tone(BUZZER, 1500);
    delay(250);
  } else {
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

## Wokwi Simulation
https://wokwi.com/projects/478154039215232001
