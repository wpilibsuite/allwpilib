// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/AnalogOutput.h"

#include <string>

#include <wpi/StringExtras.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"
#include "glass/other/DeviceTree.h"

using namespace glass;

void glass::DisplayAnalogOutputsDevice(AnalogOutputsModel* model) {
  int count = 0;
  model->ForEachAnalogOutput([&](auto&, int) { ++count; });
  if (count == 0) {
    return;
  }

  if (BeginDevice("Analog Outputs")) {
    model->ForEachAnalogOutput([&](auto& analogOut, int i) {
      auto analogOutData = analogOut.GetVoltageData();
      if (!analogOutData) {
        return;
      }
      PushID(i);

      // build label
      std::string& name = GetStorage().GetString("name");
      char label[128];
      if (!name.empty()) {
        wpi::format_to_n_c_str(label, sizeof(label), "{} [{}]###name", name, i);
      } else {
        wpi::format_to_n_c_str(label, sizeof(label), "Out[{}]###name", i);
      }

      double value = analogOutData->GetValue();
      DeviceDouble(label, true, &value, analogOutData);

      if (PopupEditName("name", &name)) {
        if (analogOutData) {
          analogOutData->SetName(name);
        }
      }
      PopID();
    });

    EndDevice();
  }
}
