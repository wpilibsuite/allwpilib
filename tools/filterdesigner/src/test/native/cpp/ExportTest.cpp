// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/codegen/Export.hpp"

#include <filesystem>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"

namespace {

using wpi::filterdesigner::BuildExportFileContents;
using wpi::filterdesigner::ExportFilter;
using wpi::filterdesigner::ExportSpec;
using wpi::filterdesigner::Language;
using wpi::filterdesigner::NormalizeClassName;
using wpi::filterdesigner::NormalizeProjectRoot;
using wpi::filterdesigner::NormalizeVariableName;
using wpi::filterdesigner::ResolveExportPath;
using wpi::filterdesigner::Section;
using wpi::filterdesigner::Sections;
using wpi::filterdesigner::ToSnakeCase;

Sections OnePassthroughSection() {
  return {Section{1.0, 0.0, 0.0, 0.0, 0.0}};
}

// Coefficient values chosen so %.17g formats them identically across
// platforms — used by the golden tests below.
Sections TwoSectionGolden() {
  return {Section{1.0, 0.0, 0.0, -0.5, 0.25},
          Section{1.0, 2.0, 1.0, -0.125, 0.75}};
}

ExportSpec SimpleSpec() {
  return ExportSpec{1000.0,
                    "Sample rate: 1000 Hz\n"
                    "Stages:\n"
                    "  1. Low pass  order=4  cutoff=50 Hz\n"};
}

// NormalizeVariableName / NormalizeClassName ---------------------------------

TEST_CASE("ExportTest NormalizeVariableNameCamelCasesForCppAndJava",
          "[filterdesigner]") {
  for (Language lang : {Language::Cpp, Language::Java}) {
    CHECK(NormalizeVariableName("my filter", lang) == "myFilter");
    CHECK(NormalizeVariableName("filter-2", lang) == "filter2");
    CHECK(NormalizeVariableName("1filter", lang) == "_1filter");
    CHECK(NormalizeVariableName("a;b", lang) == "aB");
  }
}

TEST_CASE("ExportTest NormalizeVariableNameLeavesValidCppNamesAlone",
          "[filterdesigner]") {
  // Casing the user chose is theirs to keep, including the m_ convention.
  CHECK(NormalizeVariableName("shooterFilter", Language::Cpp) ==
        "shooterFilter");
  CHECK(NormalizeVariableName("m_shooterFilter", Language::Cpp) ==
        "m_shooterFilter");
}

TEST_CASE("ExportTest NormalizeVariableNameSnakeCasesForPython",
          "[filterdesigner]") {
  CHECK(NormalizeVariableName("my filter", Language::Python) == "my_filter");
  CHECK(NormalizeVariableName("shooterFilter", Language::Python) ==
        "shooter_filter");
  CHECK(NormalizeVariableName("filter-2", Language::Python) == "filter_2");
}

TEST_CASE("ExportTest NormalizeVariableNameFallsBackWhenNothingUsable",
          "[filterdesigner]") {
  for (Language lang : {Language::Cpp, Language::Java, Language::Python}) {
    CHECK(NormalizeVariableName("", lang) == "filter");
    CHECK(NormalizeVariableName("!!!", lang) == "filter");
  }
}

TEST_CASE("ExportTest NormalizeClassNamePascalCases", "[filterdesigner]") {
  CHECK(NormalizeClassName("my filter") == "MyFilter");
  CHECK(NormalizeClassName("shooterFilter") == "ShooterFilter");
  CHECK(NormalizeClassName("9bad") == "_9bad");
  CHECK(NormalizeClassName("") == "MyFilter");
}

// ToSnakeCase ----------------------------------------------------------------

TEST_CASE("ExportSnakeCaseTest EmptyStaysEmpty", "[filterdesigner]") {
  CHECK(ToSnakeCase("") == "");
}

TEST_CASE("ExportSnakeCaseTest AllLowerStaysSame", "[filterdesigner]") {
  CHECK(ToSnakeCase("foo") == "foo");
}

TEST_CASE("ExportSnakeCaseTest PascalCaseToSnake", "[filterdesigner]") {
  CHECK(ToSnakeCase("ShooterFilter") == "shooter_filter");
}

TEST_CASE("ExportSnakeCaseTest AbbreviationRunBoundary", "[filterdesigner]") {
  // The abbreviation/word boundary should land just before the trailing
  // single-word capital — `MyHTTPServer` → `my_http_server`.
  CHECK(ToSnakeCase("MyHTTPServer") == "my_http_server");
}

TEST_CASE("ExportSnakeCaseTest DigitBoundary", "[filterdesigner]") {
  CHECK(ToSnakeCase("Filter2Stage") == "filter2_stage");
}

TEST_CASE("ExportSnakeCaseTest LeadingUnderscorePreserved",
          "[filterdesigner]") {
  CHECK(ToSnakeCase("_FooBar") == "_foo_bar");
}

// NormalizeProjectRoot -------------------------------------------------------

TEST_CASE("ExportNormalizeRootTest EmptyStaysEmpty", "[filterdesigner]") {
  CHECK(NormalizeProjectRoot({}).empty());
}

TEST_CASE("ExportNormalizeRootTest AbsolutePathStaysAbsolute",
          "[filterdesigner]") {
  auto result = NormalizeProjectRoot("/tmp/whatever");
  CHECK(result.is_absolute());
  CHECK(result.filename() == "whatever");
}

TEST_CASE("ExportNormalizeRootTest RelativePathBecomesAbsolute",
          "[filterdesigner]") {
  auto result = NormalizeProjectRoot("relative/path");
  CHECK(result.is_absolute());
}

// ResolveExportPath ----------------------------------------------------------

TEST_CASE("ExportResolvePathTest JavaPathLayout", "[filterdesigner]") {
  auto p = ResolveExportPath("/work/robot", Language::Java, "ShooterFilter");
  // `/work/robot/src/main/java/frc/robot/filters/ShooterFilter.java`
  CHECK(p.filename() == "ShooterFilter.java");
  CHECK(p.parent_path().filename() == "filters");
  CHECK(p.parent_path().parent_path().filename() == "robot");
  CHECK(p.parent_path().parent_path().parent_path().filename() == "frc");
}

TEST_CASE("ExportResolvePathTest CppPathLayout", "[filterdesigner]") {
  auto p = ResolveExportPath("/work/robot", Language::Cpp, "ShooterFilter");
  // `/work/robot/src/main/include/filters/ShooterFilter.h`
  CHECK(p.filename() == "ShooterFilter.h");
  CHECK(p.parent_path().filename() == "filters");
  CHECK(p.parent_path().parent_path().filename() == "include");
}

TEST_CASE("ExportResolvePathTest PythonPathLayoutSnakeCase",
          "[filterdesigner]") {
  auto p = ResolveExportPath("/work/robot", Language::Python, "ShooterFilter");
  CHECK(p.filename() == "shooter_filter.py");
  CHECK(p.parent_path().filename() == "filters");
}

// BuildExportFileContents ----------------------------------------------------

TEST_CASE("ExportBuildContentsTest EmptySectionsReturnsEmptyString",
          "[filterdesigner]") {
  Sections empty;
  CHECK(BuildExportFileContents(empty, Language::Cpp, "Foo", SimpleSpec())
            .empty());
}

TEST_CASE("ExportBuildContentsTest CppHasHeaderAndClassAndCoeff",
          "[filterdesigner]") {
  auto sections = OnePassthroughSection();
  auto out = BuildExportFileContents(sections, Language::Cpp, "ShooterFilter",
                                     SimpleSpec());
  // Comment header.
  CHECK(out.find("DO NOT EDIT") != std::string::npos);
  // Spec description was prefixed with `// `.
  CHECK(out.find("// Sample rate: 1000 Hz") != std::string::npos);
  // C++-specific shape.
  CHECK(out.find("#pragma once") != std::string::npos);
  CHECK(out.find("#include <wpi/math/filter/BiquadFilter.hpp>") !=
        std::string::npos);
  CHECK(out.find("namespace frc::filters") != std::string::npos);
  CHECK(out.find("ShooterFilter()") != std::string::npos);
  CHECK(out.find("wpi::math::BiquadFilter") != std::string::npos);
}

TEST_CASE("ExportBuildContentsTest JavaHasPackageAndClassAndImport",
          "[filterdesigner]") {
  auto sections = OnePassthroughSection();
  auto out = BuildExportFileContents(sections, Language::Java, "ShooterFilter",
                                     SimpleSpec());
  CHECK(out.find("DO NOT EDIT") != std::string::npos);
  CHECK(out.find("package frc.robot.filters;") != std::string::npos);
  CHECK(out.find("import org.wpilib.math.filter.BiquadFilter;") !=
        std::string::npos);
  CHECK(out.find("public final class ShooterFilter") != std::string::npos);
  CHECK(out.find("BiquadFilter.Section(") != std::string::npos);
  // Java visibility plumbing — private ctor blocks instantiation.
  CHECK(out.find("private ShooterFilter() {}") != std::string::npos);
}

TEST_CASE("ExportBuildContentsTest PythonHasSnakeCaseFunctionAndImport",
          "[filterdesigner]") {
  auto sections = OnePassthroughSection();
  auto out = BuildExportFileContents(sections, Language::Python,
                                     "ShooterFilter", SimpleSpec());
  CHECK(out.find("# DO NOT EDIT") != std::string::npos);
  CHECK(out.find("# Sample rate: 1000 Hz") != std::string::npos);
  CHECK(out.find("from wpimath import BiquadFilter") != std::string::npos);
  CHECK(out.find("def shooter_filter() -> BiquadFilter") != std::string::npos);
  CHECK(out.find("BiquadFilter.Section(b0=") != std::string::npos);
}

// Full-output golden tests. Update on intentional formatting changes.

TEST_CASE("ExportBuildContentsTest CppGoldenFile", "[filterdesigner]") {
  constexpr std::string_view kGolden =
      "// Generated by WPILib Filter Designer.\n"
      "// DO NOT EDIT \xE2\x80\x94 regenerate via the Filter Designer tool.\n"
      "//\n"
      "// Sample rate: 1000 Hz\n"
      "// Stages:\n"
      "//   1. Low pass  order=4  cutoff=50 Hz\n"
      "\n"
      "#pragma once\n"
      "\n"
      "#include <wpi/math/filter/BiquadFilter.hpp>\n"
      "\n"
      "namespace frc::filters {\n"
      "\n"
      "inline wpi::math::BiquadFilter ShooterFilter() {\n"
      "  return wpi::math::BiquadFilter{\n"
      "      {1, 0, 0, -0.5, 0.25},\n"
      "      {1, 2, 1, -0.125, 0.75},\n"
      "  };\n"
      "}\n"
      "\n"
      "}  // namespace frc::filters\n";
  CHECK(BuildExportFileContents(TwoSectionGolden(), Language::Cpp,
                                "ShooterFilter", SimpleSpec()) == kGolden);
}

TEST_CASE("ExportBuildContentsTest JavaGoldenFile", "[filterdesigner]") {
  constexpr std::string_view kGolden =
      "// Generated by WPILib Filter Designer.\n"
      "// DO NOT EDIT \xE2\x80\x94 regenerate via the Filter Designer tool.\n"
      "//\n"
      "// Sample rate: 1000 Hz\n"
      "// Stages:\n"
      "//   1. Low pass  order=4  cutoff=50 Hz\n"
      "\n"
      "package frc.robot.filters;\n"
      "\n"
      "import org.wpilib.math.filter.BiquadFilter;\n"
      "\n"
      "public final class ShooterFilter {\n"
      "  public static BiquadFilter create() {\n"
      "    return new BiquadFilter(\n"
      "        new BiquadFilter.Section(1, 0, 0, -0.5, 0.25),\n"
      "        new BiquadFilter.Section(1, 2, 1, -0.125, 0.75)\n"
      "    );\n"
      "  }\n"
      "\n"
      "  private ShooterFilter() {}\n"
      "}\n";
  CHECK(BuildExportFileContents(TwoSectionGolden(), Language::Java,
                                "ShooterFilter", SimpleSpec()) == kGolden);
}

TEST_CASE("ExportBuildContentsTest PythonGoldenFile", "[filterdesigner]") {
  constexpr std::string_view kGolden =
      "# Generated by WPILib Filter Designer.\n"
      "# DO NOT EDIT \xE2\x80\x94 regenerate via the Filter Designer tool.\n"
      "#\n"
      "# Sample rate: 1000 Hz\n"
      "# Stages:\n"
      "#   1. Low pass  order=4  cutoff=50 Hz\n"
      "\n"
      "from wpimath import BiquadFilter\n"
      "\n"
      "\n"
      "def shooter_filter() -> BiquadFilter:\n"
      "    return BiquadFilter([\n"
      "        BiquadFilter.Section(b0=1, b1=0, b2=0, a1=-0.5, a2=0.25),\n"
      "        BiquadFilter.Section(b0=1, b1=2, b2=1, a1=-0.125, a2=0.75),\n"
      "    ])\n";
  CHECK(BuildExportFileContents(TwoSectionGolden(), Language::Python,
                                "ShooterFilter", SimpleSpec()) == kGolden);
}

// ExportFilter (round-trip on disk) -----------------------------------------

TEST_CASE("ExportFilterTest NormalizesInvalidClassName", "[filterdesigner]") {
  auto sections = OnePassthroughSection();
  auto root = std::filesystem::temp_directory_path() / "fd_export_test_invalid";
  auto result =
      ExportFilter(sections, Language::Cpp, "9bad", root, SimpleSpec());
  REQUIRE(result.ok);
  CHECK(result.writtenPath.filename().string() == "_9bad.h");
}

TEST_CASE("ExportFilterTest RejectsEmptyRoot", "[filterdesigner]") {
  auto sections = OnePassthroughSection();
  auto result = ExportFilter(sections, Language::Cpp, "Foo", {}, SimpleSpec());
  CHECK_FALSE(result.ok);
  CHECK(result.message.find("Project root is empty") != std::string::npos);
}

TEST_CASE("ExportFilterTest WritesFileAndReturnsAbsolutePath",
          "[filterdesigner]") {
  auto sections = OnePassthroughSection();
  auto root = std::filesystem::temp_directory_path() / "fd_export_test_write";
  std::filesystem::remove_all(root);
  auto result =
      ExportFilter(sections, Language::Cpp, "MyFilter", root, SimpleSpec());
  UNSCOPED_INFO(result.message);
  REQUIRE(result.ok);
  CHECK(result.writtenPath.is_absolute());
  CHECK(std::filesystem::exists(result.writtenPath));
  CHECK(result.writtenPath.filename() == "MyFilter.h");
  std::filesystem::remove_all(root);
}

}  // namespace
