// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/glass/Model.hpp"

namespace glass {
class DoubleSource;
class ProfiledPIDControllerModel : public Model {
 public:
  virtual const char* GetName() const = 0;

  virtual DoubleSource* GetPData() = 0;
  virtual DoubleSource* GetIData() = 0;
  virtual DoubleSource* GetDData() = 0;
  virtual DoubleSource* GetIZoneData() = 0;
  virtual DoubleSource* GetMaxVelocityData() = 0;
  virtual DoubleSource* GetMaxAccelerationData() = 0;
  virtual DoubleSource* GetGoalData() = 0;

  virtual void SetP(double value) = 0;
  virtual void SetI(double value) = 0;
  virtual void SetD(double value) = 0;
  virtual void SetIZone(double value) = 0;
  virtual void SetMaxVelocity(double value) = 0;
  virtual void SetMaxAcceleration(double value) = 0;
  virtual void SetGoal(double value) = 0;
};
void DisplayProfiledPIDController(ProfiledPIDControllerModel* m);
}  // namespace glass
