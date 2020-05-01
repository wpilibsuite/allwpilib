/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Types.h>

#include "frc/ErrorBase.h"
#include "frc/SensorUtil.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

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
class Compressor : public ErrorBase,
                   public Sendable,
                   public SendableHelper<Compressor> {
 public:
  /**
   * Constructor. The default PCM ID is 0.
   *
   * @param module The PCM ID to use (0-62)
   */
  explicit Compressor(int pcmID = SensorUtil::GetDefaultSolenoidModule());

  ~Compressor() override = default;

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

  /**
   * Query if the compressor output has been disabled due to high current draw.
   *
   * @return true if PCM is in fault state : Compressor Drive is
   *         disabled due to compressor current being too high.
   */
  bool GetCompressorCurrentTooHighFault() const;

  /**
   * Query if the compressor output has been disabled due to high current draw
   * (sticky).
   *
   * A sticky fault will not clear on device reboot, it must be cleared through
   * code or the webdash.
   *
   * @return true if PCM sticky fault is set : Compressor Drive is
   *         disabled due to compressor current being too high.
   */
  bool GetCompressorCurrentTooHighStickyFault() const;

  /**
   * Query if the compressor output has been disabled due to a short circuit
   * (sticky).
   *
   * A sticky fault will not clear on device reboot, it must be cleared through
   * code or the webdash.
   *
   * @return true if PCM sticky fault is set : Compressor output
   *         appears to be shorted.
   */
  bool GetCompressorShortedStickyFault() const;

  /**
   * Query if the compressor output has been disabled due to a short circuit.
   *
   * @return true if PCM is in fault state : Compressor output
   *         appears to be shorted.
   */
  bool GetCompressorShortedFault() const;

  /**
   * Query if the compressor output does not appear to be wired (sticky).
   *
   * A sticky fault will not clear on device reboot, it must be cleared through
   * code or the webdash.
   *
   * @return true if PCM sticky fault is set : Compressor does not
   *         appear to be wired, i.e. compressor is not drawing enough current.
   */
  bool GetCompressorNotConnectedStickyFault() const;

  /**
   * Query if the compressor output does not appear to be wired.
   *
   * @return true if PCM is in fault state : Compressor does not
   *         appear to be wired, i.e. compressor is not drawing enough current.
   */
  bool GetCompressorNotConnectedFault() const;

  /**
   * Clear ALL sticky faults inside PCM that Compressor is wired to.
   *
   * If a sticky fault is set, then it will be persistently cleared.  Compressor
   * drive maybe momentarily disable while flags are being cleared. Care should
   * be taken to not call this too frequently, otherwise normal compressor
   * functionality may be prevented.
   *
   * If no sticky faults are set then this call will have no effect.
   */
  void ClearAllPCMStickyFaults();

  void InitSendable(SendableBuilder& builder) override;

 protected:
  hal::Handle<HAL_CompressorHandle> m_compressorHandle;

 private:
  void SetCompressor(bool on);
  int m_module;
};

}  // namespace frc
