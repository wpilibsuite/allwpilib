// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {
class DataSource;
class ProfiledPIDControllerModel : public Model {
 public:
  virtual const char* GetName() const = 0;

  virtual DataSource* GetPData() = 0;
  virtual DataSource* GetIData() = 0;
  virtual DataSource* GetDData() = 0;
  virtual DataSource* GetIZoneData() = 0;
  virtual DataSource* GetMaxVelocityData() = 0;
  virtual DataSource* GetMaxAccelerationData() = 0;
  virtual DataSource* GetGoalData() = 0;

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
