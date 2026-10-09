// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ExportNodeLogic.hpp"

#include <filesystem>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"

namespace {

using wpi::filterdesigner::DesignedFilter;
using wpi::filterdesigner::ExportNodeLogic;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::Section;

DesignedFilter MakeFilter() {
  DesignedFilter f;
  f.sampleRate = 1000.0;
  f.sections.push_back(Section{1.0, 0.0, 0.0, -0.5, 0.25});
  return f;
}

TEST_CASE("ExportNodeLogicTest NullFilterFailsWithMessage",
          "[filterdesigner]") {
  ExportNodeLogic logic;
  logic.projectRoot =
      (std::filesystem::temp_directory_path() / "fd_export_node_null").string();
  CHECK_FALSE(logic.Export(nullptr));
  CHECK_FALSE(logic.lastOk);
  CHECK(logic.lastMessage.find("No filter") != std::string::npos);
}

TEST_CASE("ExportNodeLogicTest EmptySectionsFailsWithMessage",
          "[filterdesigner]") {
  ExportNodeLogic logic;
  DesignedFilter f;
  f.sampleRate = 1000.0;
  CHECK_FALSE(logic.Export(&f));
  CHECK(logic.lastMessage.find("No filter") != std::string::npos);
}

TEST_CASE("ExportNodeLogicTest InvalidClassNameFails", "[filterdesigner]") {
  ExportNodeLogic logic;
  logic.className = "9bad";
  logic.projectRoot =
      (std::filesystem::temp_directory_path() / "fd_export_node_invalid")
          .string();
  DesignedFilter f = MakeFilter();
  CHECK(logic.Export(&f));
  CHECK(logic.lastOk);
  CHECK(logic.lastMessage.find("_9bad") != std::string::npos);
}

TEST_CASE("ExportNodeLogicTest EmptyRootFails", "[filterdesigner]") {
  ExportNodeLogic logic;
  // projectRoot left empty.
  DesignedFilter f = MakeFilter();
  CHECK_FALSE(logic.Export(&f));
  CHECK(logic.lastMessage.find("Project root is empty") != std::string::npos);
}

TEST_CASE("ExportNodeLogicTest SuccessfulExportWritesFile",
          "[filterdesigner]") {
  auto root = std::filesystem::temp_directory_path() / "fd_export_node_ok";
  std::filesystem::remove_all(root);

  ExportNodeLogic logic;
  logic.lang = Language::Cpp;
  logic.className = "ShooterFilter";
  logic.projectRoot = root.string();

  DesignedFilter f = MakeFilter();
  REQUIRE(logic.Export(&f));
  CHECK(logic.lastOk);

  auto expected =
      root / "src" / "main" / "include" / "filters" / "ShooterFilter.h";
  CHECK(std::filesystem::exists(expected));

  std::filesystem::remove_all(root);
}

TEST_CASE("ExportNodeLogicTest MessageMentionsSuccessPath",
          "[filterdesigner]") {
  auto root = std::filesystem::temp_directory_path() / "fd_export_node_msg";
  std::filesystem::remove_all(root);

  ExportNodeLogic logic;
  logic.className = "MsgFilter";
  logic.projectRoot = root.string();

  DesignedFilter f = MakeFilter();
  REQUIRE(logic.Export(&f));
  CHECK(logic.lastMessage.find("Wrote") != std::string::npos);
  CHECK(logic.lastMessage.find("MsgFilter") != std::string::npos);

  std::filesystem::remove_all(root);
}

}  // namespace
