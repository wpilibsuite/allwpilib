/*
* Copyright (C) Cross The Road Electronics.  All rights reserved.
* License information can be found in CTRE_LICENSE.txt
* For support and suggestions contact support@ctr-electronics.com or file
* an issue tracker at https://github.com/CrossTheRoadElec/Phoenix-Releases
*/

#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <frc/DriverStation.h>
#include <frc/Timer.h>
#include <frc/PWM.h>
#include <frc/motorcontrol/PWMMotorController.h>
#include <hal/HAL.h>

namespace frc {

/**
 * Cross the Road Electronics (CTRE) Talon FX Motor Controller with PWM control.
 *
 * Note that the Talon FX uses the following bounds for PWM values. These
 * values should work reasonably well for most controllers, but if users
 * experience issues such as asymmetric behavior around the deadband or
 * inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Talon FX User
 * Manual available from Cross the Road Electronics (CTRE).
 *
 * \li 2.004ms = full "forward"
 * \li 1.520ms = the "high end" of the deadband range
 * \li 1.500ms = center of the deadband range (off)
 * \li 1.480ms = the "low end" of the deadband range
 * \li 0.997ms = full "reverse"
 */
class PWMTalonFXS : public PWMMotorController {
 public:
  /**
   * Supported motor arrangements.
   */
  enum class MotorArrangement {
    /**
     * CTR Electronics Minion motor connected over JST
     */
    Minion_JST,
    /**
     * Third-party NEO motor connected over JST
     */
    NEO_JST,
    /**
     * Third-party NEO 550 motor connected over JST
     */
    NEO550_JST,
    /**
     * Third-party NEO Vortex motor connected over JST
     */
    VORTEX_JST,
    /**
     * Brushed DC motor connected to terminals A and B
     */
    Brushed_DC
  };

  void Set(double value) override;

  /**
   * Sets the mode of operation when output is neutral or disabled.
   *
   * @param bIsBrake True for brake, false for coast.
   * @return true if request has been buffered successfully.
   */
  bool SetNeutralMode(bool bIsBrake);

  /**
   * Sets the motor arrangement, including which motor to drive.
   *
   * @param motorArrangement The desired motor arrangement.
   * @return true if request has been buffered successfully.
   */
  bool SetMotorArrangement(MotorArrangement motorArrangement);

  /**
   * Constructor for a Talon FXS connected via PWM.
   *
   * @param channel The PWM channel that the Talon FX is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit PWMTalonFXS(int channel);

  PWMTalonFXS(PWMTalonFXS&&) = default;
  PWMTalonFXS& operator=(PWMTalonFXS&&) = default;
 private:
  Timer _timer;
  std::vector<units::microsecond_t> _configs;

  bool IsTmrExpired();
};

}  // namespace frc
