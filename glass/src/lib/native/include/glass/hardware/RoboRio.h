// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {

class DataSource;

class RoboRioRailModel : public Model {
 public:
  virtual DataSource* GetVoltageData() = 0;
  virtual DataSource* GetCurrentData() = 0;
  virtual DataSource* GetActiveData() = 0;
  virtual DataSource* GetFaultsData() = 0;

  virtual void SetVoltage(double val) = 0;
  virtual void SetCurrent(double val) = 0;
  virtual void SetActive(bool val) = 0;
  virtual void SetFaults(int val) = 0;
};

class RoboRioModel : public Model {
 public:
  virtual RoboRioRailModel* GetUser6VRail() = 0;
  virtual RoboRioRailModel* GetUser5VRail() = 0;
  virtual RoboRioRailModel* GetUser3V3Rail() = 0;

  virtual DataSource* GetUserButton() = 0;
  virtual DataSource* GetVInVoltageData() = 0;
  virtual DataSource* GetVInCurrentData() = 0;
  virtual DataSource* GetBrownoutVoltage() = 0;

  virtual void SetUserButton(bool val) = 0;
  virtual void SetVInVoltage(double val) = 0;
  virtual void SetVInCurrent(double val) = 0;
  virtual void SetBrownoutVoltage(double val) = 0;
};

void DisplayRoboRio(RoboRioModel* model);

}  // namespace glass
