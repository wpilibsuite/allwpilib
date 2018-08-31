/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDControllerRunner.h"

#include "frc/controller/PIDController.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc::experimental;

PIDControllerRunner::PIDControllerRunner(
    PIDController& controller, std::function<double(void)> measurementSource,
    std::function<void(double)> controllerOutput)
    : m_controller(controller),
      m_measurementSource(measurementSource),
      m_controllerOutput(controllerOutput) {
  m_notifier.StartPeriodic(m_controller.GetPeriod());
}

PIDControllerRunner::~PIDControllerRunner() { Disable(); }

void PIDControllerRunner::Enable() {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_enabled = true;
}

void PIDControllerRunner::Disable() {
  // Ensures m_enabled modification and m_controllerOutput() call occur
  // atomically
  std::lock_guard<wpi::mutex> outputLock(m_outputMutex);
  {
    std::lock_guard<wpi::mutex> mainLock(m_thisMutex);
    m_enabled = false;
  }

  m_controllerOutput(0.0);
}

bool PIDControllerRunner::IsEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_enabled;
}

void PIDControllerRunner::Run() {
  // Ensures m_enabled check and m_controllerOutput() call occur atomically
  std::lock_guard<wpi::mutex> outputLock(m_outputMutex);
  std::unique_lock<wpi::mutex> mainLock(m_thisMutex);
  if (m_enabled) {
    // Don't block other PIDControllerRunner operations on output
    mainLock.unlock();

    m_controllerOutput(m_controller.Calculate(m_measurementSource()));
  }
}

void PIDControllerRunner::InitSendable(SendableBuilder& builder) {
  m_controller.InitSendable(builder);
  builder.SetSafeState([this]() { Disable(); });
  builder.AddBooleanProperty("enabled", [this]() { return IsEnabled(); },
                             [this](bool enabled) {
                               if (enabled) {
                                 Enable();
                               } else {
                                 Disable();
                               }
                             });
}
