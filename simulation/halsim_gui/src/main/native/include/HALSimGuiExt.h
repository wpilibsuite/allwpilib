// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

namespace halsimgui {

// These functions can be used to hook into the GUI, and can be accessed
// via HAL_RegisterExtensionListener

#define HALSIMGUI_EXT_ADDGUIINIT "halsimgui::AddGuiInit"
using AddGuiInitFn = void (*)(std::function<void()> initialize);

#define HALSIMGUI_EXT_ADDGUILATEEXECUTE "halsimgui::AddGuiLateExecute"
using AddGuiLateExecuteFn = void (*)(std::function<void()> execute);

#define HALSIMGUI_EXT_ADDGUIEARLYEXECUTE "halsimgui::AddGuiEarlyExecute"
using AddGuiEarlyExecuteFn = void (*)(std::function<void()> execute);

#define HALSIMGUI_EXT_GUIEXIT "halsimgui::GuiExit"
using GuiExitFn = void (*)();

}  // namespace halsimgui
