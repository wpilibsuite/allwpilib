// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ExportNodeLogic.hpp"

#include <filesystem>
#include <string>

#include <gtest/gtest.h>

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

TEST(ExportNodeLogicTest, NullFilterFailsWithMessage) {
  ExportNodeLogic logic;
  logic.projectRoot =
      (std::filesystem::temp_directory_path() / "fd_export_node_null").string();
  EXPECT_FALSE(logic.Export(nullptr));
  EXPECT_FALSE(logic.lastOk);
  EXPECT_NE(logic.lastMessage.find("No filter"), std::string::npos);
}

TEST(ExportNodeLogicTest, EmptySectionsFailsWithMessage) {
  ExportNodeLogic logic;
  DesignedFilter f;
  f.sampleRate = 1000.0;
  EXPECT_FALSE(logic.Export(&f));
  EXPECT_NE(logic.lastMessage.find("No filter"), std::string::npos);
}

TEST(ExportNodeLogicTest, InvalidClassNameFails) {
  ExportNodeLogic logic;
  logic.className = "9bad";
  logic.projectRoot =
      (std::filesystem::temp_directory_path() / "fd_export_node_invalid")
          .string();
  DesignedFilter f = MakeFilter();
  EXPECT_FALSE(logic.Export(&f));
  EXPECT_FALSE(logic.lastOk);
  EXPECT_NE(logic.lastMessage.find("Invalid class name"), std::string::npos);
}

TEST(ExportNodeLogicTest, EmptyRootFails) {
  ExportNodeLogic logic;
  // projectRoot left empty.
  DesignedFilter f = MakeFilter();
  EXPECT_FALSE(logic.Export(&f));
  EXPECT_NE(logic.lastMessage.find("Project root is empty"), std::string::npos);
}

TEST(ExportNodeLogicTest, SuccessfulExportWritesFile) {
  auto root = std::filesystem::temp_directory_path() / "fd_export_node_ok";
  std::filesystem::remove_all(root);

  ExportNodeLogic logic;
  logic.lang = Language::Cpp;
  logic.className = "ShooterFilter";
  logic.projectRoot = root.string();

  DesignedFilter f = MakeFilter();
  ASSERT_TRUE(logic.Export(&f));
  EXPECT_TRUE(logic.lastOk);

  auto expected =
      root / "src" / "main" / "include" / "filters" / "ShooterFilter.h";
  EXPECT_TRUE(std::filesystem::exists(expected));

  std::filesystem::remove_all(root);
}

TEST(ExportNodeLogicTest, MessageMentionsSuccessPath) {
  auto root = std::filesystem::temp_directory_path() / "fd_export_node_msg";
  std::filesystem::remove_all(root);

  ExportNodeLogic logic;
  logic.className = "MsgFilter";
  logic.projectRoot = root.string();

  DesignedFilter f = MakeFilter();
  ASSERT_TRUE(logic.Export(&f));
  EXPECT_NE(logic.lastMessage.find("Wrote"), std::string::npos);
  EXPECT_NE(logic.lastMessage.find("MsgFilter"), std::string::npos);

  std::filesystem::remove_all(root);
}

}  // namespace
