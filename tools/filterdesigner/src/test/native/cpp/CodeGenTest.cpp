// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/codegen/CodeGen.hpp"

#include <string>

#include <gtest/gtest.h>

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

TEST(CodeGenTest, EmptySectionsReturnsEmptyString) {
  EXPECT_EQ(EmitCode(Sections{}, Language::Cpp), "");
  EXPECT_EQ(EmitCode(Sections{}, Language::Java), "");
  EXPECT_EQ(EmitCode(Sections{}, Language::Python), "");
}

TEST(CodeGenTest, CppEmitsBraceInitializedBiquadFilter) {
  Sections sos{{0.5, 0.25, 0.125, -0.75, 0.25}};
  std::string code = EmitCode(sos, Language::Cpp, "lowpass");
  EXPECT_NE(code.find("wpi::math::BiquadFilter lowpass{"), std::string::npos);
  EXPECT_NE(code.find("{0.5, 0.25, 0.125, -0.75, 0.25},"), std::string::npos);
  EXPECT_EQ(code.back(), '\n');
}

TEST(CodeGenTest, JavaEmitsVarargsBiquadFilterCtor) {
  Sections sos{{0.5, 0.25, 0.125, -0.75, 0.25}};
  std::string code = EmitCode(sos, Language::Java, "lowpass");
  EXPECT_NE(code.find("BiquadFilter lowpass = new BiquadFilter("),
            std::string::npos);
  EXPECT_NE(
      code.find("new BiquadFilter.Section(0.5, 0.25, 0.125, -0.75, 0.25)"),
      std::string::npos);
}

TEST(CodeGenTest, CppMultiSectionHasOnePerLineWithTrailingComma) {
  // Chosen so %.17g formats them identically on every platform.
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  std::string code = EmitCode(sos, Language::Cpp);
  EXPECT_NE(code.find("{1, 0, 0, -0.5, 0.25}"), std::string::npos);
  EXPECT_NE(code.find("{1, 2, 1, -0.125, 0.75}"), std::string::npos);
  EXPECT_NE(code.find("};"), std::string::npos);
}

TEST(CodeGenTest, JavaMultiSectionInsertsCommaBetweenSectionsNotAfterLast) {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  std::string code = EmitCode(sos, Language::Java);
  // Trailing comma after last section would be a syntax error in Java.
  auto lastSection = code.rfind("new BiquadFilter.Section(");
  ASSERT_NE(lastSection, std::string::npos);
  auto closeParen = code.find(')', lastSection);
  ASSERT_NE(closeParen, std::string::npos);
  EXPECT_EQ(code[closeParen + 1], '\n');
}

TEST(CodeGenTest, UsesHighPrecisionForScipyGoldenValues) {
  auto filter = SectionsOf(BiquadFilter::Notch(1000_Hz, 60_Hz, 10.0));
  std::string code = EmitCode(filter, Language::Cpp);
  // Enough precision to reproduce the scipy-matching coefficient.
  EXPECT_NE(code.find("0.9814970254751"), std::string::npos);
}

TEST(CodeGenTest, PythonEmitsWpimathFilterBiquadFilter) {
  Sections sos{{0.5, 0.25, 0.125, -0.75, 0.25}};
  std::string code = EmitCode(sos, Language::Python, "lowpass");
  EXPECT_NE(code.find("from wpimath.filter import BiquadFilter"),
            std::string::npos);
  EXPECT_NE(code.find("lowpass = BiquadFilter(["), std::string::npos);
  EXPECT_NE(
      code.find("BiquadFilter.Section(b0=0.5, b1=0.25, b2=0.125, a1=-0.75, "
                "a2=0.25)"),
      std::string::npos);
  EXPECT_NE(code.find("])"), std::string::npos);
}

TEST(CodeGenTest, DefaultVariableNameIsFilter) {
  Sections sos{{1.0, 0.0, 0.0, 0.0, 0.0}};
  EXPECT_NE(EmitCode(sos, Language::Cpp).find("BiquadFilter filter"),
            std::string::npos);
  EXPECT_NE(EmitCode(sos, Language::Java).find("BiquadFilter filter "),
            std::string::npos);
}

TEST(CodeGenTest, CppGoldenSnippet) {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  constexpr std::string_view kGolden =
      "wpi::math::BiquadFilter lowpass{\n"
      "    {1, 0, 0, -0.5, 0.25},\n"
      "    {1, 2, 1, -0.125, 0.75},\n"
      "};\n";
  EXPECT_EQ(EmitCode(sos, Language::Cpp, "lowpass"), kGolden);
}

TEST(CodeGenTest, JavaGoldenSnippet) {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  constexpr std::string_view kGolden =
      "BiquadFilter lowpass = new BiquadFilter(\n"
      "    new BiquadFilter.Section(1, 0, 0, -0.5, 0.25),\n"
      "    new BiquadFilter.Section(1, 2, 1, -0.125, 0.75)\n"
      ");\n";
  EXPECT_EQ(EmitCode(sos, Language::Java, "lowpass"), kGolden);
}

TEST(CodeGenTest, PythonGoldenSnippet) {
  Sections sos{{1.0, 0.0, 0.0, -0.5, 0.25}, {1.0, 2.0, 1.0, -0.125, 0.75}};
  constexpr std::string_view kGolden =
      "from wpimath.filter import BiquadFilter\n"
      "\n"
      "lowpass = BiquadFilter([\n"
      "    BiquadFilter.Section(b0=1, b1=0, b2=0, a1=-0.5, a2=0.25),\n"
      "    BiquadFilter.Section(b0=1, b1=2, b2=1, a1=-0.125, a2=0.75),\n"
      "])\n";
  EXPECT_EQ(EmitCode(sos, Language::Python, "lowpass"), kGolden);
}

}  // namespace
