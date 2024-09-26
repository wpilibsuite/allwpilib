// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Log.h"

#include <string>

#include <imgui.h>

using namespace glass;

LogData::LogData(size_t maxLines) : m_maxLines{maxLines} {}

void LogData::Clear() {
  m_buf.clear();
  m_lineOffsets.clear();
  m_lineOffsets.push_back(0);
}

void LogData::Append(std::string_view msg) {
  if (m_lineOffsets.size() >= m_maxLines) {
    Clear();
  }

  size_t oldSize = m_buf.size();
  m_buf.append(msg);
  for (size_t newSize = m_buf.size(); oldSize < newSize; ++oldSize) {
    if (m_buf[oldSize] == '\n') {
      m_lineOffsets.push_back(oldSize + 1);
    }
  }
}

const std::string& LogData::GetBuffer() {
  return m_buf;
}

void glass::DisplayLog(LogData* data, bool autoScroll) {
  if (data->m_buf.empty()) {
    return;
  }
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  const char* buf = data->m_buf.data();
  const char* bufEnd = buf + data->m_buf.size();
  ImGuiListClipper clipper;
  clipper.Begin(data->m_lineOffsets.size());
  while (clipper.Step()) {
    for (size_t lineNum = clipper.DisplayStart;
         lineNum < static_cast<size_t>(clipper.DisplayEnd); lineNum++) {
      const char* lineStart = buf + data->m_lineOffsets[lineNum];
      const char* lineEnd = (lineNum + 1 < data->m_lineOffsets.size())
                                ? (buf + data->m_lineOffsets[lineNum + 1] - 1)
                                : bufEnd;
      ImGui::TextUnformatted(lineStart, lineEnd);
    }
  }
  clipper.End();
  ImGui::PopStyleVar();

  if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1.0f);
  }
}

void LogView::Display() {
  DisplayLog(m_data, m_autoScroll);
}

void LogView::Settings() {
  ImGui::Checkbox("Auto-scroll", &m_autoScroll);
  if (ImGui::Selectable("Clear")) {
    m_data->Clear();
  }
  const auto& buf = m_data->GetBuffer();
  if (ImGui::Selectable("Copy to Clipboard", false,
                        buf.empty() ? ImGuiSelectableFlags_Disabled : 0)) {
    ImGui::SetClipboardText(buf.c_str());
  }
}

bool LogView::HasSettings() {
  return true;
}
