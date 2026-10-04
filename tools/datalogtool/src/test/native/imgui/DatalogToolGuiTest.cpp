// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string_view>

#include <imgui_test_engine/imgui_te_context.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
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

  test = IM_REGISTER_TEST(engine, "datalogtool", "timestamp_fuzziness");
  test->TestFunc = [](ImGuiTestContext* ctx) {
    ctx->SetRef("Output");
    auto& storage = wpi::glass::GetStorageRoot().GetChild("output");
    ctx->ComboClick("Style/List");
    IM_CHECK(!ctx->ItemExists("Timestamp fuzziness (ms)"));
    ctx->ComboClick("Style/Table");
    IM_CHECK(ctx->ItemExists("Timestamp fuzziness (ms)"));
    ctx->ItemInputValue("Timestamp fuzziness (ms)", "0.125");
    IM_CHECK_EQ(storage.GetDouble("timestampFuzzinessMs"), 0.125);
    ctx->ComboClick("Style/List");
    ctx->ComboClick("Style/Table");
    IM_CHECK_EQ(storage.GetDouble("timestampFuzzinessMs"), 0.125);
    ctx->ItemInputValue("Timestamp fuzziness (ms)", "-1");
    IM_CHECK_EQ(storage.GetDouble("timestampFuzzinessMs"), 0.0);
    ctx->ComboClick("Style/List");
  };
}
}  // namespace

int main(int argc, char** argv) {
  return wpi::gui::test::RunTestApp("datalogtool", Application,
                                    RegisterDatalogToolGuiTests, argc, argv);
}
