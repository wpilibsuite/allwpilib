// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/hardware/LEDDisplay.hpp"

#include <vector>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/glass/support/ExtraGuiWidgets.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::glass;

namespace {
struct IndicatorData {
  std::vector<int> values;
  std::vector<ImU32> colors;
};
}  // namespace

void wpi::glass::DisplayLEDDisplay(LEDDisplayModel* model, int index) {
  wpi::util::SmallVector<LEDDisplayModel::Data, 64> dataBuf;
  auto data = model->GetData(dataBuf);
  int length = data.size();
  auto& storage = GetStorage();

  int& numColumns = storage.GetInt("columns", 10);
  bool& serpentine = storage.GetBool("serpentine", false);
  int& order = storage.GetInt("order", LEDConfig::RowMajor);
  int& start = storage.GetInt("start", LEDConfig::UpperLeft);

  ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
  ImGui::LabelText("Length", "%d", length);
  ImGui::InputInt("Columns", &numColumns);
  {
    static const char* options[] = {"Row Major", "Column Major"};
    ImGui::Combo("Order", &order, options, 2);
  }
  {
    static const char* options[] = {"Upper Left", "Lower Left", "Upper Right",
                                    "Lower Right"};
    ImGui::Combo("Start", &start, options, 4);
  }
  ImGui::Checkbox("Serpentine", &serpentine);
  if (numColumns < 1) {
    numColumns = 1;
  }
  ImGui::PopItemWidth();

  // show as LED indicators
  auto iData = storage.GetData<IndicatorData>();
  if (!iData) {
    storage.SetData(std::make_shared<IndicatorData>());
    iData = storage.GetData<IndicatorData>();
  }
  if (length > static_cast<int>(iData->values.size())) {
    iData->values.resize(length);
  }
  if (length > static_cast<int>(iData->colors.size())) {
    iData->colors.resize(length);
  }
  for (int j = 0; j < length; ++j) {
    iData->values[j] = j + 1;
    iData->colors[j] = IM_COL32(data[j].r, data[j].g, data[j].b, 255);
  }

  LEDConfig config;
  config.serpentine = serpentine;
  config.order = static_cast<LEDConfig::Order>(order);
  config.start = static_cast<LEDConfig::Start>(start);

  DrawLEDs(iData->values.data(), length, numColumns, iData->colors.data(), 0, 0,
           config);
}

void wpi::glass::DisplayLEDDisplays(LEDDisplaysModel* model) {
  bool hasAny = false;

  model->ForEachLEDDisplay([&](LEDDisplayModel& display, int i) {
    hasAny = true;
    if (model->GetNumLEDDisplays() > 1) {
      ImGui::Text("LEDs[%d]", i);
    }
    PushID(i);
    DisplayLEDDisplay(&display, i);
    PopID();
  });
  if (!hasAny) {
    ImGui::Text("No addressable LEDs");
  }
}
