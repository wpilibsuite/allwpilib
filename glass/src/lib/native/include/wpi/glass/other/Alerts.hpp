// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>
#include <vector>

#include "wpi/glass/Model.hpp"
#include "wpi/glass/View.hpp"

namespace wpi::glass {

class Storage;

/** Display information for a single active alert. */
struct AlertData {
  /** Alert urgency level. */
  enum class Level { HIGH, MEDIUM, LOW };

  AlertData() = default;

  /**
   * Constructs alert display data.
   *
   * @param group Group identifier
   * @param id Alert identifier
   * @param text Alert text
   * @param activeStartTime Time the alert became active, or 0 if unavailable
   * @param level Alert urgency level
   */
  AlertData(std::string_view group, std::string_view id, std::string_view text,
            int64_t activeStartTime, Level level)
      : group{group},
        id{id},
        text{text},
        activeStartTime{activeStartTime},
        level{level} {}

  /** Group identifier. */
  std::string group;

  /** Alert identifier. */
  std::string id;

  /** Alert text. */
  std::string text;

  /** Time the alert became active, or 0 if unavailable. */
  int64_t activeStartTime = 0;

  /** Alert urgency level. */
  Level level = Level::LOW;
};

class AlertsModel : public Model {
 public:
  /**
   * Gets active alerts.
   *
   * @return active alerts
   */
  virtual const std::vector<AlertData>& GetAlerts() = 0;
};

/**
 * Displays active alerts grouped by level and group.
 *
 * @param model alerts model
 * @param showIds true to display non-empty alert IDs
 */
void DisplayAlerts(AlertsModel* model, bool showIds = true);

class AlertsView : public View {
 public:
  /**
   * Constructs an alerts view.
   *
   * @param model alerts model
   * @param storage view storage
   */
  AlertsView(AlertsModel* model, Storage& storage);

  void Display() override;
  void Settings() override;
  bool HasSettings() override;

 private:
  AlertsModel* m_model;
  bool& m_showIds;
};

}  // namespace wpi::glass
