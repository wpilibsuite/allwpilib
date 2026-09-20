// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

struct ImGuiTestEngine;

namespace wpi::gui::test {

using AppFunc = void (*)(std::string_view saveDir);
using TestRegistrar = void (*)(ImGuiTestEngine* engine);

void InstallTestEngineHooks();

int RunTestApp(std::string_view appName, AppFunc app,
               TestRegistrar registerTests, int argc, char** argv);

}  // namespace wpi::gui::test
