/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/Encoder.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void EncoderModel::SetName(const wpi::Twine& name) {
  if (name.str().empty()) {
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
      distancePerPulse->SetName(name + " Distance/Count");
    }
    if (auto count = GetCountData()) {
      count->SetName(name + " Count");
    }
    if (auto period = GetPeriodData()) {
      period->SetName(name + " Period");
    }
    if (auto direction = GetDirectionData()) {
      direction->SetName(name + " Direction");
    }
    if (auto distance = GetDistanceData()) {
      distance->SetName(name + " Distance");
    }
    if (auto rate = GetRateData()) {
      rate->SetName(name + " Rate");
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
  std::string* name = GetStorage().GetStringRef("name");
  char label[128];
  if (!name->empty()) {
    std::snprintf(label, sizeof(label), "%s [%d,%d]###name", name->c_str(), chA,
                  chB);
  } else {
    std::snprintf(label, sizeof(label), "Encoder[%d,%d]###name", chA, chB);
  }

  // header
  bool open = CollapsingHeader(label);

  // context menu to change name
  if (PopupEditName("name", name)) {
    model->SetName(name->c_str());
  }

  if (!open) return;

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  // distance per pulse
  if (auto distancePerPulseData = model->GetDistancePerPulseData()) {
    double value = distancePerPulseData->GetValue();
    distancePerPulseData->LabelText("Dist/Count", "%.6f", value);
  }

  // count
  if (auto countData = model->GetCountData()) {
    int value = countData->GetValue();
    if (ImGui::InputInt("##input", &value)) model->SetCount(value);
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

void glass::DisplayEncoders(EncodersModel* model, wpi::StringRef noneMsg) {
  bool hasAny = false;
  model->ForEachEncoder([&](EncoderModel& encoder, int i) {
    hasAny = true;
    PushID(i);
    DisplayEncoder(&encoder);
    PopID();
  });
  if (!hasAny && !noneMsg.empty())
    ImGui::TextUnformatted(noneMsg.begin(), noneMsg.end());
}
