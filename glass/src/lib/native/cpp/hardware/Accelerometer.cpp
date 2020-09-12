/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/Accelerometer.h"

#include "glass/DataSource.h"
#include "glass/other/DeviceTree.h"

using namespace glass;

void glass::DisplayAccelerometerDevice(AccelerometerModel* model) {
  if (!model->Exists()) return;
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
