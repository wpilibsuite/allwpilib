/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "AnalogInput.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "interfaces/Potentiometer.h"

namespace frc {

/**
 * Class for reading analog potentiometers. Analog potentiometers read
 * in an analog voltage that corresponds to a position. The position is
 * in whichever units you choose, by way of the scaling and offset
 * constants passed to the constructor.
 */
class AnalogPotentiometer : public Potentiometer, public LiveWindowSendable {
 public:
  /**
   * AnalogPotentiometer constructor.
   *
   * Use the fullRange and offset values so that the output produces
   * meaningful values. I.E: you have a 270 degree potentiometer and
   * you want the output to be degrees with the halfway point as 0
   * degrees. The fullRange value is 270.0(degrees) and the offset is
   * -135.0 since the halfway point after scaling is 135 degrees.
   *
   * This will calculate the result from the fullRange times the
   * fraction of the supply voltage, plus the offset.
   *
   * @param channel The analog channel this potentiometer is plugged into.
   * @param fullRange The scaling to multiply the voltage by to get a meaningful
   * unit.
   * @param offset The offset to add to the scaled value for controlling the
   * zero value
   */
  explicit AnalogPotentiometer(int channel, double fullRange = 1.0,
                               double offset = 0.0);

  explicit AnalogPotentiometer(AnalogInput* input, double fullRange = 1.0,
                               double offset = 0.0);

  explicit AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                               double fullRange = 1.0, double offset = 0.0);

  virtual ~AnalogPotentiometer() = default;

  /**
   * Get the current reading of the potentiomer.
   *
   * @return The current position of the potentiometer.
   */
  double Get() const override;

  /**
   * Implement the PIDSource interface.
   *
   * @return The current reading.
   */
  double PIDGet() override;

  /*
   * Live Window code, only does anything if live window is activated.
   */
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subtable) override;
  void UpdateTable() override;
  std::shared_ptr<ITable> GetTable() const override;

  /**
   * AnalogPotentiometers don't have to do anything special when entering the
   * LiveWindow.
   */
  void StartLiveWindowMode() override {}

  /**
   * AnalogPotentiometers don't have to do anything special when exiting the
   * LiveWindow.
   */
  void StopLiveWindowMode() override {}

 private:
  std::shared_ptr<AnalogInput> m_analog_input;
  double m_fullRange, m_offset;
  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
