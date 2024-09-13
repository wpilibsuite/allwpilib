// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>

namespace frc {
template <typename T>
concept WheelPositions =
    std::copy_constructible<T> && requires(T a, T b, double t) {
      { a.Interpolate(b, t) } -> std::convertible_to<T>;
    };
}  // namespace frc
