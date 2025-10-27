// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/commands2/CommandHelper.hpp>
#include <wpi/commands2/ParallelCommandGroup.hpp>

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/stable/docs/software/commandbased/convenience-features.html
class ReplaceMeParallelCommandGroup
    : public frc2::CommandHelper<frc2::ParallelCommandGroup,
                                 ReplaceMeParallelCommandGroup> {
 public:
  ReplaceMeParallelCommandGroup();
};
