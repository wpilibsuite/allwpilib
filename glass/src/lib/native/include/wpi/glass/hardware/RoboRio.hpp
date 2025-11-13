// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/glass/Model.hpp"

namespace wpi::glass {

class BooleanSource;
class DoubleSource;
class IntegerSource;

class RoboRioRailModel : public Model {
 public:
  virtual DoubleSource* GetVoltageData() = 0;
  virtual DoubleSource* GetCurrentData() = 0;
  virtual BooleanSource* GetActiveData() = 0;
  virtual IntegerSource* GetFaultsData() = 0;

  virtual void SetVoltage(double val) = 0;
  virtual void SetCurrent(double val) = 0;
  virtual void SetActive(bool val) = 0;
  virtual void SetFaults(int val) = 0;
};

class RoboRioModel : public Model {
 public:
  virtual RoboRioRailModel* GetUser3V3Rail() = 0;

  virtual DoubleSource* GetVInVoltageData() = 0;
  virtual DoubleSource* GetBrownoutVoltage() = 0;

  virtual void SetVInVoltage(double val) = 0;
  virtual void SetBrownoutVoltage(double val) = 0;
};

void DisplayRoboRio(RoboRioModel* model);

}  // namespace wpi::glass
