// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

struct ImGuiContext;

namespace glass {
class Context;
}  // namespace glass

namespace wpi::gui {
struct Context;
}  // namespace wpi::gui

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

#define HALSIMGUI_EXT_GETIMGUICONTEXT "halsimgui::GetImguiContext"
using GetImguiContextFn = ImGuiContext* (*)();

#define HALSIMGUI_EXT_GETGUICONTEXT "halsimgui::GetGuiContext"
using GetGuiContextFn = wpi::gui::Context* (*)();

#define HALSIMGUI_EXT_GETGLASSCONTEXT "halsimgui::GetGlassContext"
using GetGlassContextFn = glass::Context* (*)();

}  // namespace halsimgui
