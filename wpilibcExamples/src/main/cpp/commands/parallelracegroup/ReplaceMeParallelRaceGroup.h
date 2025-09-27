// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/command/CommandHelper.hpp>
#include <wpi/command/ParallelRaceGroup.hpp>

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/stable/docs/software/commandbased/convenience-features.html
class ReplaceMeParallelRaceGroup
    : public frc2::CommandHelper<frc2::ParallelRaceGroup,
                                 ReplaceMeParallelRaceGroup> {
 public:
  ReplaceMeParallelRaceGroup();
};
