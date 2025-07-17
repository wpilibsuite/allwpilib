// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>

#include "frc/EigenCore.h"

namespace frc {

template <typename T, int States>
concept SigmaPoints =
    requires(T t, Vectord<States> x, Matrixd<States, States> S, int i) {
      { T::NumSigmas } -> std::convertible_to<const int>;
      {
        t.SquareRootSigmaPoints(x, S)
      } -> std::same_as<Matrixd<States, T::NumSigmas>>;
      { t.Wm() } -> std::convertible_to<Vectord<T::NumSigmas>>;
      { t.Wm(i) } -> std::same_as<double>;
      { t.Wc() } -> std::convertible_to<Vectord<T::NumSigmas>>;
      { t.Wc(i) } -> std::same_as<double>;
    } && std::default_initializable<T>;

}  // namespace frc
