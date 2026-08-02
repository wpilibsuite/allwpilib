#!/usr/bin/env python3

import re
import shutil
from pathlib import Path

from upstream_utils import Lib


def run_global_replacements(wpiutil_llvm_files: list[Path]):
    for wpi_file in wpiutil_llvm_files:
        with open(wpi_file) as f:
            content = f.read()

        # Rename namespace from llvm to wpi
        content = content.replace("namespace llvm", "namespace wpi::util")
        content = content.replace("llvm:", "wpi::util:")

        # Fix #includes
        content = content.replace('include "llvm/ADT', 'include "wpi/util')
        content = content.replace('include "llvm/Config', 'include "wpi/util')
        content = content.replace('include "llvm/Support', 'include "wpi/util')
        content = content.replace(
            'include "CountCopyAndMove.h"', 'include "CountCopyAndMove.hpp"'
        )
        content = content.replace(
            'include "Windows/WindowsSupport.h"', 'include "Windows/WindowsSupport.hpp"'
        )

        # Fix uses of span
        content = content.replace("span", "std::span")
        content = content.replace("include <std::span>", "include <span>")
        if wpi_file.name == "ConvertUTFWrapper.cpp":
            content = content.replace(
                "const UTF16 *Src = reinterpret_cast<const UTF16 *>(SrcBytes.begin());",
                "const UTF16 *Src = reinterpret_cast<const UTF16 *>(&*SrcBytes.begin());",
            )
            content = content.replace(
                "const UTF16 *SrcEnd = reinterpret_cast<const UTF16 *>(SrcBytes.end());",
                "const UTF16 *SrcEnd = reinterpret_cast<const UTF16 *>(&*SrcBytes.begin() + SrcBytes.size());",
            )
            content = content.replace(
                "const UTF32 *Src = reinterpret_cast<const UTF32 *>(SrcBytes.begin());",
                "const UTF32 *Src = reinterpret_cast<const UTF32 *>(&*SrcBytes.begin());",
            )
            content = content.replace(
                "const UTF32 *SrcEnd = reinterpret_cast<const UTF32 *>(SrcBytes.end());",
                "const UTF32 *SrcEnd = reinterpret_cast<const UTF32 *>(&*SrcBytes.begin() + SrcBytes.size());",
            )

        # Remove unused headers
        content = content.replace('#include "llvm-c/ErrorHandling.h"\n', "")
        content = content.replace('#include "wpi/util/Debug.h"\n', "")
        content = content.replace('#include "wpi/util/Error.h"\n', "")
        content = content.replace('#include "wpi/util/Format.h"\n', "")
        content = content.replace('#include "wpi/util/FormatVariadic.h"\n', "")
        content = content.replace('#include "wpi/util/Hashing.h"\n', "")
        content = content.replace('#include "wpi/util/NativeFormatting.h"\n', "")
        content = content.replace('#include "wpi/util/Threading.h"\n', "")
        content = content.replace('#include "wpi/util/DataTypes.h"\n', "")
        content = content.replace('#include "wpi/util/llvm-config.h"\n', "")
        content = content.replace('#include "wpi/util/abi-breaking.h"\n', "")
        content = content.replace('#include "wpi/util/bit.h"\n', "")
        content = content.replace('#include "wpi/util/config.h"\n', "")
        content = content.replace('#include "wpi/util/Signals.h"\n', "")
        content = content.replace('#include "wpi/util/Process.h"\n', "")
        content = content.replace('#include "wpi/util/Path.h"\n', "")
        content = content.replace('#include "wpi/util/Program.h"\n', "")
        content = content.replace('#include "wpi/util/SwapByteOrder.h"\n', "")

        # Fix include guards
        content = content.replace("LLVM_ADT_", "WPIUTIL_WPI_")
        content = content.replace("LLVM_SUPPORT_", "WPIUTIL_WPI_")
        content = content.replace("LLVM_DEFINED_HAS_FEATURE", "WPI_DEFINED_HAS_FEATURE")

        content = content.replace("const std::string_view &", "std::string_view ")
        content = content.replace("sys::fs::openFileForRead", "fs::OpenFileForRead")
        content = content.replace("sys::fs::closeFile", "fs::CloseFile")
        content = content.replace("sys::fs::", "fs::")

        # Replace wpi/FileSystem.h with wpi/fs.h
        content = content.replace(
            'include "wpi/util/FileSystem.h"', 'include "wpi/util/fs.hpp"'
        )

        # Replace llvm_unreachable() with wpi_unreachable()
        content = content.replace("llvm_unreachable", "wpi_unreachable")

        content = content.replace("llvm_is_multithreaded()", "1")

        # Revert message in copyright header
        content = content.replace(
            "/// Defines the wpi::util::", "/// Defines the llvm::"
        )
        content = content.replace("// end llvm namespace", "// end wpi::util namespace")
        content = content.replace("// end namespace llvm", "// end namespace wpi::util")
        content = content.replace("// End llvm namespace", "// End wpi::util namespace")

        content = content.replace("fs::openFileForRead", "fs::OpenFileForRead")

        # Handle move from .h -> .hpp
        content = re.sub(
            '#include "wpi/util/(.*).h"', r'#include "wpi/util/\1.hpp"', content
        )

        with open(wpi_file, "w") as f:
            f.write(content)


def _find_matching_paren(content: str, open_paren: int):
    depth = 1
    i = open_paren + 1
    in_string = None
    escaped = False

    while i < len(content):
        ch = content[i]
        if in_string:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == in_string:
                in_string = None
        elif ch in ('"', "'"):
            in_string = ch
        elif ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
            if depth == 0:
                return i
        i += 1

    return None


def _split_macro_args(args: str):
    split_args = []
    start = 0
    depth = 0
    brace_depth = 0
    bracket_depth = 0
    in_string = None
    escaped = False

    for i, ch in enumerate(args):
        if in_string:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == in_string:
                in_string = None
            continue

        if ch in ('"', "'"):
            in_string = ch
        elif ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        elif ch == "{":
            brace_depth += 1
        elif ch == "}":
            brace_depth -= 1
        elif ch == "[":
            bracket_depth += 1
        elif ch == "]":
            bracket_depth -= 1
        elif (
            ch == ","
            and depth == 0
            and brace_depth == 0
            and bracket_depth == 0
        ):
            split_args.append(args[start:i].strip())
            start = i + 1

    split_args.append(args[start:].strip())
    return split_args


def _replace_macro_invocations(content: str, macros: set[str], replacement):
    pattern = re.compile(r"\b(" + "|".join(re.escape(m) for m in macros) + r")\s*\(")
    pos = 0
    output = []

    while True:
        match = pattern.search(content, pos)
        if match is None:
            output.append(content[pos:])
            break

        open_paren = match.end() - 1
        close_paren = _find_matching_paren(content, open_paren)
        if close_paren is None:
            output.append(content[pos:])
            break

        macro = match.group(1)
        args = content[open_paren + 1 : close_paren]
        new_text = replacement(macro, _split_macro_args(args))
        if new_text is None:
            output.append(content[pos : close_paren + 1])
        else:
            output.append(content[pos : match.start()])
            output.append(new_text)
        pos = close_paren + 1

    return "".join(output)


def _replace_gtest_assertions(content: str):
    comparison_macros = {
        "EXPECT_EQ": ("CHECK", "=="),
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
            return f"{catch_macro}({args[0]} {op} {args[1]})"
        if macro in boolean_macros and len(args) == 1:
            return f"{boolean_macros[macro]}({args[0]})"
        if macro in {"EXPECT_DEATH", "ASSERT_DEATH"} and len(args) == 2:
            return f"CHECK_DEATH({args[0]}, {args[1]})"
        return None

    return _replace_macro_invocations(
        content,
        set(comparison_macros)
        | set(boolean_macros)
        | {"EXPECT_DEATH", "ASSERT_DEATH"},
        replace,
    )


def run_test_replacements(wpiutil_llvm_files: list[Path]):
    catch_includes = (
        "#include <catch2/catch_test_macros.hpp>\n"
        "#include <catch2/matchers/catch_matchers_floating_point.hpp>\n"
        "#include <catch2/matchers/catch_matchers_range_equals.hpp>\n"
        "#include <catch2/matchers/catch_matchers_vector.hpp>\n"
        "#include <catch2/catch_template_test_macros.hpp>\n"
    )

    for wpi_file in wpiutil_llvm_files:
        if wpi_file.suffix != ".cpp":
            continue

        with open(wpi_file) as f:
            content = f.read()

        had_gtest = (
            '#include "gmock/gmock.h"\n' in content
            or '#include "gtest/gtest.h"\n' in content
        )
        content = content.replace('#include "gmock/gmock.h"\n', "")
        content = content.replace('#include "gtest/gtest.h"\n', "")
        if had_gtest and "#include <catch2/" not in content:
            include_match = re.search(r'(#include "[^"]+"\n)(?!#include ")', content)
            if include_match:
                content = (
                    content[: include_match.end()]
                    + catch_includes
                    + content[include_match.end() :]
                )

        content = re.sub(
            r"\bTEST\(([^,\n]+),\s*([^)]+)\)",
            r'TEST_CASE("\1 \2", "[wpiutil][llvm]")',
            content,
        )
        content = re.sub(
            r"\bTEST_F\(([^,\n]+),\s*([^)]+)\)",
            r'TEST_CASE_METHOD(\1, "\1 \2", "[wpiutil][llvm]")',
            content,
        )
        content = re.sub(r"\bSCOPED_TRACE\(", "UNSCOPED_INFO(", content)
        content = _replace_gtest_assertions(content)

        with open(wpi_file, "w") as f:
            f.write(content)


def flattened_llvm_files(llvm: Path, dirs_to_keep: list[Path]):
    file_lookup: dict[str, Path] = {}

    for dir_to_keep in dirs_to_keep:
        dir_to_crawl = llvm / dir_to_keep
        for root, _, files in dir_to_crawl.walk():
            for f in files:
                file_lookup[f] = root / f

    return file_lookup


def find_wpiutil_llvm_files(wpiutil_root: Path, subfolder: str):
    # These files have substantial changes, not worth managing with the patching process
    ignore_list = [
        "StringExtras.hpp",
        "StringExtras.cpp",
        "MemoryBuffer.cpp",
        "MemoryBuffer.hpp",
        "SmallVectorMemoryBuffer.hpp",
    ]

    wpiutil_files: list[Path] = []
    for root, _, files in (wpiutil_root / subfolder).walk():
        for f in files:
            if f not in ignore_list:
                full_file = root / f
                wpiutil_files.append(full_file)

    return wpiutil_files


def overwrite_files(wpiutil_files: list[Path], llvm_files: dict[str, Path]):
    # Very sparse rips from LLVM sources. Not worth tyring to make match upstream
    unmatched_files_whitelist = ["fs.h", "fs.cpp", "function_ref.h"]

    for wpi_file in wpiutil_files:
        wpi_base_name = wpi_file.name
        if wpi_file.suffix == ".hpp":
            wpi_base_name = wpi_file.with_suffix(".h").name

        if wpi_base_name in llvm_files:
            if wpi_file.suffix == ".h":
                shutil.copyfile(llvm_files[wpi_base_name], wpi_file.with_suffix(".hpp"))
            else:
                shutil.copyfile(llvm_files[wpi_base_name], wpi_file)

        elif wpi_base_name not in unmatched_files_whitelist:
            print(f"No file match for {wpi_file}, check if LLVM deleted it")


def overwrite_source(wpiutil_root: Path, llvm_root: Path):
    llvm_files = flattened_llvm_files(
        llvm_root,
        [
            Path("llvm/include/llvm/ADT/"),
            Path("llvm/include/llvm/Config/"),
            Path("llvm/include/llvm/Support/"),
            Path("llvm/lib/Support/"),
        ],
    )
    wpi_files = find_wpiutil_llvm_files(
        wpiutil_root, "src/main/native/thirdparty/llvm/include/wpi"
    ) + find_wpiutil_llvm_files(
        wpiutil_root, "src/main/native/thirdparty/llvm/cpp/llvm"
    )

    overwrite_files(wpi_files, llvm_files)
    run_global_replacements(wpi_files)


def overwrite_tests(wpiutil_root: Path, llvm_root: Path):
    llvm_files = flattened_llvm_files(
        llvm_root,
        [
            Path("llvm/unittests/ADT/"),
            Path("llvm/unittests/Config/"),
            Path("llvm/unittests/Support/"),
        ],
    )
    wpi_files = find_wpiutil_llvm_files(wpiutil_root, "src/test/native/cpp/llvm")

    overwrite_files(wpi_files, llvm_files)
    run_global_replacements(wpi_files)
    run_test_replacements(wpi_files)


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    wpiutil = wpilib_root / "wpiutil"

    overwrite_source(wpiutil, upstream_root)
    overwrite_tests(wpiutil, upstream_root)


def main():
    name = "llvm"
    url = "https://github.com/llvm/llvm-project"
    tag = "llvmorg-22.1.6"

    patch_options = {
        "use_threeway": True,
    }

    llvm = Lib(name, url, tag, copy_upstream_src, patch_options)
    llvm.main()


if __name__ == "__main__":
    main()
