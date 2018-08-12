/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*
 * SimulatorComponentBase.cpp
 *
 *  Created on: Aug 12, 2018
 *      Author: PJ
 */

#include "lowfisim/SimulatorComponentBase.h"

namespace frc {
namespace sim {
namespace lowfi {
SimulatorComponentBase::SimulatorComponentBase() {
  // TODO Auto-generated constructor stub
}

SimulatorComponentBase::~SimulatorComponentBase() {
  // TODO Auto-generated destructor stub
}

const std::string& SimulatorComponentBase::GetDisplayName() { return m_name; }

void SimulatorComponentBase::SetDisplayName(const std::string& displayName) {
  m_name = displayName;
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
