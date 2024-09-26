// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <glass/Model.h>
#include <glass/Provider.h>

namespace halsimgui {

class HALProvider : private glass::Provider<> {
 public:
  explicit HALProvider(glass::Storage& storage);

  using Provider::GlobalInit;
  using Provider::Register;
  using Provider::RegisterModel;
  using Provider::RegisterView;
  using Provider::ShowDefault;

  void DisplayMenu() override;

  glass::Model* GetModel(std::string_view name);

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
  void Show(ViewEntry* entry, glass::Window* window) override;
};

}  // namespace halsimgui
