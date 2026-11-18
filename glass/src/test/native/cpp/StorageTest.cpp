// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/Storage.hpp"

#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/fs.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::glass;

namespace {
fs::path TestDir() {
  return fs::temp_directory_path() / "glass-storage-test";
}
}  // namespace

TEST_CASE("Storage SavesProjectPathsRelativeToConfig", "[storage]") {
  auto project = TestDir() / "project";
  Storage storage;
  storage.GetPath("field") = (project / "field.png").string();
  storage.GetChild("Robot").GetPath("image") =
      (project / "images" / "robot.png").string();
  auto& objects = storage.GetChildArray("objects");
  objects.emplace_back(std::make_unique<Storage>());
  objects.back()->GetPath("image") =
      (project / "images" / "object.png").string();
  storage.GetString("ordinaryString") = (project / "text").string();

  auto json = storage.ToJson(project.string());
  CHECK(json["field"].get_string() == "field.png");
  CHECK(json["Robot"]["image"].get_string() == "images/robot.png");
  CHECK(json["objects"][0]["image"].get_string() == "images/object.png");
  CHECK(json["ordinaryString"].get_string() == (project / "text").string());
  CHECK(storage.GetPath("field") == (project / "field.png").string());
}

TEST_CASE("Storage SavesCurrentDirectoryPathsRelative", "[storage]") {
  Storage storage;
  storage.GetPath("image") = fs::absolute("images/robot.png").string();
  CHECK(storage.ToJson(".")["image"].get_string() == "images/robot.png");
}

TEST_CASE("Storage KeepsExternalPathsAbsolute", "[storage]") {
  auto project = TestDir() / "project";
  Storage storage;
  for (const auto& image :
       {TestDir() / "image.png", TestDir() / "project-other" / "image.png",
        project / "images" / ".." / ".." / "image.png"}) {
    storage.GetPath("image") = image.string();
    CHECK(storage.ToJson(project.string())["image"].get_string() ==
          image.string());
  }
#ifdef _WIN32
  storage.GetPath("image") = "Z:\\images\\robot.png";
  CHECK(storage.ToJson("C:\\project")["image"].get_string() ==
        "Z:\\images\\robot.png");
#endif
}

TEST_CASE("Storage GlobalConfigDoesNotUseLaunchDirectory", "[storage]") {
  auto global = TestDir() / "global-config";
  auto image = fs::absolute("images/robot.png").string();
  Storage storage;
  storage.GetPath("image") = image;
  auto json = storage.ToJson(global.string());
  CHECK(json["image"].get_string() == image);

  Storage loaded;
  REQUIRE(loaded.FromJson(json, (global / "glass.json").string().c_str()));
  CHECK(loaded.GetPath("image") == image);
}

TEST_CASE("Storage ResolvesPathsAfterProjectMove", "[storage]") {
  auto original = TestDir() / "original";
  auto moved = TestDir() / "moved";
  Storage storage;
  storage.GetPath("field") = (original / "field.png").string();
  storage.GetChild("Robot").GetPath("image") =
      (original / "images" / "robot.png").string();
  auto& objects = storage.GetChildArray("objects");
  objects.emplace_back(std::make_unique<Storage>());
  objects.back()->GetPath("image") = (original / "object.png").string();

  Storage loaded;
  REQUIRE(loaded.FromJson(storage.ToJson(original.string()),
                          (moved / "simgui.json").string().c_str()));
  CHECK(loaded.GetPath("field") == (moved / "field.png").string());
  CHECK(loaded.GetChild("Robot").GetPath("image") ==
        (moved / "images" / "robot.png").string());
  CHECK(loaded.GetChildArray("objects")[0]->GetPath("image") ==
        (moved / "object.png").string());
}

TEST_CASE("Storage SaveAsPreservesImageLocation", "[storage]") {
  auto project = TestDir() / "project";
  auto other = TestDir() / "other";
  Storage storage;
  wpi::util::json json = wpi::util::json::object();
  json["image"] = "images/robot.png";
  REQUIRE(storage.FromJson(json, (project / "simgui.json").string().c_str()));
  auto image = storage.GetPath("image");
  CHECK(image == (project / "images" / "robot.png").string());

  CHECK(storage.ToJson(other.string())["image"].get_string() == image);
  CHECK(storage.ToJson(TestDir().string())["image"].get_string() ==
        "project/images/robot.png");
  CHECK(storage.ToJson(project.string())["image"].get_string() ==
        "images/robot.png");
  CHECK(storage.ToJson()["image"].get_string() == image);
  CHECK(storage.GetPath("image") == image);
}

TEST_CASE("Storage LoadsLegacyAbsolutePaths", "[storage]") {
  auto project = TestDir() / "project";
  auto image = (project / "images" / "robot.png").string();
  Storage storage;
  wpi::util::json json = wpi::util::json::object();
  json["image"] = image;
  REQUIRE(storage.FromJson(json, (project / "simgui.json").string().c_str()));
  CHECK(storage.GetPath("image") == image);
  CHECK(storage.ToJson(project.string())["image"].get_string() ==
        "images/robot.png");
}

TEST_CASE("Storage ReloadsRegisteredPaths", "[storage]") {
  auto project = TestDir() / "project";
  Storage storage;
  auto& image = storage.GetPath("image");
  CHECK(image.empty());
  CHECK(storage.ToJson(project.string()).empty());

  wpi::util::json json = wpi::util::json::object();
  json["image"] = "images/robot.png";
  REQUIRE(storage.FromJson(json, (project / "simgui.json").string().c_str()));
  CHECK(image == (project / "images" / "robot.png").string());
  CHECK(storage.ToJson(project.string())["image"].get_string() ==
        "images/robot.png");
  storage.ClearValues();
  CHECK(image.empty());
  CHECK(storage.ToJson(project.string()).empty());
}
