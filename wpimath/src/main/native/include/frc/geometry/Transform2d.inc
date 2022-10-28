// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

namespace frc {

constexpr Transform2d::Transform2d(Translation2d translation,
                                   Rotation2d rotation)
    : m_translation(std::move(translation)), m_rotation(std::move(rotation)) {}

}  // namespace frc
