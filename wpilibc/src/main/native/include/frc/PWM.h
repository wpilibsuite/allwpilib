// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <hal/Types.h>
#include <units/time.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {
class AddressableLED;
class DMA;

/**
 * Class implements the PWM generation in the FPGA.
 *
 * The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They
 * are mapped to the microseconds to keep the pulse high, with a range of 0
 * (off) to 4096. Changes are immediately sent to the FPGA, and the update
 * occurs at the next FPGA cycle (5.05ms). There is no delay.
 */
class PWM : public wpi::Sendable, public wpi::SendableHelper<PWM> {
 public:
  friend class AddressableLED;
  friend class DMA;
  /**
   * Represents the amount to multiply the minimum servo-pulse pwm period by.
   */
  enum PeriodMultiplier {
    /**
     * Don't skip pulses. PWM pulses occur every 5.05 ms
     */
    kPeriodMultiplier_1X = 1,
    /**
     * Skip every other pulse. PWM pulses occur every 10.10 ms
     */
    kPeriodMultiplier_2X = 2,
    /**
     * Skip three out of four pulses. PWM pulses occur every 20.20 ms
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
   * @param registerSendable If true, adds this instance to SendableRegistry
   *                         and LiveWindow
   */
  explicit PWM(int channel, bool registerSendable = true);

  /**
   * Free the PWM channel.
   *
   * Free the resource associated with the PWM channel and set the value to 0.
   */
  ~PWM() override;

  PWM(PWM&&) = default;
  PWM& operator=(PWM&&) = default;

  /**
   * Set the PWM pulse time directly to the hardware.
   *
   * Write a microsecond value to a PWM channel.
   *
   * @param time Microsecond PWM value.
   */
  virtual void SetPulseTime(units::microsecond_t time);

  /**
   * Get the PWM pulse time directly from the hardware.
   *
   * Read a microsecond value from a PWM channel.
   *
   * @return Microsecond PWM control value.
   */
  virtual units::microsecond_t GetPulseTime() const;

  /**
   * Set the PWM value based on a position.
   *
   * This is intended to be used by servos.
   *
   * @pre SetBounds() called.
   *
   * @param pos The position to set the servo between 0.0 and 1.0.
   */
  virtual void SetPosition(double pos);

  /**
   * Get the PWM value in terms of a position.
   *
   * This is intended to be used by servos.
   *
   * @pre SetBounds() called.
   *
   * @return The position the servo is set to between 0.0 and 1.0.
   */
  virtual double GetPosition() const;

  /**
   * Set the PWM value based on a speed.
   *
   * This is intended to be used by motor controllers.
   *
   * @pre SetBounds() called.
   *
   * @param speed The speed to set the motor controller between -1.0 and 1.0.
   */
  virtual void SetSpeed(double speed);

  /**
   * Get the PWM value in terms of speed.
   *
   * This is intended to be used by motor controllers.
   *
   * @pre SetBounds() called.
   *
   * @return The most recently set speed between -1.0 and 1.0.
   */
  virtual double GetSpeed() const;

  /**
   * Temporarily disables the PWM output. The next set call will re-enable
   * the output.
   */
  virtual void SetDisabled();

  /**
   * Slow down the PWM signal for old devices.
   *
   * @param mult The period multiplier to apply to this channel
   */
  void SetPeriodMultiplier(PeriodMultiplier mult);

  /**
   * Latches PWM to zero.
   */
  void SetZeroLatch();

  /**
   * Optionally eliminate the deadband from a motor controller.
   *
   * @param eliminateDeadband If true, set the motor curve on the motor
   *                          controller to eliminate the deadband in the middle
   *                          of the range. Otherwise, keep the full range
   *                          without modifying any values.
   */
  void EnableDeadbandElimination(bool eliminateDeadband);

  /**
   * Set the bounds on the PWM pulse widths.
   *
   * This sets the bounds on the PWM values for a particular type of controller.
   * The values determine the upper and lower speeds as well as the deadband
   * bracket.
   *
   * @param max         The max PWM pulse width in us
   * @param deadbandMax The high end of the deadband range pulse width in us
   * @param center      The center (off) pulse width in us
   * @param deadbandMin The low end of the deadband pulse width in us
   * @param min         The minimum pulse width in us
   */
  void SetBounds(units::microsecond_t max, units::microsecond_t deadbandMax,
                 units::microsecond_t center, units::microsecond_t deadbandMin,
                 units::microsecond_t min);

  /**
   * Get the bounds on the PWM values.
   *
   * This gets the bounds on the PWM values for a particular each type of
   * controller. The values determine the upper and lower speeds as well as the
   * deadband bracket.
   *
   * @param max         The maximum pwm value
   * @param deadbandMax The high end of the deadband range
   * @param center      The center speed (off)
   * @param deadbandMin The low end of the deadband range
   * @param min         The minimum pwm value
   */
  void GetBounds(units::microsecond_t* max, units::microsecond_t* deadbandMax,
                 units::microsecond_t* center,
                 units::microsecond_t* deadbandMin, units::microsecond_t* min);

  /**
   * Sets the PWM output to be a continuous high signal while enabled.
   *
   */
  void SetAlwaysHighMode();

  int GetChannel() const;

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  int m_channel;
  hal::Handle<HAL_DigitalHandle> m_handle;
};

}  // namespace frc
