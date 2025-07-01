// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Tunables.hpp"

#include <string>

#include <fmt/format.h>

#include "wpi/tunable/TunableBackend.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/TunableTable.hpp"

using namespace wpi;

TunableTable Tunables::GetTable() {
  return TunableTable{"/"};
}

TunableTable Tunables::GetTable(std::string_view name) {
  return TunableTable{fmt::format("/{}", name)};
}

void Tunables::Publish(std::string_view name, detail::TunableBase& tunable) {
  std::string buf;
  TunableRegistry::Publish(
      TunableRegistry::NormalizeName(fmt::format("/{}", name), buf), tunable);
}

void Tunables::Publish(std::string_view name, ComplexTunable* tunable,
                       std::unique_ptr<detail::TunableMemberBase> member) {
  std::string buf;
  TunableRegistry::Publish(
      TunableRegistry::NormalizeName(fmt::format("/{}", name), buf), tunable,
      std::move(member));
}

void Tunables::Remove(std::string_view name) {
  std::string buf;
  TunableRegistry::Remove(
      TunableRegistry::NormalizeName(fmt::format("/{}", name), buf));
}
