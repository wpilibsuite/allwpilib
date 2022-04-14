// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "glass/Model.h"

struct ImVec2;

namespace glass {
class DataSource;
class DriveModel : public Model {
 public:
  struct WheelInfo {
    std::string name;
    DataSource* percent;
    std::function<void(double)> setter;

    WheelInfo(std::string_view name, DataSource* percent,
              std::function<void(double)> setter)
        : name(name), percent(percent), setter(std::move(setter)) {}
  };

  virtual const char* GetName() const = 0;
  virtual const std::vector<WheelInfo>& GetWheels() const = 0;

  virtual ImVec2 GetSpeedVector() const = 0;

  // Clamped between -1 and 1 with -1 being full CCW.
  virtual double GetRotation() const = 0;
};
void DisplayDrive(DriveModel* m);
}  // namespace glass
