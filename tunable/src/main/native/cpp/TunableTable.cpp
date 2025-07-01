// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/TunableTable.hpp"

#include <format>
#include <string>

#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/detail/TunableMember.hpp"

using namespace wpi;

TunableTable TunableTable::GetTable(std::string_view name) {
  return TunableTable{std::format("{}{}/", m_path, name)};
}

void TunableTable::Publish(std::string_view name,
                           detail::TunableBase& tunable) {
  std::string buf;
  TunableRegistry::Publish(
      TunableRegistry::NormalizeName(std::format("{}{}", m_path, name), buf),
      tunable);
}

void TunableTable::Publish(std::string_view name, ComplexTunable& tunable) {
  Publish(name, static_cast<detail::TunableBase&>(tunable));
  std::string buf;
  std::string_view normalized = TunableRegistry::NormalizeName(name, buf);
  TunableTable table = GetTable(normalized.substr(1));
  tunable.PublishTunable(table);
}

void TunableTable::Publish(std::string_view name, ComplexTunable* tunable,
                           std::unique_ptr<detail::TunableMemberBase> member) {
  std::string buf;
  TunableRegistry::Publish(
      TunableRegistry::NormalizeName(std::format("{}{}", m_path, name), buf),
      tunable, std::move(member));
}

/**
 * Removes a tunable.
 * @param name name
 */
void TunableTable::Remove(std::string_view name) {
  std::string buf;
  TunableRegistry::Remove(
      TunableRegistry::NormalizeName(std::format("{}{}", m_path, name), buf));
}
