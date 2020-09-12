/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  virtual void SetUserButton(bool val) = 0;
  virtual void SetVInVoltage(double val) = 0;
  virtual void SetVInCurrent(double val) = 0;
};

void DisplayRoboRio(RoboRioModel* model);

}  // namespace glass
