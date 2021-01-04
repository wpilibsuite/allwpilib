// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/angular_velocity.h"
#include "units/current.h"
#include "units/impedance.h"
#include "units/torque.h"
#include "units/voltage.h"

namespace frc {

/**
 * Holds the constants for a DC motor.
 */
class DCMotor {
 public:
  using radians_per_second_per_volt_t =
      units::unit_t<units::compound_unit<units::radians_per_second,
                                         units::inverse<units::volt>>>;
  using newton_meters_per_ampere_t =
      units::unit_t<units::compound_unit<units::newton_meters,
                                         units::inverse<units::ampere>>>;

  units::volt_t nominalVoltage;
  units::newton_meter_t stallTorque;
  units::ampere_t stallCurrent;
  units::ampere_t freeCurrent;
  units::radians_per_second_t freeSpeed;

  // Resistance of motor
  units::ohm_t R;

  // Motor velocity constant
  radians_per_second_per_volt_t Kv;

  // Torque constant
  newton_meters_per_ampere_t Kt;

  /**
   * Constructs a DC motor.
   *
   * @param nominalVoltage Voltage at which the motor constants were measured.
   * @param stallTorque    Torque when stalled.
   * @param stallCurrent   Current draw when stalled.
   * @param freeCurrent    Current draw under no load.
   * @param freeSpeed      Angular velocity under no load.
   * @param numMotors      Number of motors in a gearbox.
   */
  constexpr DCMotor(units::volt_t nominalVoltage,
                    units::newton_meter_t stallTorque,
                    units::ampere_t stallCurrent, units::ampere_t freeCurrent,
                    units::radians_per_second_t freeSpeed, int numMotors = 1)
      : nominalVoltage(nominalVoltage),
        stallTorque(stallTorque * numMotors),
        stallCurrent(stallCurrent * numMotors),
        freeCurrent(freeCurrent * numMotors),
        freeSpeed(freeSpeed),
        R(nominalVoltage / this->stallCurrent),
        Kv(freeSpeed / (nominalVoltage - R * this->freeCurrent)),
        Kt(this->stallTorque / this->stallCurrent) {}

  /**
   * Returns current drawn by motor with given speed and input voltage.
   *
   * @param speed        The current angular velocity of the motor.
   * @param inputVoltage The voltage being applied to the motor.
   */
  constexpr units::ampere_t Current(units::radians_per_second_t speed,
                                    units::volt_t inputVoltage) const {
    return -1.0 / Kv / R * speed + 1.0 / R * inputVoltage;
  }

  /**
   * Returns instance of CIM.
   */
  static constexpr DCMotor CIM(int numMotors = 1) {
    return DCMotor(12_V, 2.42_Nm, 133_A, 2.7_A, 5310_rpm, numMotors);
  }

  /**
   * Returns instance of MiniCIM.
   */
  static constexpr DCMotor MiniCIM(int numMotors = 1) {
    return DCMotor(12_V, 1.41_Nm, 89_A, 3_A, 5840_rpm, numMotors);
  }

  /**
   * Returns instance of Bag motor.
   */
  static constexpr DCMotor Bag(int numMotors = 1) {
    return DCMotor(12_V, 0.43_Nm, 53_A, 1.8_A, 13180_rpm, numMotors);
  }

  /**
   * Returns instance of Vex 775 Pro.
   */
  static constexpr DCMotor Vex775Pro(int numMotors = 1) {
    return DCMotor(12_V, 0.71_Nm, 134_A, 0.7_A, 18730_rpm, numMotors);
  }

  /**
   * Returns instance of Andymark RS 775-125.
   */
  static constexpr DCMotor RS775_125(int numMotors = 1) {
    return DCMotor(12_V, 0.28_Nm, 18_A, 1.6_A, 5800_rpm, numMotors);
  }

  /**
   * Returns instance of Banebots RS 775.
   */
  static constexpr DCMotor BanebotsRS775(int numMotors = 1) {
    return DCMotor(12_V, 0.72_Nm, 97_A, 2.7_A, 13050_rpm, numMotors);
  }

  /**
   * Returns instance of Andymark 9015.
   */
  static constexpr DCMotor Andymark9015(int numMotors = 1) {
    return DCMotor(12_V, 0.36_Nm, 71_A, 3.7_A, 14270_rpm, numMotors);
  }

  /**
   * Returns instance of Banebots RS 550.
   */
  static constexpr DCMotor BanebotsRS550(int numMotors = 1) {
    return DCMotor(12_V, 0.38_Nm, 84_A, 0.4_A, 19000_rpm, numMotors);
  }

  /**
   * Returns instance of NEO brushless motor.
   */
  static constexpr DCMotor NEO(int numMotors = 1) {
    return DCMotor(12_V, 2.6_Nm, 105_A, 1.8_A, 5676_rpm, numMotors);
  }

  /**
   * Returns instance of NEO 550 brushless motor.
   */
  static constexpr DCMotor NEO550(int numMotors = 1) {
    return DCMotor(12_V, 0.97_Nm, 100_A, 1.4_A, 11000_rpm, numMotors);
  }

  /**
   * Returns instance of Falcon 500 brushless motor.
   */
  static constexpr DCMotor Falcon500(int numMotors = 1) {
    return DCMotor(12_V, 4.69_Nm, 257_A, 1.5_A, 6380_rpm, numMotors);
  }
};

}  // namespace frc
