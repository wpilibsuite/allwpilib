// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsModuleType.h"
#include "frc/simulation/PneumaticsBaseSim.h"

namespace frc::sim {

class DoubleSolenoidSim {
 public:
  DoubleSolenoidSim(std::shared_ptr<PneumaticsBaseSim> moduleSim, int fwd,
                    int rev);
  DoubleSolenoidSim(int module, PneumaticsModuleType type, int fwd, int rev);
  DoubleSolenoidSim(PneumaticsModuleType type, int fwd, int rev);

  DoubleSolenoid::Value Get() const;
  void Set(DoubleSolenoid::Value output);

  /**
   * Returns true if the double solenoid is in a forward state.
   * @return true if the double solenoid is in a forward state.   
   */
  virtual bool IsForward() const;

  /**
   * Returns true if the double solenoid is in a reverse state.
   * @return true if the double solenoid is in a reverse state.   
   */
  virtual bool IsReverse() const;

  /**
   * Returns true if the double solenoid is in an off state.
   * @return true if the double solenoid is in an off state.   
   */
  virtual bool IsOff() const; 

  /**
   * Sets the double solenoid to a forward state
   */
  virtual void Forward();     

  /**
   * Sets the double solenoid to a reverse state
   */
  virtual void Reverse();  

  /**
   * Sets the double solenoid to an off state
   */
  virtual void Off();  

  std::shared_ptr<PneumaticsBaseSim> GetModuleSim() const;

 private:
  std::shared_ptr<PneumaticsBaseSim> m_module;
  int m_fwd;
  int m_rev;
};

}  // namespace frc::sim
