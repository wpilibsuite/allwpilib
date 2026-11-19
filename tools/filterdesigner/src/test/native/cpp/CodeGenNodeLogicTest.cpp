// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/CodeGenNodeLogic.hpp"

#include <string>

#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("CodeGenNodeLogicTest NullFilterReturnsEmpty", "[filterdesigner]") {
  CodeGenNodeLogic logic;
  CHECK(logic.Generate(nullptr).empty());
}

TEST_CASE("CodeGenNodeLogicTest EmptySectionsReturnsEmpty",
          "[filterdesigner]") {
  CodeGenNodeLogic logic;
  DesignedFilter f;
  f.sampleRate = 1000.0;
  CHECK(logic.Generate(&f).empty());
}

TEST_CASE("CodeGenNodeLogicTest DefaultsEmitCppSnippet", "[filterdesigner]") {
  CodeGenNodeLogic logic;
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  CHECK(out.find("wpi::math::BiquadFilter filter") != std::string::npos);
}

TEST_CASE("CodeGenNodeLogicTest JavaLangSelectsJavaEmitter",
          "[filterdesigner]") {
  CodeGenNodeLogic logic;
  logic.lang = Language::Java;
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  CHECK(out.find("BiquadFilter filter = new BiquadFilter") !=
        std::string::npos);
}

TEST_CASE("CodeGenNodeLogicTest PythonLangSelectsPythonEmitter",
          "[filterdesigner]") {
  CodeGenNodeLogic logic;
  logic.lang = Language::Python;
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  CHECK(out.find("from wpimath import BiquadFilter") != std::string::npos);
}

TEST_CASE("CodeGenNodeLogicTest VarNameAppearsInEmittedCode",
          "[filterdesigner]") {
  CodeGenNodeLogic logic;
  logic.varName = "shooterFilter";
  DesignedFilter f = MakeFilter();
  std::string out = logic.Generate(&f);
  CHECK(out.find("shooterFilter") != std::string::npos);
}

TEST_CASE("CodeGenNodeLogicTest NormalizesVarNamePerLanguage",
          "[filterdesigner]") {
  DesignedFilter f = MakeFilter();
  CodeGenNodeLogic logic;
  logic.varName = "my filter";

  logic.lang = Language::Cpp;
  CHECK(logic.Generate(&f).find("myFilter") != std::string::npos);
  logic.lang = Language::Java;
  CHECK(logic.Generate(&f).find("myFilter") != std::string::npos);
  logic.lang = Language::Python;
  CHECK(logic.Generate(&f).find("my_filter") != std::string::npos);
}

TEST_CASE("CodeGenNodeLogicTest UnusableVarNameStillEmits",
          "[filterdesigner]") {
  DesignedFilter f = MakeFilter();
  for (const char* bad : {"", "!!!", "1filter"}) {
    CodeGenNodeLogic logic;
    logic.varName = bad;
    UNSCOPED_INFO("varName = '" << bad << "'");
    CHECK_FALSE(logic.Generate(&f).empty());
  }
}

}  // namespace
