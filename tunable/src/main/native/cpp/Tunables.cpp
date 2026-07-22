// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Tunables.hpp"

#include <memory>
#include <utility>

#include "wpi/tunable/TunableTable.hpp"

using namespace wpi;

TunableTable Tunables::GetTable() {
  return TunableTable{"/"};
}

TunableTable Tunables::GetTable(std::string_view name) {
  return GetTable().GetTable(name);
}

void Tunables::Publish(std::string_view name, detail::TunableBase& tunable) {
  GetTable().Publish(name, tunable);
}

void Tunables::Publish(std::string_view name, ComplexTunable& tunable) {
  GetTable().Publish(name, tunable);
}

void Tunables::Publish(std::string_view name, ComplexTunable* tunable,
                       std::unique_ptr<detail::TunableMemberBase> member) {
  GetTable().Publish(name, tunable, std::move(member));
}

void Tunables::Remove(std::string_view name) {
  GetTable().Remove(name);
}
