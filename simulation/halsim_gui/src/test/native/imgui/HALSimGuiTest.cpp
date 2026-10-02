// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string_view>

#include <imgui_test_engine/imgui_te_context.h>

#include "wpi/gui/test/GuiTestEngineRunner.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hal/Main.h"

extern "C" int HALSIM_InitExtension(void);

namespace halsimgui {
void SetTestStorageDir(std::string_view saveDir);
}  // namespace halsimgui

namespace {
void RunHALSimGui(std::string_view saveDir) {
  HAL_Initialize();
  halsimgui::SetTestStorageDir(saveDir);
  HALSIM_InitExtension();
  if (HAL_HasMain()) {
    HAL_RunMain();
  }
}

void RegisterHALSimGuiTests(ImGuiTestEngine* engine) {
  ImGuiTest* test = IM_REGISTER_TEST(engine, "halsim_gui", "default_windows");
  test->TestFunc = [](ImGuiTestContext* ctx) {
    ctx->Yield(3);
    IM_CHECK(ctx->WindowInfo("DS").ID != 0);
    IM_CHECK(ctx->WindowInfo("Display").ID != 0);
    IM_CHECK(ctx->WindowInfo("System Joysticks").ID != 0);
    IM_CHECK(ctx->WindowInfo("Joysticks").ID != 0);
    IM_CHECK(ctx->WindowInfo("NetworkTables").ID != 0);
  };
}
}  // namespace

int main(int argc, char** argv) {
  return wpi::gui::test::RunTestApp("halsim_gui", RunHALSimGui,
                                    RegisterHALSimGuiTests, argc, argv);
}
