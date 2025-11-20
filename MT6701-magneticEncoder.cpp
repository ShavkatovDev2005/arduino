#include <Wire.h>
#include <SimpleFOC.h>
#include "MT6701.hpp"

// --------- MT6701 ---------
MT6701 mt6701(MT6701::DEFAULT_ADDRESS, 2); // 2 ms update interval

// --------- Motor and Driver ---------
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(11, 10, 9, 8);

// --------- Commander ---------
Commander command = Commander(Serial);
void onTarget(char* cmd){ command.motion(&motor, cmd); }

void setup() {
  Serial.begin(115200);
  delay(500);

  // start I2C for MT6701
  Wire.begin(21, 22); // ESP32 SDA=21, SCL=22
  mt6701.begin();

  // --------- Motor Driver ---------
  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkDriver(&driver);

  // --------- Motor Parameters ---------
  motor.controller = MotionControlType::angle;
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  motor.voltage_limit = 6;
  motor.LPF_velocity.Tf = 0.02;
  motor.P_angle.P = 20;
  motor.velocity_limit = 4;

  motor.init();
  motor.initFOC();

  // --------- Commander ---------
  command.add('T', onTarget, "motion control");

  Serial.println("Motor ready with MT6701 encoder!");
}

void loop() {
  // update MT6701 angle
  float angle = mt6701.getAngleDegrees();
  
  // set motor target angle
  motor.target = angle;

  // run FOC
  motor.loopFOC();
  motor.move();

  // run commander
  command.run();

  delay(5); // small delay
}