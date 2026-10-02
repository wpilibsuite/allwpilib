// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/other/ProfiledPIDController.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

namespace wpi::glass {
class NTProfiledPIDControllerModel : public ProfiledPIDControllerModel {
 public:
  static constexpr const char* TYPE = "ProfiledPIDController";

  explicit NTProfiledPIDControllerModel(std::string_view path);
  NTProfiledPIDControllerModel(wpi::nt::NetworkTableInstance inst,
                               std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }

  DoubleSource* GetPData() override { return &m_pData; }
  DoubleSource* GetIData() override { return &m_iData; }
  DoubleSource* GetDData() override { return &m_dData; }
  DoubleSource* GetIZoneData() override { return &m_iZoneData; }
  DoubleSource* GetMaxVelocityData() override {
    return m_constraints.GetTopic().Exists() ? &m_maxVelocityData : nullptr;
  }
  DoubleSource* GetMaxAccelerationData() override {
    return m_constraints.GetTopic().Exists() ? &m_maxAccelerationData : nullptr;
  }
  DoubleSource* GetGoalData() override {
    return m_goal.Exists() ? &m_goalData : nullptr;
  }

  void SetP(double value) override;
  void SetI(double value) override;
  void SetD(double value) override;
  void SetIZone(double value) override;
  void SetMaxVelocity(double value) override;
  void SetMaxAcceleration(double value) override;
  void SetGoal(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override;

 private:
  void SetConstraints(double maxVelocity, double maxAcceleration);

  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::DoubleEntry m_p;
  wpi::nt::DoubleEntry m_i;
  wpi::nt::DoubleEntry m_d;
  wpi::nt::DoubleEntry m_iZone;
  wpi::nt::GenericSubscriber m_constraints;
  wpi::nt::GenericPublisher m_constraintsPublisher;
  wpi::nt::DoubleEntry m_goal;

  DoubleSource m_pData;
  DoubleSource m_iData;
  DoubleSource m_dData;
  DoubleSource m_iZoneData;
  DoubleSource m_maxVelocityData;
  DoubleSource m_maxAccelerationData;
  DoubleSource m_goalData;

  std::string m_nameValue;
  std::string m_constraintsTypeString;
};
}  // namespace wpi::glass
