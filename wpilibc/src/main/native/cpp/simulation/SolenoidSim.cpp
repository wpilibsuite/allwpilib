// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/SolenoidSim.h"

#include "frc/simulation/PCMSim.h"
#include "frc/SensorUtil.h"


using namespace frc;
using namespace frc::sim;

SolenoidSim::SolenoidSim(int channel) : m_channel{channel} {}

SolenoidSim::SolenoidSim(int module, int channel) : m_pcm{module}, m_channel{channel} {}

SolenoidSim::SolenoidSim(PCMSim& pcm, int channel) : m_pcm{pcm}, m_channel{channel} {}

std::unique_ptr<CallbackStore> SolenoidSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  return m_pcm.RegisterSolenoidInitializedCallback(m_channel, callback, initialNotify);
}

bool SolenoidSim::GetInitialized() const {
    return m_pcm.GetSolenoidInitialized(m_channel);
}

void SolenoidSim::SetInitialized(bool initialized) {
    m_pcm.SetSolenoidInitialized(m_channel, initialized);
}

std::unique_ptr<CallbackStore> SolenoidSim::RegisterOutputCallback(
    NotifyCallback callback, bool initialNotify) {
  return m_pcm.RegisterSolenoidOutputCallback(m_channel, callback, initialNotify);
}

bool SolenoidSim::GetOutput() const {
    return m_pcm.GetSolenoidOutput(m_channel);
}

void SolenoidSim::SetOutput(bool output) {
    m_pcm.SetSolenoidOutput(m_channel, output);
}