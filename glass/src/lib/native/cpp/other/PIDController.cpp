/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
    auto createTuningParameter = [](const char* name, double* v,
                                    std::function<void(double)> callback) {
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
      if (ImGui::InputDouble(name, v, 0.0, 0.0, "%.3f")) callback(*v);
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
