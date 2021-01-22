// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/StringRef.h>

#include "glass/Model.h"

namespace glass {

class StringChooserModel : public Model {
 public:
  virtual const std::string& GetDefault() = 0;
  virtual const std::string& GetSelected() = 0;
  virtual const std::string& GetActive() = 0;
  virtual const std::vector<std::string>& GetOptions() = 0;

  virtual void SetDefault(wpi::StringRef val) = 0;
  virtual void SetSelected(wpi::StringRef val) = 0;
  virtual void SetActive(wpi::StringRef val) = 0;
  virtual void SetOptions(wpi::ArrayRef<std::string> val) = 0;
};

void DisplayStringChooser(StringChooserModel* model);

}  // namespace glass
