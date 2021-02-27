// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_OPTIONAL_H_
#define WPIUTIL_WPI_OPTIONAL_H_

// clang-format off

#ifdef _MSC_VER
#pragma message("warning: wpi/optional.h is deprecated; include <optional> instead")
#else
#warning "wpi/optional.h is deprecated; include <optional> instead"
#endif

#include <optional>

namespace wpi {

template <typename T>
using optional [[deprecated("use std::optional")]] = std::optional<T>;

using nullopt_t [[deprecated("use std::nullopt_t")]] = std::nullopt_t;

[[deprecated("use std::nullopt")]] inline constexpr std::nullopt_t nullopt =
    std::nullopt;

}  // namespace wpi

// clang-format on

#endif  // WPIUTIL_WPI_OPTIONAL_H_
