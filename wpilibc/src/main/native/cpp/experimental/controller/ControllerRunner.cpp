/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/controller/ControllerRunner.h"

using namespace frc::experimental;

ControllerRunner::ControllerRunner(Controller& controller,
                                   std::function<void(double)> controllerOutput)
    : m_controller(controller), m_controllerOutput(controllerOutput) {
  m_notifier.StartPeriodic(m_controller.GetPeriod());
}

ControllerRunner::ControllerRunner(Controller& controller,
                                   ControllerOutput& controllerOutput)
    : ControllerRunner(controller, [&](double output) {
        controllerOutput.SetOutput(output);
      }) {}

void ControllerRunner::Enable() {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_enabled = true;
}

void ControllerRunner::Disable() {
  // Ensures m_enabled modification and m_controllerOutput() call occur
  // atomically
  std::lock_guard<wpi::mutex> outputLock(m_outputMutex);
  {
    std::lock_guard<wpi::mutex> mainLock(m_thisMutex);
    m_enabled = false;
  }

  m_controllerOutput(0.0);
}

bool ControllerRunner::IsEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_enabled;
}

void ControllerRunner::Run() {
  // Ensures m_enabled check and m_controllerOutput() call occur atomically
  std::lock_guard<wpi::mutex> outputLock(m_outputMutex);
  std::unique_lock<wpi::mutex> mainLock(m_thisMutex);
  if (m_enabled) {
    // Don't block other ControllerRunner operations on output
    mainLock.unlock();

    m_controllerOutput(m_controller.Update());
  }
}
