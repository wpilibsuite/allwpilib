// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/function_ref.h>

#include "glass/Model.h"

namespace glass {

class DataSource;

class RelayModel : public Model {
 public:
  virtual DataSource* GetForwardData() = 0;
  virtual DataSource* GetReverseData() = 0;

  virtual void SetForward(bool val) = 0;
  virtual void SetReverse(bool val) = 0;
};

class RelaysModel : public Model {
 public:
  virtual void ForEachRelay(
      wpi::function_ref<void(RelayModel& model, int index)> func) = 0;
};

void DisplayRelay(RelayModel* model, int index, bool outputsEnabled);
void DisplayRelays(RelaysModel* model, bool outputsEnabled,
                   std::string_view noneMsg = "No relays");

}  // namespace glass
