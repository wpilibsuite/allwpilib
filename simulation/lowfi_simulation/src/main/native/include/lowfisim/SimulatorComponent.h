/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPILIB_SIMULATION_LOWFI_SIMULATION_SRC_MAIN_NATIVE_INCLUDE_LOWFISIM_SIMULATORCOMPONENT_H_
#define WPILIB_SIMULATION_LOWFI_SIMULATION_SRC_MAIN_NATIVE_INCLUDE_LOWFISIM_SIMULATORCOMPONENT_H_

#include <string>

namespace frc {
namespace sim {
namespace lowfi {

class SimulatorComponent {
 public:
  virtual ~SimulatorComponent() {}

  virtual bool IsWrapperInitialized() = 0;

  virtual const std::string& GetDisplayName() = 0;

  virtual void SetDisplayName(const std::string& displayName) = 0;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc

#endif  // WPILIB_SIMULATION_LOWFI_SIMULATION_SRC_MAIN_NATIVE_INCLUDE_LOWFISIM_SIMULATORCOMPONENT_H_
