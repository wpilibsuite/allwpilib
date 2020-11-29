/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogGyroGui.h"

#include <cstdio>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/Value.h>
#include <hal/simulation/AnalogGyroData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogGyroAngle, "AGyro Angle");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogGyroRate, "AGyro Rate");
struct AnalogGyroSource {
  explicit AnalogGyroSource(int32_t index) : angle{index}, rate{index} {}
  AnalogGyroAngleSource angle;
  AnalogGyroRateSource rate;
};
}  // namespace

static std::vector<std::unique_ptr<AnalogGyroSource>> gAnalogGyroSources;

static void UpdateAnalogGyroSources() {
  for (int i = 0, iend = gAnalogGyroSources.size(); i < iend; ++i) {
    auto& source = gAnalogGyroSources[i];
    if (HALSIM_GetAnalogGyroInitialized(i)) {
      if (!source) {
        source = std::make_unique<AnalogGyroSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayAnalogGyros() {
  for (int i = 0, iend = gAnalogGyroSources.size(); i < iend; ++i) {
    if (auto source = gAnalogGyroSources[i].get()) {
      char name[32];
      std::snprintf(name, sizeof(name), "AnalogGyro[%d]", i);
      if (SimDeviceGui::StartDevice(name, HALSIM_GetAnalogGyroDisplayName(i))) {
        HAL_Value value;

        // angle
        value = HAL_MakeDouble(source->angle.GetValue());
        if (SimDeviceGui::DisplayValueSource("Angle", false, &value,
                                             &source->angle))
          HALSIM_SetAnalogGyroAngle(i, value.data.v_double);

        // rate
        value = HAL_MakeDouble(source->rate.GetValue());
        if (SimDeviceGui::DisplayValueSource("Rate", false, &value,
                                             &source->rate))
          HALSIM_SetAnalogGyroRate(i, value.data.v_double);

        SimDeviceGui::FinishDevice();
      }
    }
  }
}

void AnalogGyroGui::Initialize() {
  gAnalogGyroSources.resize(HAL_GetNumAccumulators());
  HALSimGui::AddExecute(UpdateAnalogGyroSources);
  SimDeviceGui::Add(DisplayAnalogGyros);
}
