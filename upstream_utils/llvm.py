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
        content = content.replace("namespace llvm", "namespace wpi")
        content = content.replace("llvm:", "wpi:")

        # Fix #includes
        content = content.replace('include "llvm/ADT', 'include "wpi/util')
        content = content.replace('include "llvm/Config', 'include "wpi/util')
        content = content.replace('include "llvm/Support', 'include "wpi/util')
        content = content.replace('include "CountCopyAndMove.h"', 'include "CountCopyAndMove.hpp"')
        content = content.replace('include "Windows/WindowsSupport.h"', 'include "Windows/WindowsSupport.hpp"')

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
        content = content.replace('#include "wpi/util/config.h"\n', "")
        content = content.replace('#include "wpi/util/Signals.h"\n', "")
        content = content.replace('#include "wpi/util/Process.h"\n', "")
        content = content.replace('#include "wpi/util/Path.h"\n', "")
        content = content.replace('#include "wpi/util/Program.h"\n', "")

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
        content = content.replace("/// Defines the wpi::", "/// Defines the llvm::")
        content = content.replace("// end llvm namespace", "// end wpi namespace")
        content = content.replace("// end namespace llvm", "// end namespace wpi")
        content = content.replace("// End llvm namespace", "// End wpi namespace")

        content = content.replace("fs::openFileForRead", "fs::OpenFileForRead")

        # Handle move from .h -> .hpp
        content = re.sub('#include "wpi/util/(.*).h"', r'#include "wpi/util/\1.hpp"', content)

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


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    wpiutil = wpilib_root / "wpiutil"

    overwrite_source(wpiutil, upstream_root)
    overwrite_tests(wpiutil, upstream_root)


def main():
    name = "llvm"
    url = "https://github.com/llvm/llvm-project"
    tag = "llvmorg-20.1.7"

    patch_options = {
        "use_threeway": True,
    }

    llvm = Lib(name, url, tag, copy_upstream_src, patch_options)
    llvm.main()


if __name__ == "__main__":
    main()
