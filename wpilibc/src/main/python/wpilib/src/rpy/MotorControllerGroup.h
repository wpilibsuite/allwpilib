// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>
#include <memory>


#include "wpi/hardware/motor/MotorController.hpp"
#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {

class PyMotorControllerGroup : public wpi::util::Sendable,
                             public MotorController,
                             public wpi::util::SendableHelper<PyMotorControllerGroup> {
 public:
  PyMotorControllerGroup(std::vector<std::shared_ptr<wpi::MotorController>> &&args) :
    m_motorControllers(args) {}
  ~PyMotorControllerGroup() override = default;

  PyMotorControllerGroup(PyMotorControllerGroup&&) = default;
  PyMotorControllerGroup& operator=(PyMotorControllerGroup&&) = default;

  void Set(double speed) override;
  void SetVoltage(wpi::units::volt_t output) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  void Initialize();

  bool m_isInverted = false;
  std::vector<std::shared_ptr<wpi::MotorController>> m_motorControllers;
};

}  // namespace rpy
