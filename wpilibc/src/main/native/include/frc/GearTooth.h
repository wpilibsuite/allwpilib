// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <wpi/deprecated.h>

#include "frc/Counter.h"

namespace frc {

/**
 * Alias for counter class.
 *
 * Implements the gear tooth sensor supplied by FIRST. Currently there is no
 * reverse sensing on the gear tooth sensor, but in future versions we might
 * implement the necessary timing in the FPGA to sense direction.
 *
 * @deprecated No longer used per FMS usage reporting
 */
class GearTooth : public Counter {
 public:
  // 55 uSec for threshold
  static constexpr double kGearToothThreshold = 55e-6;

  /**
   * Construct a GearTooth sensor given a channel.
   *
   * @param channel            The DIO channel that the sensor is connected to.
   *                           0-9 are on-board, 10-25 are on the MXP.
   * @param directionSensitive True to enable the pulse length decoding in
   *                           hardware to specify count direction.
   */
  WPI_DEPRECATED(
      "The only sensor this works with is no longer available and no teams use "
      "it according to FMS usage reporting.")
  explicit GearTooth(int channel, bool directionSensitive = false);

  /**
   * Construct a GearTooth sensor given a digital input.
   *
   * This should be used when sharing digital inputs.
   *
   * @param source             A pointer to the existing DigitalSource object
   *                           (such as a DigitalInput)
   * @param directionSensitive True to enable the pulse length decoding in
   *                           hardware to specify count direction.
   */
  WPI_DEPRECATED(
      "The only sensor this works with is no longer available and no teams use "
      "it according to FMS usage reporting.")
  explicit GearTooth(DigitalSource* source, bool directionSensitive = false);

  /**
   * Construct a GearTooth sensor given a digital input.
   *
   * This should be used when sharing digital inputs.
   *
   * @param source             A reference to the existing DigitalSource object
   *                           (such as a DigitalInput)
   * @param directionSensitive True to enable the pulse length decoding in
   *                           hardware to specify count direction.
   */
  WPI_DEPRECATED(
      "The only sensor this works with is no longer available and no teams use "
      "it according to FMS usage reporting.")
  explicit GearTooth(std::shared_ptr<DigitalSource> source,
                     bool directionSensitive = false);

  GearTooth(GearTooth&&) = default;
  GearTooth& operator=(GearTooth&&) = default;

  /**
   * Common code called by the constructors.
   */
  void EnableDirectionSensing(bool directionSensitive);

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
