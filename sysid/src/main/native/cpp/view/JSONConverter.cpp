// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/JSONConverter.h"
#include "sysid/view/JSONConverter.h"

#include <exception>

#include <imgui.h>
#include <portable-file-dialogs.h>
#include <wpi/timestamp.h>

#include "sysid/Util.h"

using namespace sysid;

void JSONConverter::DisplayConverter(
    const char* tooltip,
    std::function<std::string(std::string_view, wpi::Logger&)> converter) {
  if (ImGui::Button(tooltip)) {
    m_opener = std::make_unique<pfd::open_file>(
        tooltip, "", std::vector<std::string>{"JSON File", SYSID_PFD_JSON_EXT});
  }

  if (m_opener && m_opener->ready()) {
    if (!m_opener->result().empty()) {
      m_location = m_opener->result()[0];
      try {
        converter(m_location, m_logger);
        m_timestamp = wpi::Now() * 1E-6;
      } catch (const std::exception& e) {
        ImGui::OpenPopup("Exception Caught!");
        m_exception = e.what();
      }
    }
    m_opener.reset();
  }

  if (wpi::Now() * 1E-6 - m_timestamp < 5) {
    ImGui::SameLine();
    ImGui::Text("Saved!");
  }

  // Handle exceptions.
  ImGui::SetNextWindowSize(ImVec2(480.f, 0.0f));
  if (ImGui::BeginPopupModal("Exception Caught!")) {
    ImGui::PushTextWrapPos(0.0f);
    ImGui::Text(
        "An error occurred when parsing the JSON. This most likely means that "
        "the JSON data is incorrectly formatted.");
    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s",
                       m_exception.c_str());
    ImGui::PopTextWrapPos();
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void JSONConverter::DisplayCSVConvert() {
  DisplayConverter("Select SysId JSON", sysid::ToCSV);
}
