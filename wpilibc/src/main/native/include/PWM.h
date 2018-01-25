/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <HAL/Types.h>

#include "ErrorBase.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

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
 *   - 1 = minimum pulse width (currently .5ms)
 *   - 0 = disabled (i.e. PWM output is held low)
 */
class PWM : public ErrorBase, public SendableBase {
 public:
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

  explicit PWM(int channel);
  ~PWM() override;
  virtual void SetRaw(uint16_t value);
  virtual uint16_t GetRaw() const;
  virtual void SetPosition(double pos);
  virtual double GetPosition() const;
  virtual void SetSpeed(double speed);
  virtual double GetSpeed() const;
  virtual void SetDisabled();
  void SetPeriodMultiplier(PeriodMultiplier mult);
  void SetZeroLatch();
  void EnableDeadbandElimination(bool eliminateDeadband);
  void SetBounds(double max, double deadbandMax, double center,
                 double deadbandMin, double min);
  void SetRawBounds(int max, int deadbandMax, int center, int deadbandMin,
                    int min);
  void GetRawBounds(int32_t* max, int32_t* deadbandMax, int32_t* center,
                    int32_t* deadbandMin, int32_t* min);
  int GetChannel() const { return m_channel; }

 protected:
  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  HAL_DigitalHandle m_handle;
};

}  // namespace frc
