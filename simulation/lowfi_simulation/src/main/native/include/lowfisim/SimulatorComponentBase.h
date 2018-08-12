/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*
 * SimulatorComponentBase.h
 *
 *  Created on: Aug 12, 2018
 *      Author: PJ
 */

#ifndef WPILIB_SIMULATION_LOWFI_SIMULATION_SRC_MAIN_NATIVE_INCLUDE_LOWFISIM_SIMULATORCOMPONENTBASE_H_
#define WPILIB_SIMULATION_LOWFI_SIMULATION_SRC_MAIN_NATIVE_INCLUDE_LOWFISIM_SIMULATORCOMPONENTBASE_H_

#include <string>

#include "lowfisim/SimulatorComponent.h"

namespace frc {
namespace sim {
namespace lowfi {

class SimulatorComponentBase : public virtual SimulatorComponent {
 public:
  SimulatorComponentBase();
  virtual ~SimulatorComponentBase();

  const std::string& GetDisplayName() override;

  void SetDisplayName(const std::string& displayName) override;

 private:
  std::string m_name;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc

#endif  // WPILIB_SIMULATION_LOWFI_SIMULATION_SRC_MAIN_NATIVE_INCLUDE_LOWFISIM_SIMULATORCOMPONENTBASE_H_
