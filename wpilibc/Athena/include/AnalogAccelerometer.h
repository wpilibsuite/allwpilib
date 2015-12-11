/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "AnalogInput.h"
#include "SensorBase.h"
#include "PIDSource.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

/**
 * Handle operation of an analog accelerometer.
 * The accelerometer reads acceleration directly through the sensor. Many
 * sensors have
 * multiple axis and can be treated as multiple devices. Each is calibrated by
 * finding
 * the center value over a period of time.
 */
class AnalogAccelerometer : public SensorBase,
                            public PIDSource,
                            public LiveWindowSendable {
 public:
  explicit AnalogAccelerometer(int32_t channel);
  explicit AnalogAccelerometer(AnalogInput *channel);
  explicit AnalogAccelerometer(std::shared_ptr<AnalogInput> channel);
  virtual ~AnalogAccelerometer() = default;

  float GetAcceleration() const;
  void SetSensitivity(float sensitivity);
  void SetZero(float zero);
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
  float m_voltsPerG = 1.0;
  float m_zeroGVoltage = 2.5;

  std::shared_ptr<ITable> m_table;
};
