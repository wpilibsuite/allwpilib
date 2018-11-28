/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/ShuffleboardInstance.h"

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/StringMap.h>

#include "frc/shuffleboard/Shuffleboard.h"

using namespace frc::detail;

struct ShuffleboardInstance::Impl {
  wpi::StringMap<ShuffleboardTab> tabs;

  bool tabsChanged = false;
  std::shared_ptr<nt::NetworkTable> rootTable;
  std::shared_ptr<nt::NetworkTable> rootMetaTable;
};

ShuffleboardInstance::ShuffleboardInstance(nt::NetworkTableInstance ntInstance)
    : m_impl(new Impl) {
  m_impl->rootTable = ntInstance.GetTable(Shuffleboard::kBaseTableName);
  m_impl->rootMetaTable = m_impl->rootTable->GetSubTable(".metadata");
}

ShuffleboardInstance::~ShuffleboardInstance() {}

frc::ShuffleboardTab& ShuffleboardInstance::GetTab(wpi::StringRef title) {
  if (m_impl->tabs.find(title) == m_impl->tabs.end()) {
    m_impl->tabs.try_emplace(title, ShuffleboardTab(*this, title));
    m_impl->tabsChanged = true;
  }
  return m_impl->tabs.find(title)->second;
}

void ShuffleboardInstance::Update() {
  if (m_impl->tabsChanged) {
    std::vector<std::string> tabTitles;
    for (auto& entry : m_impl->tabs) {
      tabTitles.emplace_back(entry.second.GetTitle());
    }
    m_impl->rootMetaTable->GetEntry("Tabs").ForceSetStringArray(tabTitles);
    m_impl->tabsChanged = false;
  }
  for (auto& entry : m_impl->tabs) {
    auto& tab = entry.second;
    tab.BuildInto(m_impl->rootTable,
                  m_impl->rootMetaTable->GetSubTable(tab.GetTitle()));
  }
}

void ShuffleboardInstance::EnableActuatorWidgets() {
  for (auto& entry : m_impl->tabs) {
    auto& tab = entry.second;
    for (auto& component : tab.GetComponents()) {
      component->EnableIfActuator();
    }
  }
}

void ShuffleboardInstance::DisableActuatorWidgets() {
  for (auto& entry : m_impl->tabs) {
    auto& tab = entry.second;
    for (auto& component : tab.GetComponents()) {
      component->DisableIfActuator();
    }
  }
}

void ShuffleboardInstance::SelectTab(int index) {
  m_impl->rootMetaTable->GetEntry("Selected").ForceSetDouble(index);
}

void ShuffleboardInstance::SelectTab(wpi::StringRef title) {
  m_impl->rootMetaTable->GetEntry("Selected").ForceSetString(title);
}
