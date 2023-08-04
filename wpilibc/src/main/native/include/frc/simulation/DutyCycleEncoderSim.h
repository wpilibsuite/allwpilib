// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/angle.h>

namespace frc {

class DutyCycleEncoder;

namespace sim {

/**
 * Class to control a simulated duty cycle encoder.
 */
class DutyCycleEncoderSim {
 public:
  /**
   * Constructs from a DutyCycleEncoder object.
   *
   * @param encoder DutyCycleEncoder to simulate
   */
  explicit DutyCycleEncoderSim(const DutyCycleEncoder& encoder);

  /**
   * Constructs from a digital input channel.
   *
   * @param channel digital input channel
   */
  explicit DutyCycleEncoderSim(int channel);

  /**
   * Get the position in turns.
   *
   * @return The position.
   */
  double Get();

  /**
   * Set the position in turns.
   *
   * @param turns The position.
   */
  void Set(units::turn_t turns);

  /**
   * Get the distance.
   *
   * @return The distance.
   */

  double GetDistance();

  /**
   * Set the distance.
   *
   * @param distance The distance.
   */
  void SetDistance(double distance);

  /**
   * Get the absolute position.
   *
   * @return The absolute position
   */
  double GetAbsolutePosition();

  /**
   * Set the absolute position.
   *
   * @param position The absolute position
   */
  void SetAbsolutePosition(double position);

  /**
   * Get the distance per rotation for this encoder.
   *
   * @return The scale factor that will be used to convert rotation to useful
   * units.
   */
  double GetDistancePerRotation();

  /**
   * Get if the encoder is connected.
   *
   * @return true if the encoder is connected.
   */
  bool IsConnected();

  /**
   * Set if the encoder is connected.
   *
   * @param isConnected Whether or not the sensor is connected.
   */
  void SetConnected(bool isConnected);

 private:
  hal::SimDouble m_simPosition;
  hal::SimDouble m_simDistancePerRotation;
  hal::SimDouble m_simAbsolutePosition;
  hal::SimBoolean m_simIsConnected;
};

}  // namespace sim
}  // namespace frc
