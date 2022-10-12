// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ShuffleboardInstance.h"

#include <hal/FRCUsageReporting.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/SmallVector.h>
#include <wpi/StringMap.h>

#include "frc/shuffleboard/Shuffleboard.h"

using namespace frc::detail;

struct ShuffleboardInstance::Impl {
  wpi::StringMap<std::unique_ptr<ShuffleboardTab>> tabs;

  bool tabsChanged = false;
  std::shared_ptr<nt::NetworkTable> rootTable;
  std::shared_ptr<nt::NetworkTable> rootMetaTable;
};

ShuffleboardInstance::ShuffleboardInstance(nt::NetworkTableInstance ntInstance)
    : m_impl(new Impl) {
  m_impl->rootTable = ntInstance.GetTable(Shuffleboard::kBaseTableName);
  m_impl->rootMetaTable = m_impl->rootTable->GetSubTable(".metadata");
  HAL_Report(HALUsageReporting::kResourceType_Shuffleboard, 0);
}

ShuffleboardInstance::~ShuffleboardInstance() = default;

frc::ShuffleboardTab& ShuffleboardInstance::GetTab(std::string_view title) {
  if (m_impl->tabs.find(title) == m_impl->tabs.end()) {
    m_impl->tabs.try_emplace(title,
                             std::make_unique<ShuffleboardTab>(*this, title));
    m_impl->tabsChanged = true;
  }
  return *m_impl->tabs.find(title)->second;
}

void ShuffleboardInstance::Update() {
  if (m_impl->tabsChanged) {
    wpi::SmallVector<std::string, 16> tabTitles;
    for (auto& entry : m_impl->tabs) {
      tabTitles.emplace_back(entry.second->GetTitle());
    }
    m_impl->rootMetaTable->GetEntry("Tabs").SetStringArray(tabTitles);
    m_impl->tabsChanged = false;
  }
  for (auto& entry : m_impl->tabs) {
    auto& tab = *entry.second;
    tab.BuildInto(m_impl->rootTable,
                  m_impl->rootMetaTable->GetSubTable(tab.GetTitle()));
  }
}

void ShuffleboardInstance::EnableActuatorWidgets() {
  for (auto& entry : m_impl->tabs) {
    auto& tab = *entry.second;
    for (auto& component : tab.GetComponents()) {
      component->EnableIfActuator();
    }
  }
}

void ShuffleboardInstance::DisableActuatorWidgets() {
  for (auto& entry : m_impl->tabs) {
    auto& tab = *entry.second;
    for (auto& component : tab.GetComponents()) {
      component->DisableIfActuator();
    }
  }
}

void ShuffleboardInstance::SelectTab(int index) {
  m_impl->rootMetaTable->GetEntry("Selected").SetDouble(index);
}

void ShuffleboardInstance::SelectTab(std::string_view title) {
  m_impl->rootMetaTable->GetEntry("Selected").SetString(title);
}
