/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <hal/Types.h>
#include <wpi/raw_ostream.h>

#include "frc/MotorSafety.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {
class AddressableLED;
class SendableBuilder;

/**
 * Class implements the PWM generation in the FPGA.
 *
 * The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They
 * are mapped to the hardware dependent values, in this case 0-2000 for the
 * FPGA. Changes are immediately sent to the FPGA, and the update occurs at the
 * next FPGA cycle (5.005ms). There is no delay.
 *
 * As of revision 0.1.10 of the FPGA, the FPGA interprets the 0-2000 values as
 * follows:
 *   - 2000 = maximum pulse width
 *   - 1999 to 1001 = linear scaling from "full forward" to "center"
 *   - 1000 = center value
 *   - 999 to 2 = linear scaling from "center" to "full reverse"
 *   - 1 = minimum pulse width (currently 0.5ms)
 *   - 0 = disabled (i.e. PWM output is held low)
 */
class PWM : public MotorSafety, public Sendable, public SendableHelper<PWM> {
 public:
  friend class AddressableLED;
  /**
   * Represents the amount to multiply the minimum servo-pulse pwm period by.
   */
  enum PeriodMultiplier {
    /**
     * Don't skip pulses. PWM pulses occur every 5.005 ms
     */
    kPeriodMultiplier_1X = 1,
    /**
     * Skip every other pulse. PWM pulses occur every 10.010 ms
     */
    kPeriodMultiplier_2X = 2,
    /**
     * Skip three out of four pulses. PWM pulses occur every 20.020 ms
     */
    kPeriodMultiplier_4X = 4
  };

  /**
   * Allocate a PWM given a channel number.
   *
   * Checks channel value range and allocates the appropriate channel.
   * The allocation is only done to help users ensure that they don't double
   * assign channels.
   *
   * @param channel The PWM channel number. 0-9 are on-board, 10-19 are on the
   *                MXP port
   */
  explicit PWM(int channel);

  /**
   * Free the PWM channel.
   *
   * Free the resource associated with the PWM channel and set the value to 0.
   */
  ~PWM() override;

  PWM(PWM&&) = default;
  PWM& operator=(PWM&&) = default;

  // MotorSafety interface
  void StopMotor() override;
  void GetDescription(wpi::raw_ostream& desc) const override;

  /**
   * Set the PWM value directly to the hardware.
   *
   * Write a raw value to a PWM channel.
   *
   * @param value Raw PWM value.
   */
  virtual void SetRaw(uint16_t value);

  /**
   * Get the PWM value directly from the hardware.
   *
   * Read a raw value from a PWM channel.
   *
   * @return Raw PWM control value.
   */
  virtual uint16_t GetRaw() const;

  /**
   * Set the PWM value based on a position.
   *
   * This is intended to be used by servos.
   *
   * @pre SetMaxPositivePwm() called.
   * @pre SetMinNegativePwm() called.
   *
   * @param pos The position to set the servo between 0.0 and 1.0.
   */
  virtual void SetPosition(double pos);

  /**
   * Get the PWM value in terms of a position.
   *
   * This is intended to be used by servos.
   *
   * @pre SetMaxPositivePwm() called.
   * @pre SetMinNegativePwm() called.
   *
   * @return The position the servo is set to between 0.0 and 1.0.
   */
  virtual double GetPosition() const;

  /**
   * Set the PWM value based on a speed.
   *
   * This is intended to be used by speed controllers.
   *
   * @pre SetMaxPositivePwm() called.
   * @pre SetMinPositivePwm() called.
   * @pre SetCenterPwm() called.
   * @pre SetMaxNegativePwm() called.
   * @pre SetMinNegativePwm() called.
   *
   * @param speed The speed to set the speed controller between -1.0 and 1.0.
   */
  virtual void SetSpeed(double speed);

  /**
   * Get the PWM value in terms of speed.
   *
   * This is intended to be used by speed controllers.
   *
   * @pre SetMaxPositivePwm() called.
   * @pre SetMinPositivePwm() called.
   * @pre SetMaxNegativePwm() called.
   * @pre SetMinNegativePwm() called.
   *
   * @return The most recently set speed between -1.0 and 1.0.
   */
  virtual double GetSpeed() const;

  /**
   * Temporarily disables the PWM output. The next set call will reenable
   * the output.
   */
  virtual void SetDisabled();

  /**
   * Slow down the PWM signal for old devices.
   *
   * @param mult The period multiplier to apply to this channel
   */
  void SetPeriodMultiplier(PeriodMultiplier mult);

  void SetZeroLatch();

  /**
   * Optionally eliminate the deadband from a speed controller.
   *
   * @param eliminateDeadband If true, set the motor curve on the Jaguar to
   *                          eliminate the deadband in the middle of the range.
   *                          Otherwise, keep the full range without modifying
   *                          any values.
   */
  void EnableDeadbandElimination(bool eliminateDeadband);

  /**
   * Set the bounds on the PWM pulse widths.
   *
   * This sets the bounds on the PWM values for a particular type of controller.
   * The values determine the upper and lower speeds as well as the deadband
   * bracket.
   *
   * @param max         The max PWM pulse width in ms
   * @param deadbandMax The high end of the deadband range pulse width in ms
   * @param center      The center (off) pulse width in ms
   * @param deadbandMin The low end of the deadband pulse width in ms
   * @param min         The minimum pulse width in ms
   */
  void SetBounds(double max, double deadbandMax, double center,
                 double deadbandMin, double min);

  /**
   * Set the bounds on the PWM values.
   *
   * This sets the bounds on the PWM values for a particular each type of
   * controller. The values determine the upper and lower speeds as well as the
   * deadband bracket.
   *
   * @param max         The Minimum pwm value
   * @param deadbandMax The high end of the deadband range
   * @param center      The center speed (off)
   * @param deadbandMin The low end of the deadband range
   * @param min         The minimum pwm value
   */
  void SetRawBounds(int max, int deadbandMax, int center, int deadbandMin,
                    int min);

  /**
   * Get the bounds on the PWM values.
   *
   * This Gets the bounds on the PWM values for a particular each type of
   * controller. The values determine the upper and lower speeds as well as the
   * deadband bracket.
   *
   * @param max         The Minimum pwm value
   * @param deadbandMax The high end of the deadband range
   * @param center      The center speed (off)
   * @param deadbandMin The low end of the deadband range
   * @param min         The minimum pwm value
   */
  void GetRawBounds(int32_t* max, int32_t* deadbandMax, int32_t* center,
                    int32_t* deadbandMin, int32_t* min);

  int GetChannel() const;

 protected:
  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  hal::Handle<HAL_DigitalHandle> m_handle;
};

}  // namespace frc
