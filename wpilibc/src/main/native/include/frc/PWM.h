/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "frc/PWMBase.h"
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
class PWM : public PWMBase, public Sendable, public SendableHelper<PWM> {
 public:
  friend class AddressableLED;
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

 protected:
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
