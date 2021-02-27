// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/PIDController.h"

#include <string>

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayPIDController(PIDControllerModel* m) {
  if (auto name = m->GetName()) {
    ImGui::Text("%s", name);
    ImGui::Separator();
  }

  if (m->Exists()) {
    auto flag = m->IsReadOnly() ? ImGuiInputTextFlags_ReadOnly
                                : ImGuiInputTextFlags_None;
    auto createTuningParameter = [flag](const char* name, double* v,
                                        std::function<void(double)> callback) {
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
      if (ImGui::InputDouble(name, v, 0.0, 0.0, "%.3f", flag)) {
        callback(*v);
      }
    };

    if (auto p = m->GetPData()) {
      double value = p->GetValue();
      createTuningParameter("P", &value, [=](auto v) { m->SetP(v); });
    }
    if (auto i = m->GetIData()) {
      double value = i->GetValue();
      createTuningParameter("I", &value, [=](auto v) { m->SetI(v); });
    }
    if (auto d = m->GetDData()) {
      double value = d->GetValue();
      createTuningParameter("D", &value, [=](auto v) { m->SetD(v); });
    }
    if (auto s = m->GetSetpointData()) {
      double value = s->GetValue();
      createTuningParameter("Setpoint", &value,
                            [=](auto v) { m->SetSetpoint(v); });
    }
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown PID Controller");
    ImGui::PopStyleColor();
  }
}
