// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/CodeGenNodeLogic.hpp"

#include <string>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"

namespace {

using wpi::filterdesigner::CodeGenNodeLogic;
using wpi::filterdesigner::DesignedFilter;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::Section;

DesignedFilter MakeFilter() {
  DesignedFilter f;
  f.sampleRate = 1000.0;
  f.sections.push_back(Section{1.0, 0.0, 0.0, -0.5, 0.25});
  return f;
}

TEST(CodeGenNodeLogicTest, NullFilterReturnsEmpty) {
  CodeGenNodeLogic logic;
  EXPECT_TRUE(logic.Generate(nullptr).empty());
}

TEST(CodeGenNodeLogicTest, EmptySectionsReturnsEmpty) {
  CodeGenNodeLogic logic;
  DesignedFilter f;
  f.sampleRate = 1000.0;
  EXPECT_TRUE(logic.Generate(&f).empty());
}

TEST(CodeGenNodeLogicTest, DefaultsEmitCppSnippet) {
  CodeGenNodeLogic logic;
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  EXPECT_NE(out.find("wpi::math::BiquadFilter filter"), std::string::npos);
}

TEST(CodeGenNodeLogicTest, JavaLangSelectsJavaEmitter) {
  CodeGenNodeLogic logic;
  logic.lang = Language::Java;
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  EXPECT_NE(out.find("BiquadFilter filter = new BiquadFilter"),
            std::string::npos);
}

TEST(CodeGenNodeLogicTest, PythonLangSelectsPythonEmitter) {
  CodeGenNodeLogic logic;
  logic.lang = Language::Python;
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  EXPECT_NE(out.find("from wpimath.filter import BiquadFilter"),
            std::string::npos);
}

TEST(CodeGenNodeLogicTest, VarNameAppearsInEmittedCode) {
  CodeGenNodeLogic logic;
  logic.varName = "shooterFilter";
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  EXPECT_NE(out.find("shooterFilter"), std::string::npos);
}

}  // namespace
