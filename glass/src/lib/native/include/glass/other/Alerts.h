// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include "glass/Model.h"

namespace glass {

class AlertsModel : public Model {
 public:
  virtual const std::vector<std::string>& GetInfos() = 0;
  virtual const std::vector<std::string>& GetWarnings() = 0;
  virtual const std::vector<std::string>& GetErrors() = 0;
};

void DisplayAlerts(AlertsModel* model);

}  // namespace glass
