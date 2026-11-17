// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunables/Tunables.hpp"

#include <memory>
#include <utility>

#include "wpi/tunables/TunableTable.hpp"

namespace wpi::tunables {

TunableTable GetTable() {
  return TunableTable{"/"};
}

TunableTable GetTable(std::string_view name) {
  return GetTable().GetTable(name);
}

bool Publish(std::string_view name, detail::TunableBase& tunable) {
  return GetTable().Publish(name, tunable);
}

bool Publish(std::string_view name, ComplexTunable& tunable) {
  return GetTable().Publish(name, tunable);
}

bool Publish(std::string_view name, ComplexTunable* tunable,
             std::unique_ptr<detail::TunableMemberBase> member) {
  return GetTable().Publish(name, tunable, std::move(member));
}

void Remove(std::string_view name) {
  GetTable().Remove(name);
}

}  // namespace wpi::tunables
