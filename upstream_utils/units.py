#!/usr/bin/env python3

import os
import re
import shutil
from pathlib import Path

from llvm import _replace_macro_invocations

from upstream_utils import Lib, walk_cwd_and_copy_if, walk_if


def _replace_gtest_assertions(content: str):
    comparison_macros = {
        "EXPECT_EQ": ("CHECK", "=="),
        "EXPECT_STREQ": ("CHECK", "=="),
        "EXPECT_DOUBLE_EQ": ("CHECK", "=="),
        "EXPECT_STRNE": ("CHECK", "!="),
        "EXPECT_NE": ("CHECK", "!="),
        "EXPECT_LT": ("CHECK", "<"),
        "EXPECT_LE": ("CHECK", "<="),
        "EXPECT_GT": ("CHECK", ">"),
        "EXPECT_GE": ("CHECK", ">="),
        "ASSERT_EQ": ("REQUIRE", "=="),
        "ASSERT_NE": ("REQUIRE", "!="),
        "ASSERT_LT": ("REQUIRE", "<"),
        "ASSERT_LE": ("REQUIRE", "<="),
        "ASSERT_GT": ("REQUIRE", ">"),
        "ASSERT_GE": ("REQUIRE", ">="),
    }
    boolean_macros = {
        "EXPECT_TRUE": "CHECK",
        "EXPECT_FALSE": "CHECK_FALSE",
        "ASSERT_TRUE": "REQUIRE",
        "ASSERT_FALSE": "REQUIRE_FALSE",
    }

    def replace(macro: str, args: list[str]):
        if macro in comparison_macros and len(args) == 2:
            catch_macro, op = comparison_macros[macro]
            if macro == "EXPECT_STRNE" or macro == "EXPECT_STREQ":
                return f"{catch_macro}(std::string_view({args[0]}) {op} {args[1]})"
            return f"{catch_macro}({args[0]} {op} {args[1]})"
        if macro in boolean_macros and len(args) == 1:
            return f"{boolean_macros[macro]}({args[0]})"
        if macro in {"EXPECT_DEATH", "ASSERT_DEATH"} and len(args) == 2:
            return f"CHECK_DEATH({args[0]}, {args[1]})"
        if macro == "EXPECT_THROW" and len(args) == 2:
            return f"CHECK_THROWS_AS({args[0]}, {args[1]})"
        if macro == "EXPECT_NEAR" and len(args) == 3:
            return f"CHECK_THAT({args[0]}, Catch::Matchers::WithinAbs({args[1]}, {args[2]}))"
        return None

    return _replace_macro_invocations(
        content,
        set(comparison_macros)
        | set(boolean_macros)
        | {"EXPECT_DEATH", "ASSERT_DEATH", "EXPECT_THROW", "EXPECT_NEAR"},
        replace,
    )


def run_test_replacements(files: list[Path]):
    catch_includes = (
        "#include <catch2/catch_test_macros.hpp>\n"
        "#include <catch2/matchers/catch_matchers_floating_point.hpp>\n"
        "#include <catch2/matchers/catch_matchers_range_equals.hpp>\n"
        "#include <catch2/matchers/catch_matchers_vector.hpp>\n"
        "#include <catch2/catch_template_test_macros.hpp>\n"
    )

    for wpi_file in files:
        with open(wpi_file) as f:
            content = f.read()

        had_gtest = (
            '#include "gmock/gmock.h"\n' in content
            or '#include "gtest/gtest.h"\n' in content
            or "#include <gmock/gmock.h>\n" in content
            or "#include <gtest/gtest.h>\n" in content
        )
        content = content.replace('#include "gmock/gmock.h"\n', "")
        content = content.replace('#include "gtest/gtest.h"\n', "")
        content = content.replace("#include <gmock/gmock.h>\n", "")
        content = content.replace("#include <gtest/gtest.h>\n", "")
        content = content.replace(" : public ::testing::Test", "")
        if had_gtest and "#include <catch2/" not in content:
            include_match = re.search(r'(#include <[^"]+>\n)(?!#include ")', content)
            if include_match:
                content = (
                    content[: include_match.end()]
                    + catch_includes
                    + content[include_match.end() :]
                )

        content = re.sub(
            r"\bTEST\(([^,\n]+),\s*([^)]+)\)",
            r'TEST_CASE("\1 \2", "[wpimath][units]")',
            content,
        )
        content = re.sub(
            r"\bTEST_F\(([^,\n]+),\s*([^)]+)\)",
            r'TEST_CASE_METHOD(\1, "\1 \2", "[wpimath][units]")',
            content,
        )
        content = re.sub(r"\bSCOPED_TRACE\(", "UNSCOPED_INFO(", content)
        content = _replace_gtest_assertions(content)

        with open(wpi_file, "w") as f:
            f.write(content)


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    wpimath = wpilib_root / "wpimath"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/units/include",
        "src/test/native/cpp/units",
    ]:
        shutil.rmtree(wpimath / d, ignore_errors=True)

    # Copy units include files into allwpilib
    os.chdir(upstream_root / "include")
    files = walk_if(Path("."), lambda dp, f: True)
    src_include_files = [f.absolute() for f in files]
    wpimath_units_root = wpimath / "src/main/native/thirdparty/units/include/wpi"
    dest_include_files = [(wpimath_units_root / f).with_suffix(".hpp") for f in files]

    # Rename to .hpp
    for i in range(len(src_include_files)):
        dest_dir = dest_include_files[i].parent
        if not dest_dir.exists():
            dest_dir.mkdir(parents=True)
        shutil.copyfile(src_include_files[i], dest_include_files[i])

    os.chdir(upstream_root / "test")
    test_files = walk_cwd_and_copy_if(
        lambda dp, f: f == "main.cpp" or f == "odrDimensionConcept.h",
        wpimath / "src/test/native/cpp/units",
    )
    run_test_replacements(test_files)
    # Perform namespace renames
    for wpi_file in dest_include_files + test_files:
        content: str
        with open(wpi_file) as f:
            content = f.read()

        content = content.replace("units::", "wpi::units::")
        content = content.replace("namespace units", "namespace wpi::units")
        content = re.sub(
            "#include <units/(.*)\\.h", r"#include <wpi/units/\1.hpp", content
        )
        content = content.replace("#include <units.h>", "#include <wpi/units.hpp>")

        with open(wpi_file, "w") as f:
            f.write(content)


def main():
    name = "units"
    url = "https://github.com/nholthaus/units.git"
    tag = "v3.6.0"

    units = Lib(name, url, tag, copy_upstream_src)
    units.main()


if __name__ == "__main__":
    main()
