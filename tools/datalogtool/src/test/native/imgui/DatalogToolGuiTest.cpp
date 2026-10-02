// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <system_error>

#include <imgui_test_engine/imgui_te_context.h>

#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/gui/test/GuiTestEngineRunner.hpp"

void Application(std::string_view saveDir,
                 std::span<const std::string_view> filenames);

namespace {
void RegisterDatalogToolGuiTests(ImGuiTestEngine* engine) {
  ImGuiTest* test = IM_REGISTER_TEST(engine, "datalogtool", "default_windows");
  test->TestFunc = [](ImGuiTestContext* ctx) {
    ctx->Yield(3);
    IM_CHECK(ctx->WindowInfo("Input Files").ID != 0);
    IM_CHECK(ctx->WindowInfo("Entries").ID != 0);
    IM_CHECK(ctx->WindowInfo("Output").ID != 0);
  };

  test = IM_REGISTER_TEST(engine, "datalogtool", "startup_files");
  test->TestFunc = [](ImGuiTestContext* ctx) {
    ctx->Yield(3);
    ctx->SetRef("Input Files/Input Files");
    IM_CHECK(ctx->ItemExists("startup first/X"));
    IM_CHECK(ctx->ItemExists("startup second/X"));
    IM_CHECK(ctx->ItemExists("startup missing/X"));

    // The repeated startup filename should only create one row.
    ctx->ItemClick("startup first/X");
    ctx->Yield(3);
    IM_CHECK(!ctx->ItemExists("startup first/X"));
    IM_CHECK(ctx->ItemExists("startup second/X"));
  };
}

void RunApplicationWithFiles(std::string_view saveDir) {
  auto first =
      (std::filesystem::path{saveDir} / "startup first.wpilog").string();
  auto second =
      (std::filesystem::path{saveDir} / "startup second.wpilog").string();
  auto missing =
      (std::filesystem::path{saveDir} / "startup missing.wpilog").string();
  for (const auto& filename : {first, second}) {
    std::error_code ec;
    wpi::log::DataLogWriter writer{filename, ec};
    if (ec) {
      throw std::system_error{ec};
    }
    writer.Flush();
  }

  std::array<std::string_view, 4> filenames{first, second, missing, first};
  Application(saveDir, filenames);
}
}  // namespace

int main(int argc, char** argv) {
  return wpi::gui::test::RunTestApp("datalogtool", RunApplicationWithFiles,
                                    RegisterDatalogToolGuiTests, argc, argv);
}
