#!/usr/bin/env python3

import os
import shutil

from upstream_utils import (
    get_repo_root,
    clone_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    git_am,
)


def main():
    upstream_root = clone_repo("https://github.com/RobotLocomotion/drake", "v1.6.0")
    wpilib_root = get_repo_root()
    wpimath = os.path.join(wpilib_root, "wpimath")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in [
        "0001-Replace-Eigen-Dense-with-Eigen-Core.patch",
        "0002-Add-WPILIB_DLLEXPORT-to-DARE-function-declarations.patch",
    ]:
        git_am(os.path.join(wpilib_root, "upstream_utils/drake_patches", f))

    # Delete old install
    for d in [
        "src/main/native/thirdparty/drake/src",
        "src/main/native/thirdparty/drake/include",
        "src/test/native/cpp/drake",
        "src/test/native/include/drake",
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy drake source files into allwpilib
    src_files = walk_cwd_and_copy_if(
        lambda dp, f: f
        in ["drake_assert_and_throw.cc", "discrete_algebraic_riccati_equation.cc"],
        os.path.join(wpimath, "src/main/native/thirdparty/drake/src"),
    )

    # Copy drake header files into allwpilib
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: f
        in [
            "drake_assert.h",
            "drake_assertion_error.h",
            "is_approx_equal_abstol.h",
            "never_destroyed.h",
            "drake_copyable.h",
            "drake_throw.h",
            "discrete_algebraic_riccati_equation.h",
        ],
        os.path.join(wpimath, "src/main/native/thirdparty/drake/include/drake"),
    )

    # Copy drake test source files into allwpilib
    os.chdir(os.path.join(upstream_root, "math/test"))
    test_src_files = walk_cwd_and_copy_if(
        lambda dp, f: f == "discrete_algebraic_riccati_equation_test.cc",
        os.path.join(wpimath, "src/test/native/cpp/drake"),
    )
    os.chdir(upstream_root)

    # Copy drake test header files into allwpilib
    test_include_files = walk_cwd_and_copy_if(
        lambda dp, f: f == "eigen_matrix_compare.h",
        os.path.join(wpimath, "src/test/native/include/drake"),
    )

    for f in src_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/thirdparty/drake/include")]
        )
    for f in include_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/thirdparty/drake/include")]
        )
    for f in test_src_files:
        comment_out_invalid_includes(
            f,
            [
                os.path.join(wpimath, "src/main/native/thirdparty/drake/include"),
                os.path.join(wpimath, "src/test/native/include"),
            ],
        )
    for f in test_include_files:
        comment_out_invalid_includes(
            f,
            [
                os.path.join(wpimath, "src/main/native/thirdparty/drake/include"),
                os.path.join(wpimath, "src/test/native/include"),
            ],
        )


if __name__ == "__main__":
    main()
