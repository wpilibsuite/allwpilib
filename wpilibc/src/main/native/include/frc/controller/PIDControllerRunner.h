/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include <wpi/mutex.h>

#include "frc/Notifier.h"
#include "frc/controller/PIDController.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

class PIDControllerRunner : SendableBase {
 public:
  /**
   * Allocates a PIDControllerRunner.
   *
   * @param controller       The controller on which to call Update().
   * @param controllerOutput The function which updates the plant using the
   *                         controller output passed as the argument.
   */
  PIDControllerRunner(frc2::PIDController& controller,
                      std::function<double(void)> measurementSource,
                      std::function<void(double)> controllerOutput);

  ~PIDControllerRunner();

  PIDControllerRunner(PIDControllerRunner&& rhs);
  PIDControllerRunner& operator=(PIDControllerRunner&& rhs);

  /**
   * Begins running the controller.
   */
  void Enable();

  /**
   * Stops running the controller.
   *
   * This sets the output to zero before stopping.
   */
  void Disable();

  /**
   * Returns whether controller is running.
   */
  bool IsEnabled() const;

  void InitSendable(SendableBuilder& builder) override;

 protected:
  PIDControllerRunner(PIDControllerRunner&& rhs,
                      std::scoped_lock<wpi::mutex, wpi::mutex> lock);

 private:
  Notifier m_notifier{&PIDControllerRunner::Run, this};
  frc2::PIDController* m_controller;
  std::function<double(void)> m_measurementSource;
  std::function<void(double)> m_controllerOutput;
  bool m_enabled = false;

  mutable wpi::mutex m_thisMutex;

  // Ensures when Disable() is called, m_controllerOutput() won't run if
  // Controller::Update() is already running at that time.
  mutable wpi::mutex m_outputMutex;

  void Run();
};

}  // namespace frc
