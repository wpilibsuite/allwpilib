/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/STLExtras.h>
#include <wpi/StringRef.h>

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
                   wpi::StringRef noneMsg = "No relays");

}  // namespace glass
