// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/DataSourceView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <imgui.h>

namespace wpi::filterdesigner {

void DataSourceView::Display() {
  if (ImGui::BeginTabBar("##datasource")) {
    // BeginTabItem returns true only for the active tab, so m_mode tracks
    // whichever tab the user is currently looking at.
    if (ImGui::BeginTabItem("WPILOG")) {
      m_mode = Mode::WpiLog;
      m_logLoader.Display();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Live NT4")) {
      m_mode = Mode::Nt4;
      m_nt4.Display();
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

const Signal* DataSourceView::SelectedSignal() const {
  switch (m_mode) {
    case Mode::WpiLog:
      return m_logLoader.SelectedSignal();
    case Mode::Nt4:
      return m_nt4.SelectedSignal();
  }
  return nullptr;
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
