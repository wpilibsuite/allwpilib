// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunables/ComplexTunable.hpp"

#include "wpi/tunables/TunableRegistry.hpp"

using namespace wpi;
using namespace wpi::tunables;

void ComplexTunable::anchor() {}

void ComplexTunable::PublishChildTunable(std::string_view name,
                                         detail::TunableBase& tunable) {
  TunableRegistry::PublishChild(*this, name, tunable);
}

void ComplexTunable::PublishChildTunable(std::string_view name,
                                         ComplexTunable& tunable) {
  TunableRegistry::PublishChild(*this, name, tunable);
}

void ComplexTunable::RemoveChildTunable(std::string_view name) {
  TunableRegistry::RemoveChild(*this, name);
}

void ComplexTunable::SetChildTunableChanged(std::string_view name) {
  TunableRegistry::SetChildChanged(*this, name);
}
