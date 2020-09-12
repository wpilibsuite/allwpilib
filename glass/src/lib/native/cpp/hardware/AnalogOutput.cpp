/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/AnalogOutput.h"

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/other/DeviceTree.h"

using namespace glass;

void glass::DisplayAnalogOutputsDevice(AnalogOutputsModel* model) {
  int count = 0;
  model->ForEachAnalogOutput([&](auto&, int) { ++count; });
  if (count == 0) return;

  if (BeginDevice("Analog Outputs")) {
    model->ForEachAnalogOutput([&](auto& analogOut, int i) {
      auto analogOutData = analogOut.GetVoltageData();
      if (!analogOutData) return;
      PushID(i);

      // build label
      std::string* name = GetStorage().GetStringRef("name");
      char label[128];
      if (!name->empty()) {
        std::snprintf(label, sizeof(label), "%s [%d]###name", name->c_str(), i);
      } else {
        std::snprintf(label, sizeof(label), "Out[%d]###name", i);
      }

      double value = analogOutData->GetValue();
      DeviceDouble(label, true, &value, analogOutData);

      if (PopupEditName("name", name)) {
        if (analogOutData) analogOutData->SetName(name->c_str());
      }
      PopID();
    });

    EndDevice();
  }
}
