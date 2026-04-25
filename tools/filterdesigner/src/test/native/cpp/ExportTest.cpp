// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/codegen/Export.hpp"

#include <filesystem>
#include <string>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace {

using wpi::filterdesigner::BuildExportFileContents;
using wpi::filterdesigner::ExportFilter;
using wpi::filterdesigner::ExportSpec;
using wpi::filterdesigner::IsValidIdentifier;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::NormalizeProjectRoot;
using wpi::filterdesigner::ResolveExportPath;
using wpi::filterdesigner::Section;
using wpi::filterdesigner::Sections;
using wpi::filterdesigner::ToSnakeCase;

Sections OnePassthroughSection() {
  return {Section{1.0, 0.0, 0.0, 0.0, 0.0}};
}

ExportSpec SimpleSpec() {
  return ExportSpec{1000.0,
                    "Sample rate: 1000 Hz\n"
                    "Stages:\n"
                    "  1. Low pass  order=4  cutoff=50 Hz\n"};
}

// IsValidIdentifier ----------------------------------------------------------

TEST(ExportIdentifierTest, RejectsEmpty) {
  EXPECT_FALSE(IsValidIdentifier(""));
}

TEST(ExportIdentifierTest, RejectsLeadingDigit) {
  EXPECT_FALSE(IsValidIdentifier("9abc"));
}

TEST(ExportIdentifierTest, RejectsHyphen) {
  EXPECT_FALSE(IsValidIdentifier("foo-bar"));
}

TEST(ExportIdentifierTest, RejectsSpace) {
  EXPECT_FALSE(IsValidIdentifier("foo bar"));
}

TEST(ExportIdentifierTest, AcceptsLeadingUnderscore) {
  EXPECT_TRUE(IsValidIdentifier("_foo"));
}

TEST(ExportIdentifierTest, AcceptsAlphanumericAndUnderscore) {
  EXPECT_TRUE(IsValidIdentifier("Shooter_Filter_2"));
}

TEST(ExportIdentifierTest, AcceptsSingleLetter) {
  EXPECT_TRUE(IsValidIdentifier("X"));
}

// ToSnakeCase ----------------------------------------------------------------

TEST(ExportSnakeCaseTest, EmptyStaysEmpty) {
  EXPECT_EQ(ToSnakeCase(""), "");
}

TEST(ExportSnakeCaseTest, AllLowerStaysSame) {
  EXPECT_EQ(ToSnakeCase("foo"), "foo");
}

TEST(ExportSnakeCaseTest, PascalCaseToSnake) {
  EXPECT_EQ(ToSnakeCase("ShooterFilter"), "shooter_filter");
}

TEST(ExportSnakeCaseTest, AbbreviationRunBoundary) {
  // The abbreviation/word boundary should land just before the trailing
  // single-word capital — `MyHTTPServer` → `my_http_server`.
  EXPECT_EQ(ToSnakeCase("MyHTTPServer"), "my_http_server");
}

TEST(ExportSnakeCaseTest, DigitBoundary) {
  EXPECT_EQ(ToSnakeCase("Filter2Stage"), "filter2_stage");
}

TEST(ExportSnakeCaseTest, LeadingUnderscorePreserved) {
  EXPECT_EQ(ToSnakeCase("_FooBar"), "_foo_bar");
}

// NormalizeProjectRoot -------------------------------------------------------

TEST(ExportNormalizeRootTest, EmptyStaysEmpty) {
  EXPECT_TRUE(NormalizeProjectRoot({}).empty());
}

TEST(ExportNormalizeRootTest, AbsolutePathStaysAbsolute) {
  auto result = NormalizeProjectRoot("/tmp/whatever");
  EXPECT_TRUE(result.is_absolute());
  EXPECT_EQ(result.filename(), "whatever");
}

TEST(ExportNormalizeRootTest, RelativePathBecomesAbsolute) {
  auto result = NormalizeProjectRoot("relative/path");
  EXPECT_TRUE(result.is_absolute());
}

// ResolveExportPath ----------------------------------------------------------

TEST(ExportResolvePathTest, JavaPathLayout) {
  auto p = ResolveExportPath("/work/robot", Language::Java, "ShooterFilter");
  // `/work/robot/src/main/java/frc/robot/filters/ShooterFilter.java`
  EXPECT_EQ(p.filename(), "ShooterFilter.java");
  EXPECT_EQ(p.parent_path().filename(), "filters");
  EXPECT_EQ(p.parent_path().parent_path().filename(), "robot");
  EXPECT_EQ(p.parent_path().parent_path().parent_path().filename(), "frc");
}

TEST(ExportResolvePathTest, CppPathLayout) {
  auto p = ResolveExportPath("/work/robot", Language::Cpp, "ShooterFilter");
  // `/work/robot/src/main/include/filters/ShooterFilter.h`
  EXPECT_EQ(p.filename(), "ShooterFilter.h");
  EXPECT_EQ(p.parent_path().filename(), "filters");
  EXPECT_EQ(p.parent_path().parent_path().filename(), "include");
}

TEST(ExportResolvePathTest, PythonPathLayoutSnakeCase) {
  auto p = ResolveExportPath("/work/robot", Language::Python, "ShooterFilter");
  EXPECT_EQ(p.filename(), "shooter_filter.py");
  EXPECT_EQ(p.parent_path().filename(), "filters");
}

// BuildExportFileContents ----------------------------------------------------

TEST(ExportBuildContentsTest, EmptySectionsReturnsEmptyString) {
  Sections empty;
  EXPECT_TRUE(BuildExportFileContents(empty, Language::Cpp, "Foo", SimpleSpec())
                  .empty());
}

TEST(ExportBuildContentsTest, CppHasHeaderAndClassAndCoeff) {
  auto sections = OnePassthroughSection();
  auto out = BuildExportFileContents(sections, Language::Cpp, "ShooterFilter",
                                     SimpleSpec());
  // Comment header.
  EXPECT_NE(out.find("DO NOT EDIT"), std::string::npos);
  // Spec description was prefixed with `// `.
  EXPECT_NE(out.find("// Sample rate: 1000 Hz"), std::string::npos);
  // C++-specific shape.
  EXPECT_NE(out.find("#pragma once"), std::string::npos);
  EXPECT_NE(out.find("#include <wpi/math/filter/BiquadFilter.hpp>"),
            std::string::npos);
  EXPECT_NE(out.find("namespace frc::filters"), std::string::npos);
  EXPECT_NE(out.find("ShooterFilter()"), std::string::npos);
  EXPECT_NE(out.find("wpi::math::BiquadFilter"), std::string::npos);
}

TEST(ExportBuildContentsTest, JavaHasPackageAndClassAndImport) {
  auto sections = OnePassthroughSection();
  auto out = BuildExportFileContents(sections, Language::Java, "ShooterFilter",
                                     SimpleSpec());
  EXPECT_NE(out.find("DO NOT EDIT"), std::string::npos);
  EXPECT_NE(out.find("package frc.robot.filters;"), std::string::npos);
  EXPECT_NE(out.find("import org.wpilib.math.filter.BiquadFilter;"),
            std::string::npos);
  EXPECT_NE(out.find("public final class ShooterFilter"), std::string::npos);
  EXPECT_NE(out.find("BiquadFilter.Section("), std::string::npos);
  // Java visibility plumbing — private ctor blocks instantiation.
  EXPECT_NE(out.find("private ShooterFilter() {}"), std::string::npos);
}

TEST(ExportBuildContentsTest, PythonHasSnakeCaseFunctionAndImport) {
  auto sections = OnePassthroughSection();
  auto out = BuildExportFileContents(sections, Language::Python,
                                     "ShooterFilter", SimpleSpec());
  EXPECT_NE(out.find("# DO NOT EDIT"), std::string::npos);
  EXPECT_NE(out.find("# Sample rate: 1000 Hz"), std::string::npos);
  EXPECT_NE(out.find("from wpimath.filter import BiquadFilter"),
            std::string::npos);
  EXPECT_NE(out.find("def shooter_filter() -> BiquadFilter"),
            std::string::npos);
  EXPECT_NE(out.find("BiquadFilter.Section(b0="), std::string::npos);
}

// ExportFilter (round-trip on disk) -----------------------------------------

TEST(ExportFilterTest, RejectsInvalidClassName) {
  auto sections = OnePassthroughSection();
  auto root = std::filesystem::temp_directory_path() / "fd_export_test_invalid";
  auto result =
      ExportFilter(sections, Language::Cpp, "9bad", root, SimpleSpec());
  EXPECT_FALSE(result.ok);
  EXPECT_NE(result.message.find("Invalid class name"), std::string::npos);
}

TEST(ExportFilterTest, RejectsEmptyRoot) {
  auto sections = OnePassthroughSection();
  auto result = ExportFilter(sections, Language::Cpp, "Foo", {}, SimpleSpec());
  EXPECT_FALSE(result.ok);
  EXPECT_NE(result.message.find("Project root is empty"), std::string::npos);
}

TEST(ExportFilterTest, WritesFileAndReturnsAbsolutePath) {
  auto sections = OnePassthroughSection();
  auto root = std::filesystem::temp_directory_path() / "fd_export_test_write";
  std::filesystem::remove_all(root);
  auto result =
      ExportFilter(sections, Language::Cpp, "MyFilter", root, SimpleSpec());
  ASSERT_TRUE(result.ok) << result.message;
  EXPECT_TRUE(result.writtenPath.is_absolute());
  EXPECT_TRUE(std::filesystem::exists(result.writtenPath));
  EXPECT_EQ(result.writtenPath.filename(), "MyFilter.h");
  std::filesystem::remove_all(root);
}

}  // namespace
