// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DoubleSolenoidSim.h"

#include "frc/SensorUtil.h"
#include "frc/simulation/PCMSim.h"

using namespace frc;
using namespace frc::sim;

DoubleSolenoidSim::DoubleSolenoidSim(int fwd, int rev)
    : m_fwd{fwd}, m_rev{rev} {}

DoubleSolenoidSim::DoubleSolenoidSim(int module, int fwd, int rev)
    : m_pcm{module}, m_fwd{fwd}, m_rev{rev} {}

DoubleSolenoidSim::DoubleSolenoidSim(PCMSim& pcm, int fwd, int rev)
    : m_pcm{pcm}, m_fwd{fwd}, m_rev{rev} {}

DoubleSolenoidSim::DoubleSolenoidSim(DoubleSolenoid&solenoid)
    : m_pcm{solenoid.GetModuleNumber()}, m_fwd{solenoid.GetFwdChannel()},
      m_rev{solenoid.GetRevChannel()} {}

std::unique_ptr<CallbackStore>
DoubleSolenoidSim::RegisterFwdInitializedCallback(NotifyCallback callback,
                                                  bool initialNotify) {
  return m_pcm.RegisterSolenoidInitializedCallback(m_fwd, callback,
                                                   initialNotify);
}

bool DoubleSolenoidSim::GetFwdInitialized() const {
  return m_pcm.GetSolenoidInitialized(m_fwd);
}

void DoubleSolenoidSim::SetFwdInitialized(bool initialized) {
  m_pcm.SetSolenoidInitialized(m_fwd, initialized);
}

std::unique_ptr<CallbackStore>
DoubleSolenoidSim::RegisterRevInitializedCallback(NotifyCallback callback,
                                                  bool initialNotify) {
  return m_pcm.RegisterSolenoidInitializedCallback(m_rev, callback,
                                                   initialNotify);
}

bool DoubleSolenoidSim::GetRevInitialized() const {
  return m_pcm.GetSolenoidInitialized(m_rev);
}

void DoubleSolenoidSim::SetRevInitialized(bool initialized) {
  m_pcm.SetSolenoidInitialized(m_rev, initialized);
}

void DoubleSolenoidSim::Set(DoubleSolenoid::Value value) {
  bool forward = false;
  bool reverse = false;

  switch (value) {
    case DoubleSolenoid::Value::kOff:
      forward = false;
      reverse = false;
      break;
    case DoubleSolenoid::Value::kForward:
      forward = true;
      reverse = false;
      break;
    case DoubleSolenoid::Value::kReverse:
      forward = false;
      reverse = true;
      break;
  }

  m_pcm.SetSolenoidOutput(m_fwd, forward);
  m_pcm.SetSolenoidOutput(m_rev, reverse);
}

DoubleSolenoid::Value DoubleSolenoidSim::Get() const {
  bool valueForward = m_pcm.GetSolenoidOutput(m_fwd);
  bool valueReverse = m_pcm.GetSolenoidOutput(m_rev);

  if (valueForward) {
    return DoubleSolenoid::Value::kForward;
  } else if (valueReverse) {
    return DoubleSolenoid::Value::kReverse;
  } else {
    return DoubleSolenoid::Value::kOff;
  }
}
