/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/buttons/Button.h"

namespace frc {

class InternalButton : public Button {
 public:
  InternalButton() = default;
  explicit InternalButton(bool inverted);
  virtual ~InternalButton() = default;

  InternalButton(InternalButton&&) = default;
  InternalButton& operator=(InternalButton&&) = default;

  void SetInverted(bool inverted);
  void SetPressed(bool pressed);

  virtual bool Get();

 private:
  bool m_pressed = false;
  bool m_inverted = false;
};

}  // namespace frc
