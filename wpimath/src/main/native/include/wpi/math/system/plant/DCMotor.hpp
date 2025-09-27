// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "units/angular_velocity.h"
#include "units/current.h"
#include "units/impedance.h"
#include "units/torque.h"
#include "units/voltage.h"

namespace frc {

/**
 * Holds the constants for a DC motor.
 */
class WPILIB_DLLEXPORT DCMotor {
 public:
  using radians_per_second_per_volt_t =
      units::unit_t<units::compound_unit<units::radians_per_second,
                                         units::inverse<units::volt>>>;
  using newton_meters_per_ampere_t =
      units::unit_t<units::compound_unit<units::newton_meters,
                                         units::inverse<units::ampere>>>;

  /// Voltage at which the motor constants were measured.
  units::volt_t nominalVoltage;

  /// Torque when stalled.
  units::newton_meter_t stallTorque;

  /// Current draw when stalled.
  units::ampere_t stallCurrent;

  /// Current draw under no load.
  units::ampere_t freeCurrent;

  /// Angular velocity under no load.
  units::radians_per_second_t freeSpeed;

  /// Motor internal resistance.
  units::ohm_t R;

  /// Motor velocity constant.
  radians_per_second_per_volt_t Kv;

  /// Motor torque constant.
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
   * Returns current drawn by motor for a given torque.
   *
   * @param torque The torque produced by the motor.
   */
  constexpr units::ampere_t Current(units::newton_meter_t torque) const {
    return torque / Kt;
  }

  /**
   * Returns torque produced by the motor with a given current.
   *
   * @param current     The current drawn by the motor.
   */
  constexpr units::newton_meter_t Torque(units::ampere_t current) const {
    return current * Kt;
  }

  /**
   * Returns the voltage provided to the motor for a given torque and
   * angular velocity.
   *
   * @param torque      The torque produced by the motor.
   * @param speed       The current angular velocity of the motor.
   */
  constexpr units::volt_t Voltage(units::newton_meter_t torque,
                                  units::radians_per_second_t speed) const {
    return 1.0 / Kv * speed + 1.0 / Kt * R * torque;
  }

  /**
   * Returns the angular speed produced by the motor at a given torque and input
   * voltage.
   *
   * @param torque        The torque produced by the motor.
   * @param inputVoltage  The input voltage provided to the motor.
   */
  constexpr units::radians_per_second_t Speed(
      units::newton_meter_t torque, units::volt_t inputVoltage) const {
    return inputVoltage * Kv - 1.0 / Kt * torque * R * Kv;
  }

  /**
   * Returns a copy of this motor with the given gearbox reduction applied.
   *
   * @param gearboxReduction  The gearbox reduction.
   */
  constexpr DCMotor WithReduction(double gearboxReduction) {
    return DCMotor(nominalVoltage, stallTorque * gearboxReduction, stallCurrent,
                   freeCurrent, freeSpeed / gearboxReduction);
  }

  /**
   * Returns a gearbox of CIM motors.
   */
  static constexpr DCMotor CIM(int numMotors = 1) {
    return DCMotor(12_V, 2.42_Nm, 133_A, 2.7_A, 5310_rpm, numMotors);
  }

  /**
   * Returns a gearbox of MiniCIM motors.
   */
  static constexpr DCMotor MiniCIM(int numMotors = 1) {
    return DCMotor(12_V, 1.41_Nm, 89_A, 3_A, 5840_rpm, numMotors);
  }

  /**
   * Returns a gearbox of Bag motor motors.
   */
  static constexpr DCMotor Bag(int numMotors = 1) {
    return DCMotor(12_V, 0.43_Nm, 53_A, 1.8_A, 13180_rpm, numMotors);
  }

  /**
   * Returns a gearbox of Vex 775 Pro motors.
   */
  static constexpr DCMotor Vex775Pro(int numMotors = 1) {
    return DCMotor(12_V, 0.71_Nm, 134_A, 0.7_A, 18730_rpm, numMotors);
  }

  /**
   * Returns a gearbox of Andymark RS 775-125 motors.
   */
  static constexpr DCMotor RS775_125(int numMotors = 1) {
    return DCMotor(12_V, 0.28_Nm, 18_A, 1.6_A, 5800_rpm, numMotors);
  }

  /**
   * Returns a gearbox of Banebots RS 775 motors.
   */
  static constexpr DCMotor BanebotsRS775(int numMotors = 1) {
    return DCMotor(12_V, 0.72_Nm, 97_A, 2.7_A, 13050_rpm, numMotors);
  }

  /**
   * Returns a gearbox of Andymark 9015 motors.
   */
  static constexpr DCMotor Andymark9015(int numMotors = 1) {
    return DCMotor(12_V, 0.36_Nm, 71_A, 3.7_A, 14270_rpm, numMotors);
  }

  /**
   * Returns a gearbox of Banebots RS 550 motors.
   */
  static constexpr DCMotor BanebotsRS550(int numMotors = 1) {
    return DCMotor(12_V, 0.38_Nm, 84_A, 0.4_A, 19000_rpm, numMotors);
  }

  /**
   * Returns a gearbox of NEO brushless motors.
   */
  static constexpr DCMotor NEO(int numMotors = 1) {
    return DCMotor(12_V, 2.6_Nm, 105_A, 1.8_A, 5676_rpm, numMotors);
  }

  /**
   * Returns a gearbox of NEO 550 brushless motors.
   */
  static constexpr DCMotor NEO550(int numMotors = 1) {
    return DCMotor(12_V, 0.97_Nm, 100_A, 1.4_A, 11000_rpm, numMotors);
  }

  /**
   * Returns a gearbox of Falcon 500 brushless motors.
   */
  static constexpr DCMotor Falcon500(int numMotors = 1) {
    return DCMotor(12_V, 4.69_Nm, 257_A, 1.5_A, 6380_rpm, numMotors);
  }

  /**
   * Return a gearbox of Falcon 500 motors with FOC (Field-Oriented Control)
   * enabled.
   */
  static constexpr DCMotor Falcon500FOC(int numMotors = 1) {
    // https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/
    return DCMotor(12_V, 5.84_Nm, 304_A, 1.5_A, 6080_rpm, numMotors);
  }

  /**
   * Return a gearbox of Romi/TI_RSLK MAX motors.
   */
  static constexpr DCMotor RomiBuiltIn(int numMotors = 1) {
    // From https://www.pololu.com/product/1520/specs
    return DCMotor(4.5_V, 0.1765_Nm, 1.25_A, 0.13_A, 150_rpm, numMotors);
  }

  /**
   * Return a gearbox of Kraken X60 brushless motors.
   */
  static constexpr DCMotor KrakenX60(int numMotors = 1) {
    // From https://store.ctr-electronics.com/announcing-kraken-x60/
    return DCMotor(12_V, 7.09_Nm, 366_A, 2_A, 6000_rpm, numMotors);
  }

  /**
   * Return a gearbox of Kraken X60 brushless motors with FOC (Field-Oriented
   * Control) enabled.
   */
  static constexpr DCMotor KrakenX60FOC(int numMotors = 1) {
    // From https://store.ctr-electronics.com/announcing-kraken-x60/
    return DCMotor(12_V, 9.37_Nm, 483_A, 2_A, 5800_rpm, numMotors);
  }

  /**
   * Return a gearbox of Neo Vortex brushless motors.
   */
  static constexpr DCMotor NeoVortex(int numMotors = 1) {
    // From https://www.revrobotics.com/next-generation-spark-neo/
    return DCMotor(12_V, 3.60_Nm, 211_A, 3.615_A, 6784_rpm, numMotors);
  }
};

}  // namespace frc

#include "frc/system/plant/proto/DCMotorProto.h"
#include "frc/system/plant/struct/DCMotorStruct.h"
