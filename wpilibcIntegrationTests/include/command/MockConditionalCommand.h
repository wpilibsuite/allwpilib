/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/ConditionalCommand.h>

#include "command/MockCommand.h"

namespace frc {

class MockConditionalCommand : public ConditionalCommand {
 public:
  MockConditionalCommand(MockCommand* onTrue, MockCommand* onFalse);
  void SetCondition(bool condition);

 protected:
  bool Condition() override;

 private:
  bool m_condition = false;
};

}  // namespace frc
