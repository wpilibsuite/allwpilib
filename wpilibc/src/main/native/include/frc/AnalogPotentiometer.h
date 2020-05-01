/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "frc/AnalogInput.h"
#include "frc/ErrorBase.h"
#include "frc/interfaces/Potentiometer.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

/**
 * Class for reading analog potentiometers. Analog potentiometers read in an
 * analog voltage that corresponds to a position. The position is in whichever
 * units you choose, by way of the scaling and offset constants passed to the
 * constructor.
 */
class AnalogPotentiometer : public ErrorBase,
                            public Potentiometer,
                            public Sendable,
                            public SendableHelper<AnalogPotentiometer> {
 public:
  /**
   * Construct an Analog Potentiometer object from a channel number.
   *
   * Use the fullRange and offset values so that the output produces meaningful
   * values. I.E: you have a 270 degree potentiometer and you want the output to
   * be degrees with the halfway point as 0 degrees. The fullRange value is
   * 270.0 degrees and the offset is -135.0 since the halfway point after
   * scaling is 135 degrees.
   *
   * This will calculate the result from the fullRange times the fraction of the
   * supply voltage, plus the offset.
   *
   * @param channel   The channel number on the roboRIO to represent. 0-3 are
   *                  on-board 4-7 are on the MXP port.
   * @param fullRange The angular value (in desired units) representing the full
   *                  0-5V range of the input.
   * @param offset    The angular value (in desired units) representing the
   *                  angular output at 0V.
   */
  explicit AnalogPotentiometer(int channel, double fullRange = 1.0,
                               double offset = 0.0);

  /**
   * Construct an Analog Potentiometer object from an existing Analog Input
   * pointer.
   *
   * Use the fullRange and offset values so that the output produces meaningful
   * values. I.E: you have a 270 degree potentiometer and you want the output to
   * be degrees with the halfway point as 0 degrees. The fullRange value is
   * 270.0 degrees and the offset is -135.0 since the halfway point after
   * scaling is 135 degrees.
   *
   * This will calculate the result from the fullRange times the fraction of the
   * supply voltage, plus the offset.
   *
   * @param channel   The existing Analog Input pointer
   * @param fullRange The angular value (in desired units) representing the full
   *                  0-5V range of the input.
   * @param offset    The angular value (in desired units) representing the
   *                  angular output at 0V.
   */
  explicit AnalogPotentiometer(AnalogInput* input, double fullRange = 1.0,
                               double offset = 0.0);

  /**
   * Construct an Analog Potentiometer object from an existing Analog Input
   * pointer.
   *
   * Use the fullRange and offset values so that the output produces meaningful
   * values. I.E: you have a 270 degree potentiometer and you want the output to
   * be degrees with the halfway point as 0 degrees. The fullRange value is
   * 270.0 degrees and the offset is -135.0 since the halfway point after
   * scaling is 135 degrees.
   *
   * This will calculate the result from the fullRange times the fraction of the
   * supply voltage, plus the offset.
   *
   * @param channel   The existing Analog Input pointer
   * @param fullRange The angular value (in desired units) representing the full
   *                  0-5V range of the input.
   * @param offset    The angular value (in desired units) representing the
   *                  angular output at 0V.
   */
  explicit AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                               double fullRange = 1.0, double offset = 0.0);

  ~AnalogPotentiometer() override = default;

  AnalogPotentiometer(AnalogPotentiometer&&) = default;
  AnalogPotentiometer& operator=(AnalogPotentiometer&&) = default;

  /**
   * Get the current reading of the potentiomer.
   *
   * @return The current position of the potentiometer (in the units used for
   *         fullRange and offset).
   */
  double Get() const override;

  /**
   * Implement the PIDSource interface.
   *
   * @return The current reading.
   */
  double PIDGet() override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  std::shared_ptr<AnalogInput> m_analog_input;
  double m_fullRange, m_offset;
};

}  // namespace frc
