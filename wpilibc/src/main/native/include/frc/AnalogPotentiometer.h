// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
   * @param channel   The Analog Input channel number on the roboRIO the
   *                  potentiometer is plugged into. 0-3 are on-board and 4-7
   *                  are on the MXP port.
   * @param fullRange The value (in desired units) representing the full
   *                  0-5V range of the input.
   * @param offset    The value (in desired units) representing the
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
   * @param fullRange The value (in desired units) representing the full
   *                  0-5V range of the input.
   * @param offset    The value (in desired units) representing the
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
   * @param fullRange The value (in desired units) representing the full
   *                  0-5V range of the input.
   * @param offset    The value (in desired units) representing the
   *                  angular output at 0V.
   */
  explicit AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                               double fullRange = 1.0, double offset = 0.0);

  ~AnalogPotentiometer() override = default;

  AnalogPotentiometer(AnalogPotentiometer&&) = default;
  AnalogPotentiometer& operator=(AnalogPotentiometer&&) = default;

  /**
   * Get the current reading of the potentiometer.
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
