// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/Encoder.h"

#include <string>

#include <fmt/format.h>
#include <imgui.h>
#include <wpi/StringExtras.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"

using namespace glass;

void EncoderModel::SetName(std::string_view name) {
  if (name.empty()) {
    if (auto distancePerPulse = GetDistancePerPulseData()) {
      distancePerPulse->SetName("");
    }
    if (auto count = GetCountData()) {
      count->SetName("");
    }
    if (auto period = GetPeriodData()) {
      period->SetName("");
    }
    if (auto direction = GetDirectionData()) {
      direction->SetName("");
    }
    if (auto distance = GetDistanceData()) {
      distance->SetName("");
    }
    if (auto rate = GetRateData()) {
      rate->SetName("");
    }
  } else {
    if (auto distancePerPulse = GetDistancePerPulseData()) {
      distancePerPulse->SetName(fmt::format("{} Distance/Count", name));
    }
    if (auto count = GetCountData()) {
      count->SetName(fmt::format("{} Count", name));
    }
    if (auto period = GetPeriodData()) {
      period->SetName(fmt::format("{} Period", name));
    }
    if (auto direction = GetDirectionData()) {
      direction->SetName(fmt::format("{} Direction", name));
    }
    if (auto distance = GetDistanceData()) {
      distance->SetName(fmt::format("{} Distance", name));
    }
    if (auto rate = GetRateData()) {
      rate->SetName(fmt::format("{} Rate", name));
    }
  }
}

void glass::DisplayEncoder(EncoderModel* model) {
  if (auto simDevice = model->GetSimDevice()) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::TextUnformatted(simDevice);
    ImGui::PopStyleColor();
    return;
  }

  int chA = model->GetChannelA();
  int chB = model->GetChannelB();

  // build header label
  std::string& name = GetStorage().GetString("name");
  char label[128];
  if (!name.empty()) {
    wpi::format_to_n_c_str(label, sizeof(label), "{} [{},{}]###header", name,
                           chA, chB);
  } else {
    wpi::format_to_n_c_str(label, sizeof(label), "Encoder[{},{}]###header", chA,
                           chB);
  }

  // header
  bool open = CollapsingHeader(label);

  // context menu to change name
  if (PopupEditName("header", &name)) {
    model->SetName(name);
  }

  if (!open) {
    return;
  }

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  // distance per pulse
  if (auto distancePerPulseData = model->GetDistancePerPulseData()) {
    double value = distancePerPulseData->GetValue();
    distancePerPulseData->LabelText("Dist/Count", "%.6f", value);
  }

  // count
  if (auto countData = model->GetCountData()) {
    int value = countData->GetValue();
    if (ImGui::InputInt("##input", &value)) {
      model->SetCount(value);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      model->SetCount(0);
    }
    ImGui::SameLine();
    ImGui::Selectable("Count");
    countData->EmitDrag();
  }

  // max period
  {
    double maxPeriod = model->GetMaxPeriod();
    ImGui::LabelText("Max Period", "%.6f", maxPeriod);
  }

  // period
  if (auto periodData = model->GetPeriodData()) {
    double value = periodData->GetValue();
    if (periodData->InputDouble("Period", &value, 0, 0, "%.6g")) {
      model->SetPeriod(value);
    }
  }

  // reverse direction
  ImGui::LabelText("Reverse Direction", "%s",
                   model->GetReverseDirection() ? "true" : "false");

  // direction
  if (auto directionData = model->GetDirectionData()) {
    static const char* options[] = {"reverse", "forward"};
    int value = directionData->GetValue() ? 1 : 0;
    if (directionData->Combo("Direction", &value, options, 2)) {
      model->SetDirection(value != 0);
    }
  }

  // distance
  if (auto distanceData = model->GetDistanceData()) {
    double value = distanceData->GetValue();
    if (distanceData->InputDouble("Distance", &value, 0, 0, "%.6g")) {
      model->SetDistance(value);
    }
  }

  // rate
  if (auto rateData = model->GetRateData()) {
    double value = rateData->GetValue();
    if (rateData->InputDouble("Rate", &value, 0, 0, "%.6g")) {
      model->SetRate(value);
    }
  }
  ImGui::PopItemWidth();
}

void glass::DisplayEncoders(EncodersModel* model, std::string_view noneMsg) {
  bool hasAny = false;
  model->ForEachEncoder([&](EncoderModel& encoder, int i) {
    hasAny = true;
    PushID(i);
    DisplayEncoder(&encoder);
    PopID();
  });
  if (!hasAny && !noneMsg.empty()) {
    ImGui::TextUnformatted(noneMsg.data(), noneMsg.data() + noneMsg.size());
  }
}
