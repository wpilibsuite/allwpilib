// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <type_traits>

namespace wpi {

template <class Derived, class Base>
concept DecayedDerivedFrom =
    std::derived_from<std::decay_t<Derived>, std::decay_t<Base>> &&
    std::convertible_to<std::decay_t<Derived>*, std::decay_t<Base>*>;

}  // namespace wpi
