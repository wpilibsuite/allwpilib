/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <glass/Model.h>
#include <glass/Provider.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <wpi/StringRef.h>

namespace halsimgui {

class HALProvider : public glass::Provider<> {
 public:
  explicit HALProvider(const wpi::Twine& iniName) : Provider{iniName} {}

  void DisplayMenu() override;

  glass::Model* GetModel(wpi::StringRef name);

  /**
   * Returns true if outputs are disabled.
   *
   * @return true if outputs are disabled, false otherwise.
   */
  static bool AreOutputsDisabled();

  /**
   * Returns true if outputs are enabled.
   *
   * @return true if outputs are enabled, false otherwise.
   */
  static bool AreOutputsEnabled() { return !AreOutputsDisabled(); }

 private:
  void Update() override;

  void Show(ViewEntry* entry, glass::Window* window) override;
};

}  // namespace halsimgui
