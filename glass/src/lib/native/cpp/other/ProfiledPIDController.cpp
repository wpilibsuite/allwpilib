// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/ProfiledPIDController.h"

#include <string>

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayProfiledPIDController(ProfiledPIDControllerModel* m) {
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
    // Workaround to allow for the input of inf, -inf, and nan
    auto createTuningParameterNoFilter =
        [flag](const char* name, double* v,
               std::function<void(double)> callback) {
          ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
          if (ImGui::InputScalar(name, ImGuiDataType_Double, v, NULL, NULL,
                                 "%.3f", flag)) {
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
    if (auto s = m->GetIZoneData()) {
      double value = s->GetValue();
      createTuningParameterNoFilter("IZone", &value,
                                    [=](auto v) { m->SetIZone(v); });
    }
    if (auto s = m->GetMaxVelocityData()) {
      double value = s->GetValue();
      createTuningParameter("Max Velocity", &value,
                            [=](auto v) { m->SetMaxVelocity(v); });
    }
    if (auto s = m->GetMaxAccelerationData()) {
      double value = s->GetValue();
      createTuningParameter("Max Acceleration", &value,
                            [=](auto v) { m->SetMaxAcceleration(v); });
    }
    if (auto s = m->GetGoalData()) {
      double value = s->GetValue();
      createTuningParameter("Goal", &value, [=](auto v) { m->SetGoal(v); });
    }
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown PID Controller");
    ImGui::PopStyleColor();
  }
}
