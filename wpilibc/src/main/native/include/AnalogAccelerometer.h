/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "AnalogInput.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "PIDSource.h"
#include "SensorBase.h"

namespace frc {

/**
 * Handle operation of an analog accelerometer.
 * The accelerometer reads acceleration directly through the sensor. Many
 * sensors have multiple axis and can be treated as multiple devices. Each is
 * calibrated by finding the center value over a period of time.
 */
class AnalogAccelerometer : public SensorBase,
                            public PIDSource,
                            public LiveWindowSendable {
 public:
  explicit AnalogAccelerometer(int channel);
  explicit AnalogAccelerometer(AnalogInput* channel);
  explicit AnalogAccelerometer(std::shared_ptr<AnalogInput> channel);
  virtual ~AnalogAccelerometer() = default;

  double GetAcceleration() const;
  void SetSensitivity(double sensitivity);
  void SetZero(double zero);
  double PIDGet() override;

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 private:
  void InitAccelerometer();

  std::shared_ptr<AnalogInput> m_analogInput;
  double m_voltsPerG = 1.0;
  double m_zeroGVoltage = 2.5;

  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
