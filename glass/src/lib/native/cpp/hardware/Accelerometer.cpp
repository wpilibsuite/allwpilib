// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/Accelerometer.h"

#include "glass/DataSource.h"
#include "glass/other/DeviceTree.h"

using namespace glass;

void glass::DisplayAccelerometerDevice(AccelerometerModel* model) {
  if (!model->Exists()) {
    return;
  }
  if (BeginDevice("BuiltInAccel")) {
    // Range
    {
      int value = model->GetRange();
      static const char* rangeOptions[] = {"2G", "4G", "8G"};
      DeviceEnum("Range", true, &value, rangeOptions, 3);
    }

    // X Accel
    if (auto xData = model->GetXData()) {
      double value = xData->GetValue();
      if (DeviceDouble("X Accel", false, &value, xData)) {
        model->SetX(value);
      }
    }

    // Y Accel
    if (auto yData = model->GetYData()) {
      double value = yData->GetValue();
      if (DeviceDouble("Y Accel", false, &value, yData)) {
        model->SetY(value);
      }
    }

    // Z Accel
    if (auto zData = model->GetZData()) {
      double value = zData->GetValue();
      if (DeviceDouble("Z Accel", false, &value, zData)) {
        model->SetZ(value);
      }
    }

    EndDevice();
  }
}
