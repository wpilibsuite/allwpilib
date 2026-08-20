// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/TunableTable.hpp"

#include <format>
#include <memory>
#include <string>
#include <utility>

#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/detail/TunableMember.hpp"

using namespace wpi;
using namespace wpi::tunable;

TunableTable TunableTable::GetTable(std::string_view name) {
  return TunableTable{std::format("{}{}/", m_path, name)};
}

bool TunableTable::Publish(std::string_view name,
                           detail::TunableBase& tunable) {
  std::string buf;
  return TunableRegistry::Publish(
      TunableRegistry::NormalizeName(std::format("{}{}", m_path, name), buf),
      tunable);
}

bool TunableTable::Publish(std::string_view name, ComplexTunable& tunable) {
  std::string buf;
  return TunableRegistry::Publish(
      TunableRegistry::NormalizeName(std::format("{}{}", m_path, name), buf),
      tunable);
}

bool TunableTable::Publish(std::string_view name, ComplexTunable* tunable,
                           std::unique_ptr<detail::TunableMemberBase> member) {
  std::string buf;
  return TunableRegistry::Publish(
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
