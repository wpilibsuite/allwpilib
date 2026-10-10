// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>
#include <string_view>

#include <imgui_test_engine/imgui_te_context.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/glass/networktables/NTField2D.hpp"
#include "wpi/gui/test/GuiTestEngineRunner.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StructArrayTopic.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/json.hpp"

void Application(std::string_view saveDir);

namespace {
struct FieldImageTestState {
  wpi::glass::NTField2DModel model{"/FieldImageTest"};
  std::unique_ptr<wpi::glass::Storage> storage;
  fs::path project;
};

void RegisterGlassGuiTests(ImGuiTestEngine* engine) {
  ImGuiTest* test = IM_REGISTER_TEST(engine, "glass", "default_windows");
  test->TestFunc = [](ImGuiTestContext* ctx) {
    ctx->Yield(3);
    IM_CHECK(ctx->WindowInfo("NetworkTables").ID != 0);
    IM_CHECK(ctx->WindowInfo("NetworkTables Settings").ID != 0);
  };

  test = IM_REGISTER_TEST(engine, "glass", "field_images_move_with_project");
  test->SetVarsDataType<FieldImageTestState>();
  test->GuiFunc = [](ImGuiTestContext* ctx) {
    auto& vars = ctx->GetVars<FieldImageTestState>();
    if (!vars.storage) {
      return;
    }
    ImGui::Begin("Field image test");
    wpi::glass::PushStorageStack(*vars.storage);
    wpi::glass::DisplayField2D(&vars.model, {200, 100});
    wpi::glass::PopStorageStack();
    ImGui::End();
  };
  test->TestFunc = [](ImGuiTestContext* ctx) {
    auto& vars = ctx->GetVars<FieldImageTestState>();
    auto base = fs::path{wpi::glass::GetStorageDir()};
    vars.project = base / "original";
    fs::create_directories(vars.project / "images");
    auto image = vars.project / "images" / "test.ppm";
    {
      fs::ofstream output{image, std::ios::binary};
      output << "P6\n1 1\n255\n";
      output.write("\xff\x00\x00", 3);
      IM_CHECK(output.good());
    }
    auto publisher =
        wpi::nt::NetworkTableInstance::GetDefault()
            .GetStructArrayTopic<wpi::math::Pose2d>("/FieldImageTest/Robot")
            .Publish();
    vars.model.AddFieldObject("Robot");
    wpi::util::json json = wpi::util::json::object();
    json["builtin"] = "";
    json["image"] = image.string();
    json["Robot"] = wpi::util::json::object();
    json["Robot"]["image"] = image.string();
    vars.storage = std::make_unique<wpi::glass::Storage>();
    IM_CHECK(vars.storage->FromJson(
        json, (vars.project / "simgui.json").string().c_str()));
    ctx->Yield(3);

    json = vars.storage->ToJson(vars.project.string());
    IM_CHECK(json["image"].get_string() == "images/test.ppm");
    IM_CHECK(json["Robot"]["image"].get_string() == "images/test.ppm");

    auto moved = base / "moved";
    fs::rename(vars.project, moved);
    vars.project = moved;
    vars.storage = std::make_unique<wpi::glass::Storage>();
    IM_CHECK(
        vars.storage->FromJson(json, (moved / "simgui.json").string().c_str()));
    ctx->Yield(3);

    // Failed texture loads clear the saved filename, so these also verify
    // that both images were actually loaded from the relocated project.
    image = moved / "images" / "test.ppm";
    IM_CHECK(vars.storage->ReadString("image") == image.string());
    IM_CHECK(vars.storage->GetChild("Robot").ReadString("image") ==
             image.string());
    auto global = vars.storage->ToJson((base / "global").string());
    IM_CHECK(global["image"].get_string() == image.string());
    IM_CHECK(global["Robot"]["image"].get_string() == image.string());
    vars.storage.reset();
  };
}
}  // namespace

int main(int argc, char** argv) {
  return wpi::gui::test::RunTestApp("glass", Application, RegisterGlassGuiTests,
                                    argc, argv);
}
