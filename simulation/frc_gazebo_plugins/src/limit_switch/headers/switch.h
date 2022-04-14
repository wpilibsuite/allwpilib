// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

class Switch {
 public:
  virtual ~Switch() = default;

  /// \brief Returns true when the switch is triggered.
  virtual bool Get() = 0;
};
