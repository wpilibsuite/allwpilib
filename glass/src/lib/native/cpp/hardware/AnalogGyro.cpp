/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/AnalogGyro.h"

#include "glass/DataSource.h"
#include "glass/other/DeviceTree.h"

using namespace glass;

void glass::DisplayAnalogGyroDevice(AnalogGyroModel* model, int index) {
  char name[32];
  std::snprintf(name, sizeof(name), "AnalogGyro[%d]", index);
  if (BeginDevice(name)) {
    // angle
    if (auto angleData = model->GetAngleData()) {
      double value = angleData->GetValue();
      if (DeviceDouble("Angle", false, &value, angleData)) {
        model->SetAngle(value);
      }
    }

    // rate
    if (auto rateData = model->GetRateData()) {
      double value = rateData->GetValue();
      if (DeviceDouble("Rate", false, &value, rateData)) {
        model->SetRate(value);
      }
    }
    EndDevice();
  }
}

void glass::DisplayAnalogGyrosDevice(AnalogGyrosModel* model) {
  model->ForEachAnalogGyro(
      [&](AnalogGyroModel& gyro, int i) { DisplayAnalogGyroDevice(&gyro, i); });
}
