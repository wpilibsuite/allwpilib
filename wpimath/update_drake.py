#!/usr/bin/env python3

import os
import re
import subprocess

from upstream_utils import clone_repo, comment_out_invalid_includes, copy_to, walk_if


def drake_main_cpp_inclusions(dp, f):
    """Returns true if the given source file in the drake git repo should be
    copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    return f in [
        "drake_assert_and_throw.cc", "discrete_algebraic_riccati_equation.cc"
    ]


def drake_main_h_inclusions(dp, f):
    """Returns true if the given header file in the drake git repo should be
    copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    return f in [
        "drake_assert.h", "drake_assertion_error.h", "is_approx_equal_abstol.h",
        "never_destroyed.h", "drake_copyable.h", "drake_throw.h",
        "discrete_algebraic_riccati_equation.h"
    ]


def drake_test_cpp_inclusions(dp, f):
    """Returns true if the given source file in the drake git repo should be
    copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    return f == "discrete_algebraic_riccati_equation_test.cc"


def drake_test_h_inclusions(dp, f):
    """Returns true if the given header file in the drake git repo should be
    copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    return f == "eigen_matrix_compare.h"


def main():
    wpimath = os.getcwd()
    clone_repo("https://github.com/RobotLocomotion/drake", "v0.31.0")
    repo = os.getcwd()

    # Copy drake source files into allwpilib
    files = walk_if(".", drake_main_cpp_inclusions)
    dest = os.path.join(wpimath, "src/main/native/cpp/drake")
    copy_to(files, dest)

    for i in range(len(files)):
        # Files moved to dest
        files[i] = os.path.join(dest, files[i])

        # copy_to() renames .cc files to .cpp
        if files[i].endswith(".cc"):
            files[i] = os.path.splitext(files[i])[0] + ".cpp"

    for f in files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/include")])

    # Copy drake test source files into allwpilib
    os.chdir(os.path.join(repo, "math/test"))
    files = walk_if(".", drake_test_cpp_inclusions)
    dest = os.path.join(wpimath, "src/test/native/cpp/drake")
    copy_to(files, dest)
    os.chdir(repo)

    for i in range(len(files)):
        # Files moved to dest
        files[i] = os.path.join(dest, files[i])

        # copy_to() renames .cc files to .cpp
        if files[i].endswith(".cc"):
            files[i] = os.path.splitext(files[i])[0] + ".cpp"

    for f in files:
        comment_out_invalid_includes(f, [
            os.path.join(wpimath, "src/main/native/include"),
            os.path.join(wpimath, "src/test/native/include")
        ])

    # Copy drake header files into allwpilib
    files = walk_if(".", drake_main_h_inclusions)
    dest = os.path.join(wpimath, "src/main/native/include/drake")
    copy_to(files, dest)

    # Files moved to dest
    for i in range(len(files)):
        files[i] = os.path.join(dest, files[i])

    for f in files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/include")])

    # Copy drake test header files into allwpilib
    files = walk_if(".", drake_test_h_inclusions)
    dest = os.path.join(wpimath, "src/test/native/include/drake")
    copy_to(files, dest)

    # Files moved to dest
    for i in range(len(files)):
        files[i] = os.path.join(dest, files[i])

    for f in files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/include")])

    # Apply patches
    os.chdir(wpimath)
    subprocess.check_output(["git", "apply", "drake-dare-qrn.patch"])
    subprocess.check_output(["git", "apply", "drake-dare-qrn-tests.patch"])
    subprocess.check_output(
        ["git", "apply", "drake-replace-noreturn-attributes.patch"])
    subprocess.check_output(
        ["git", "apply", "drake-replace-dense-with-core.patch"])


if __name__ == "__main__":
    main()
