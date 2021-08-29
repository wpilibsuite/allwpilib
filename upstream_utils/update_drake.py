#!/usr/bin/env python3

import os
import shutil

from upstream_utils import setup_upstream_repo, comment_out_invalid_includes, walk_cwd_and_copy_if, apply_patches


def main():
    root, repo = setup_upstream_repo("https://github.com/RobotLocomotion/drake",
                                     "v0.33.0")
    wpimath = os.path.join(root, "wpimath")

    # Delete old install
    for d in [
            "src/main/native/cpp/drake", "src/main/native/include/drake",
            "src/test/native/cpp/drake", "src/test/native/include/drake"
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy drake source files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: f in
        ["drake_assert_and_throw.cc", "discrete_algebraic_riccati_equation.cc"],
        os.path.join(wpimath, "src/main/native/cpp/drake"))

    # Copy drake header files into allwpilib
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: f in [
            "drake_assert.h", "drake_assertion_error.h",
            "is_approx_equal_abstol.h", "never_destroyed.h", "drake_copyable.h",
            "drake_throw.h", "discrete_algebraic_riccati_equation.h"
        ], os.path.join(wpimath, "src/main/native/include/drake"))

    # Copy drake test source files into allwpilib
    os.chdir(os.path.join(repo, "math/test"))
    test_src_files = walk_cwd_and_copy_if(
        lambda dp, f: f == "discrete_algebraic_riccati_equation_test.cc",
        os.path.join(wpimath, "src/test/native/cpp/drake"))
    os.chdir(repo)

    # Copy drake test header files into allwpilib
    test_include_files = walk_cwd_and_copy_if(
        lambda dp, f: f == "eigen_matrix_compare.h",
        os.path.join(wpimath, "src/test/native/include/drake"))

    for f in src_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/include")])
    for f in include_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/include")])
    for f in test_src_files:
        comment_out_invalid_includes(f, [
            os.path.join(wpimath, "src/main/native/include"),
            os.path.join(wpimath, "src/test/native/include")
        ])
    for f in test_include_files:
        comment_out_invalid_includes(f, [
            os.path.join(wpimath, "src/main/native/include"),
            os.path.join(wpimath, "src/test/native/include")
        ])

    apply_patches(root, ["upstream_utils/drake-replace-dense-with-core.patch"])


if __name__ == "__main__":
    main()
