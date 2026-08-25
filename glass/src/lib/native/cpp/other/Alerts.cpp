// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/other/Alerts.hpp"

#include <algorithm>
#include <vector>

#include <IconsFontAwesome6.h>
#include <imgui.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"

using namespace wpi::glass;

namespace {

int GetLevelSortOrder(AlertData::Level level) {
  switch (level) {
    case AlertData::Level::HIGH:
      return 0;
    case AlertData::Level::MEDIUM:
      return 1;
    case AlertData::Level::LOW:
      return 2;
  }
  return 3;
}

ImVec4 GetLevelColor(AlertData::Level level) {
  switch (level) {
    case AlertData::Level::HIGH:
      return {0.9f, 0.25f, 0.25f, 1.0f};
    case AlertData::Level::MEDIUM:
      return {0.85f, 0.56f, 0.0f, 1.0f};
    case AlertData::Level::LOW:
      return ImGui::GetStyleColorVec4(ImGuiCol_Text);
  }
  return ImGui::GetStyleColorVec4(ImGuiCol_Text);
}

void DisplayAlertTable(const std::vector<const AlertData*>& alerts,
                       size_t begin, size_t end, bool showId,
                       bool showActiveStartTime) {
  int columnCount = 1;
  if (showId) {
    ++columnCount;
  }
  if (showActiveStartTime) {
    ++columnCount;
  }

  if (ImGui::BeginTable("Alerts", columnCount,
                        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Text", ImGuiTableColumnFlags_WidthStretch);
    if (showId) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
    }
    if (showActiveStartTime) {
      ImGui::TableSetupColumn("Started", ImGuiTableColumnFlags_WidthFixed);
    }
    ImGui::TableHeadersRow();

    for (size_t i = begin; i < end; ++i) {
      const AlertData* alert = alerts[i];
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(alert->level));
      float availableWidth = ImGui::GetContentRegionAvail().x;
      ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + availableWidth);
      ImGui::TextUnformatted(alert->text.c_str());
      ImGui::PopTextWrapPos();
      ImGui::PopStyleColor();
      if (showId) {
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(alert->id.c_str());
      }
      if (showActiveStartTime) {
        ImGui::TableNextColumn();
        if (alert->activeStartTime != 0) {
          ImGui::Text("%.3f", TimestampToDisplayTime(alert->activeStartTime));
        }
      }
    }
    ImGui::EndTable();
  }
}

}  // namespace

AlertsView::AlertsView(AlertsModel* model, Storage& storage)
    : m_model{model}, m_showIds{storage.GetBool("showIds", false)} {}

void wpi::glass::DisplayAlerts(AlertsModel* model, bool showIds) {
  const auto& alerts = model->GetAlerts();
  if (alerts.empty()) {
    ImGui::TextUnformatted("No active alerts");
    return;
  }

  bool showGroup = false;
  bool hasId = false;
  bool showActiveStartTime = false;
  std::vector<const AlertData*> sortedAlerts;
  sortedAlerts.reserve(alerts.size());
  for (auto&& alert : alerts) {
    sortedAlerts.emplace_back(&alert);
    showGroup = showGroup || !alert.group.empty();
    hasId = hasId || !alert.id.empty();
    showActiveStartTime = showActiveStartTime || alert.activeStartTime != 0;
  }
  bool showId = showIds && hasId;

  std::stable_sort(sortedAlerts.begin(), sortedAlerts.end(),
                   [](const AlertData* lhs, const AlertData* rhs) {
                     int lhsLevel = GetLevelSortOrder(lhs->level);
                     int rhsLevel = GetLevelSortOrder(rhs->level);
                     if (lhsLevel != rhsLevel) {
                       return lhsLevel < rhsLevel;
                     }
                     if (lhs->group.empty() != rhs->group.empty()) {
                       return !lhs->group.empty();
                     }
                     if (lhs->group != rhs->group) {
                       return lhs->group < rhs->group;
                     }
                     return lhs->activeStartTime > rhs->activeStartTime;
                   });

  size_t levelBegin = 0;
  while (levelBegin < sortedAlerts.size()) {
    AlertData::Level level = sortedAlerts[levelBegin]->level;
    size_t levelEnd = levelBegin + 1;
    while (levelEnd < sortedAlerts.size() &&
           sortedAlerts[levelEnd]->level == level) {
      ++levelEnd;
    }

    const char* levelLabel = "Unknown";
    switch (level) {
      case AlertData::Level::HIGH:
        levelLabel = ICON_FA_CIRCLE_XMARK " High";
        break;
      case AlertData::Level::MEDIUM:
        levelLabel = ICON_FA_TRIANGLE_EXCLAMATION " Medium";
        break;
      case AlertData::Level::LOW:
        levelLabel = ICON_FA_CIRCLE_INFO " Low";
        break;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(level));
    bool levelOpen = TreeNodeEx(levelLabel, ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextDisabled("(%zu)", levelEnd - levelBegin);

    if (levelOpen) {
      if (showGroup) {
        size_t groupBegin = levelBegin;
        while (groupBegin < levelEnd) {
          const char* groupName = sortedAlerts[groupBegin]->group.empty()
                                      ? "Ungrouped"
                                      : sortedAlerts[groupBegin]->group.c_str();
          size_t groupEnd = groupBegin + 1;
          while (groupEnd < levelEnd && sortedAlerts[groupEnd]->group ==
                                            sortedAlerts[groupBegin]->group) {
            ++groupEnd;
          }

          bool groupOpen =
              TreeNodeEx(groupName, ImGuiTreeNodeFlags_DefaultOpen);
          ImGui::SameLine();
          ImGui::TextDisabled("(%zu)", groupEnd - groupBegin);
          if (groupOpen) {
            DisplayAlertTable(sortedAlerts, groupBegin, groupEnd, showId,
                              showActiveStartTime);
            TreePop();
          }
          groupBegin = groupEnd;
        }
      } else {
        DisplayAlertTable(sortedAlerts, levelBegin, levelEnd, showId,
                          showActiveStartTime);
      }
      TreePop();
    }

    levelBegin = levelEnd;
  }
}

void AlertsView::Display() {
  DisplayAlerts(m_model, m_showIds);
}

void AlertsView::Settings() {
  ImGui::Checkbox("Show IDs", &m_showIds);
}

bool AlertsView::HasSettings() {
  return true;
}
