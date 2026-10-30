// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/codegen/CodeGen.hpp"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/filterdesigner/model/Stage.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace {

using wpi::filterdesigner::EmitCode;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::Section;
using wpi::filterdesigner::Sections;
using wpi::math::BiquadFilter;
using namespace wpi::units;

Sections SectionsOf(const BiquadFilter& f) {
  auto span = f.Sections();
  return Sections(span.begin(), span.end());
}

TEST_CASE("CodeGenTest EmptySectionsReturnsEmptyString", "[filterdesigner]") {
  CHECK(EmitCode(Sections{}, Language::Cpp) == "");
  CHECK(EmitCode(Sections{}, Language::Java) == "");
  CHECK(EmitCode(Sections{}, Language::Python) == "");
}

TEST_CASE("CodeGenTest CppEmitsBraceInitializedBiquadFilter",
          "[filterdesigner]") {
  Sections sos{{0.5, 0.25, 0.125, -0.75, 0.25}};
  std::string code = EmitCode(sos, Language::Cpp, "lowpass");
  CHECK(code.find("wpi::math::BiquadFilter lowpass{") != std::string::npos);
  CHECK(code.find("{0.5, 0.25, 0.125, -0.75, 0.25},") != std::string::npos);
  CHECK(code.back() == '\n');
}

TEST_CASE("CodeGenTest JavaEmitsVarargsBiquadFilterCtor", "[filterdesigner]") {
  Sections sos{{0.5, 0.25, 0.125, -0.75, 0.25}};
  std::string code = EmitCode(sos, Language::Java, "lowpass");
  CHECK(code.find("BiquadFilter lowpass = new BiquadFilter(") !=
        std::string::npos);
  CHECK(code.find("new BiquadFilter.Section(0.5, 0.25, 0.125, -0.75, 0.25)") !=
        std::string::npos);
}

TEST_CASE("CodeGenTest CppMultiSectionHasOnePerLineWithTrailingComma",
          "[filterdesigner]") {
  // Chosen so %.17g formats them identically on every platform.
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  std::string code = EmitCode(sos, Language::Cpp);
  CHECK(code.find("{1, 0, 0, -0.5, 0.25}") != std::string::npos);
  CHECK(code.find("{1, 2, 1, -0.125, 0.75}") != std::string::npos);
  CHECK(code.find("};") != std::string::npos);
}

TEST_CASE("CodeGenTest JavaMultiSectionInsertsCommaBetweenSectionsNotAfterLast",
          "[filterdesigner]") {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  std::string code = EmitCode(sos, Language::Java);
  // Trailing comma after last section would be a syntax error in Java.
  auto lastSection = code.rfind("new BiquadFilter.Section(");
  REQUIRE(lastSection != std::string::npos);
  auto closeParen = code.find(')', lastSection);
  REQUIRE(closeParen != std::string::npos);
  CHECK(code[closeParen + 1] == '\n');
}

TEST_CASE("CodeGenTest UsesHighPrecisionForScipyGoldenValues",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Notch(1000_Hz, 60_Hz, 10.0));
  std::string code = EmitCode(filter, Language::Cpp);
  // Enough precision to reproduce the scipy-matching coefficient.
  CHECK(code.find("0.9814970254751") != std::string::npos);
}

TEST_CASE("CodeGenTest PythonEmitsWpimathFilterBiquadFilter",
          "[filterdesigner]") {
  Sections sos{{0.5, 0.25, 0.125, -0.75, 0.25}};
  std::string code = EmitCode(sos, Language::Python, "lowpass");
  CHECK(code.find("from wpimath import BiquadFilter") != std::string::npos);
  CHECK(code.find("lowpass = BiquadFilter([") != std::string::npos);
  CHECK(code.find("BiquadFilter.Section(b0=0.5, b1=0.25, b2=0.125, a1=-0.75, "
                  "a2=0.25)") != std::string::npos);
  CHECK(code.find("])") != std::string::npos);
}

TEST_CASE("CodeGenTest DefaultVariableNameIsFilter", "[filterdesigner]") {
  Sections sos{{1.0, 0.0, 0.0, 0.0, 0.0}};
  CHECK(EmitCode(sos, Language::Cpp).find("BiquadFilter filter") !=
        std::string::npos);
  CHECK(EmitCode(sos, Language::Java).find("BiquadFilter filter ") !=
        std::string::npos);
}

TEST_CASE("CodeGenTest CppGoldenSnippet", "[filterdesigner]") {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  constexpr std::string_view kGolden =
      "wpi::math::BiquadFilter lowpass{\n"
      "    {1, 0, 0, -0.5, 0.25},\n"
      "    {1, 2, 1, -0.125, 0.75},\n"
      "};\n";
  CHECK(EmitCode(sos, Language::Cpp, "lowpass") == kGolden);
}

TEST_CASE("CodeGenTest JavaGoldenSnippet", "[filterdesigner]") {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  constexpr std::string_view kGolden =
      "BiquadFilter lowpass = new BiquadFilter(\n"
      "    new BiquadFilter.Section(1, 0, 0, -0.5, 0.25),\n"
      "    new BiquadFilter.Section(1, 2, 1, -0.125, 0.75)\n"
      ");\n";
  CHECK(EmitCode(sos, Language::Java, "lowpass") == kGolden);
}

TEST_CASE("CodeGenTest PythonGoldenSnippet", "[filterdesigner]") {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  constexpr std::string_view kGolden =
      "from wpimath import BiquadFilter\n"
      "\n"
      "lowpass = BiquadFilter([\n"
      "    BiquadFilter.Section(b0=1, b1=0, b2=0, a1=-0.5, a2=0.25),\n"
      "    BiquadFilter.Section(b0=1, b1=2, b2=1, a1=-0.125, a2=0.75),\n"
      "])\n";
  CHECK(EmitCode(sos, Language::Python, "lowpass") == kGolden);
}

}  // namespace
