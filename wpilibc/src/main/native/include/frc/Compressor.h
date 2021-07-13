// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/PneumaticsBase.h"
#include "frc/PneumaticsModuleType.h"
#include "frc/SensorUtil.h"

namespace frc {

/**
 * Class for operating a compressor connected to a %PCM (Pneumatic Control
 * Module).
 *
 * The PCM will automatically run in closed loop mode by default whenever a
 * Solenoid object is created. For most cases, a Compressor object does not need
 * to be instantiated or used in a robot program. This class is only required in
 * cases where the robot program needs a more detailed status of the compressor
 * or to enable/disable closed loop control.
 *
 * Note: you cannot operate the compressor directly from this class as doing so
 * would circumvent the safety provided by using the pressure switch and closed
 * loop control. You can only turn off closed loop control, thereby stopping
 * the compressor from operating.
 */
class Compressor : public wpi::Sendable,
                   public wpi::SendableHelper<Compressor> {
 public:
  /**
   * Constructor. The default PCM ID is 0.
   *
   * @param module The PCM ID to use (0-62)
   */
  Compressor(int module, PneumaticsModuleType moduleType);

  explicit Compressor(PneumaticsModuleType moduleType);

  ~Compressor() override;

  Compressor(const Compressor&) = delete;
  Compressor& operator=(const Compressor&) = delete;

  Compressor(Compressor&&) = default;
  Compressor& operator=(Compressor&&) = default;

  /**
   * Starts closed-loop control. Note that closed loop control is enabled by
   * default.
   */
  void Start();

  /**
   * Stops closed-loop control. Note that closed loop control is enabled by
   * default.
   */
  void Stop();

  /**
   * Check if compressor output is active.
   *
   * @return true if the compressor is on
   */
  bool Enabled() const;

  /**
   * Check if the pressure switch is triggered.
   *
   * @return true if pressure is low
   */
  bool GetPressureSwitchValue() const;

  /**
   * Query how much current the compressor is drawing.
   *
   * @return The current through the compressor, in amps
   */
  double GetCompressorCurrent() const;

  /**
   * Enables or disables automatically turning the compressor on when the
   * pressure is low.
   *
   * @param on Set to true to enable closed loop control of the compressor.
   *           False to disable.
   */
  void SetClosedLoopControl(bool on);

  /**
   * Returns true if the compressor will automatically turn on when the
   * pressure is low.
   *
   * @return True if closed loop control of the compressor is enabled. False if
   *         disabled.
   */
  bool GetClosedLoopControl() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<PneumaticsBase> m_module;
};

}  // namespace frc
