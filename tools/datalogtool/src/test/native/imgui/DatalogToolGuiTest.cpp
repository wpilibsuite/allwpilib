// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string_view>

#include <imgui_test_engine/imgui_te_context.h>

#include "wpi/gui/test/GuiTestEngineRunner.hpp"

void Application(std::string_view saveDir);

namespace {
void RegisterDatalogToolGuiTests(ImGuiTestEngine* engine) {
  ImGuiTest* test = IM_REGISTER_TEST(engine, "datalogtool", "default_windows");
  test->TestFunc = [](ImGuiTestContext* ctx) {
    ctx->Yield(3);
    IM_CHECK(ctx->WindowInfo("Input Files").ID != 0);
    IM_CHECK(ctx->WindowInfo("Entries").ID != 0);
    IM_CHECK(ctx->WindowInfo("Output").ID != 0);
  };
}
}  // namespace

int main(int argc, char** argv) {
  return wpi::gui::test::RunTestApp("datalogtool", Application,
                                    RegisterDatalogToolGuiTests, argc, argv);
}
