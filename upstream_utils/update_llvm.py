#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    setup_upstream_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    am_patches,
    walk_if,
    copy_to,
)


def run_global_replacements(wpiutil_llvm_files):

    for wpi_file in wpiutil_llvm_files:
        with open(wpi_file) as f:
            content = f.read()

        # Rename namespace from llvm to wpi
        content = content.replace("namespace llvm", "namespace wpi")
        content = content.replace("llvm::", "wpi::")

        # Fix #includes
        content = content.replace('include "llvm/ADT', 'include "wpi')
        content = content.replace('include "llvm/Config', 'include "wpi')
        content = content.replace('include "llvm/Support', 'include "wpi')

        # Remove unused headers
        content = content.replace('#include "llvm-c/ErrorHandling.h"\n', "")
        content = content.replace('#include "wpi/Debug.h"\n', "")
        content = content.replace('#include "wpi/Error.h"\n', "")
        content = content.replace('#include "wpi/Format.h"\n', "")
        content = content.replace('#include "wpi/FormatVariadic.h"\n', "")
        content = content.replace('#include "wpi/NativeFormatting.h"\n', "")
        content = content.replace('#include "wpi/Threading.h"\n', "")
        content = content.replace('#include "wpi/DataTypes.h"\n', "")
        content = content.replace('#include "wpi/llvm-config.h"\n', "")
        content = content.replace('#include "wpi/abi-breaking.h"\n', "")
        content = content.replace('#include "wpi/config.h"\n', "")
        content = content.replace('#include "wpi/Signals.h"\n', "")
        content = content.replace('#include "wpi/Process.h"\n', "")
        content = content.replace('#include "wpi/Path.h"\n', "")
        content = content.replace('#include "wpi/Program.h"\n', "")

        # Fix include guards
        content = content.replace("LLVM_ADT_", "WPIUTIL_WPI_")
        content = content.replace("LLVM_SUPPORT_", "WPIUTIL_WPI_")
        content = content.replace("LLVM_DEFINED_HAS_FEATURE", "WPI_DEFINED_HAS_FEATURE")

        content = content.replace("const std::string_view &", "std::string_view ")
        content = content.replace("sys::fs::openFileForRead", "fs::OpenFileForRead")
        content = content.replace("sys::fs::closeFile", "fs::CloseFile")
        content = content.replace("sys::fs::", "fs::")

        # Replace wpi/FileSystem.h with wpi/fs.h
        content = content.replace('include "wpi/FileSystem.h"', 'include "wpi/fs.h"')

        # Replace llvm_unreachable() with wpi_unreachable()
        content = content.replace("llvm_unreachable", "wpi_unreachable")

        content = content.replace("llvm_is_multithreaded()", "1")

        # Revert message in copyright header
        content = content.replace("/// Defines the wpi::", "/// Defines the llvm::")
        content = content.replace("// end llvm namespace", "// end wpi namespace")
        content = content.replace("// end namespace llvm", "// end namespace wpi")
        content = content.replace("// End llvm namespace", "// End wpi namespace")

        content = content.replace("fs::openFileForRead", "fs::OpenFileForRead")

        with open(wpi_file, "w") as f:
            f.write(content)


def flattened_llvm_files(llvm, dirs_to_keep):
    file_lookup = {}

    for dir_to_keep in dirs_to_keep:
        dir_to_crawl = os.path.join(llvm, dir_to_keep)
        for root, _, files in os.walk(dir_to_crawl):
            for f in files:
                file_lookup[f] = os.path.join(root, f)

    return file_lookup


def find_wpiutil_llvm_files(wpiutil_root, subfolder):

    # These files have substantial changes, not worth managing with the patching process
    ignore_list = [
        "StringExtras.h",
        "StringExtras.cpp",
        "MemoryBuffer.cpp",
        "MemoryBuffer.h",
        "SmallVectorMemoryBuffer.h",
    ]

    wpiutil_files = []
    for root, _, files in os.walk(os.path.join(wpiutil_root, subfolder)):
        for f in files:
            if f not in ignore_list:
                full_file = os.path.join(root, f)
                wpiutil_files.append(full_file)

    return wpiutil_files


def overwrite_files(wpiutil_files, llvm_files):
    # Very sparse rips from LLVM sources. Not worth tyring to make match upstream
    unmatched_files_whitelist = ["fs.h", "fs.cpp", "function_ref.h"]

    for wpi_file in wpiutil_files:
        wpi_base_name = os.path.basename(wpi_file)
        if wpi_base_name not in llvm_files:
            if wpi_base_name not in unmatched_files_whitelist:
                print(f"No file match for {wpi_file}, check if LLVM deleted it")
        else:
            shutil.copyfile(llvm_files[wpi_base_name], wpi_file)


def overwrite_source(wpiutil_root, llvm_root):
    llvm_files = flattened_llvm_files(
        llvm_root,
        [
            "llvm/include/llvm/ADT/",
            "llvm/include/llvm/Config",
            "llvm/include/llvm/Support/",
            "llvm/lib/Support/",
        ],
    )
    wpi_files = find_wpiutil_llvm_files(
        wpiutil_root, "src/main/native/thirdparty/llvm/include/wpi"
    ) + find_wpiutil_llvm_files(
        wpiutil_root, "src/main/native/thirdparty/llvm/cpp/llvm"
    )

    overwrite_files(wpi_files, llvm_files)
    run_global_replacements(wpi_files)


def overwrite_tests(wpiutil_root, llvm_root):
    llvm_files = flattened_llvm_files(
        llvm_root,
        ["llvm/unittests/ADT/", "llvm/unittests/Config", "llvm/unittests/Support/"],
    )
    wpi_files = find_wpiutil_llvm_files(wpiutil_root, "src/test/native/cpp/llvm")

    overwrite_files(wpi_files, llvm_files)
    run_global_replacements(wpi_files)


def main():
    root, repo = setup_upstream_repo(
        "https://github.com/llvm/llvm-project", "llvmorg-14.0.6"
    )
    wpiutil = os.path.join(root, "wpiutil")

    patch_root = os.path.join(root, "upstream_utils/llvm_patches")
    # yapf: disable
    frontend_patches = [
        os.path.join(patch_root, "0001-Fix-spelling-language-errors.patch"),
        os.path.join(patch_root, "0002-Remove-StringRef-ArrayRef-and-Optional.patch"),
        os.path.join(patch_root, "0003-Wrap-std-min-max-calls-in-parens-for-Windows-warning.patch"),
        os.path.join(patch_root, "0004-Change-unique_function-storage-size.patch"),
        os.path.join(patch_root, "0005-Threading-updates.patch"),
        os.path.join(patch_root, "0006-ifdef-guard-safety.patch"),
        os.path.join(patch_root, "0007-Explicitly-use-std.patch"),
        os.path.join(patch_root, "0008-Remove-format_provider.patch"),
        os.path.join(patch_root, "0009-Add-compiler-warning-pragmas.patch"),
        os.path.join(patch_root, "0010-Remove-unused-functions.patch"),
        os.path.join(patch_root, "0011-Detemplatize-SmallVectorBase.patch"),
        os.path.join(patch_root, "0012-Add-vectors-to-raw_ostream.patch"),
        os.path.join(patch_root, "0013-Extra-collections-features.patch"),
        os.path.join(patch_root, "0014-EpochTracker-ABI-macro.patch"),
        os.path.join(patch_root, "0015-Delete-numbers-from-MathExtras.patch"),
        os.path.join(patch_root, "0016-Add-lerp-and-sgn.patch"),
        os.path.join(patch_root, "0017-Fixup-includes.patch"),
        os.path.join(patch_root, "0018-Use-std-is_trivially_copy_constructible.patch"),
        os.path.join(patch_root, "0019-Windows-support.patch"),
        os.path.join(patch_root, "0020-Prefer-fmtlib.patch"),
        os.path.join(patch_root, "0021-Prefer-wpi-s-fs.h.patch"),
        os.path.join(patch_root, "0022-Remove-unused-functions.patch"),
        os.path.join(patch_root, "0023-OS-specific-changes.patch"),
        os.path.join(patch_root, "0024-Use-SmallVector-for-UTF-conversion.patch"),
        os.path.join(patch_root, "0025-Prefer-to-use-static-pointers-in-raw_ostream.patch"),
        os.path.join(patch_root, "0026-constexpr-endian-byte-swap.patch"),
        os.path.join(patch_root, "0027-Copy-type-traits-from-STLExtras.h-into-PointerUnion..patch"),
        os.path.join(patch_root, "0028-Remove-StringMap-test-for-llvm-sort.patch"),

    ]
    # yapf: enable
    am_patches(repo, frontend_patches, use_threeway=True)

    overwrite_source(wpiutil, repo)
    overwrite_tests(wpiutil, repo)


if __name__ == "__main__":
    main()
