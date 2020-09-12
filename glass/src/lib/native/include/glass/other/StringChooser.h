/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
