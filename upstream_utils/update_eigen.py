#!/usr/bin/env python3

import os
import re
import shutil

from upstream_utils import (
    get_repo_root,
    clone_repo,
    comment_out_invalid_includes,
    walk_cwd_and_copy_if,
    git_am,
)


def eigen_inclusions(dp, f):
    """Returns true if the given file in the "Eigen" include directory of the
    Eigen git repo should be copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    if not dp.startswith("./Eigen"):
        return False

    abspath = os.path.join(dp, f)

    # Exclude NonMPL2.h since all non-MPL2 code will be excluded anyway
    if f == "NonMPL2.h":
        return False

    # Exclude BLAS support
    if f.endswith("_BLAS.h") or "blas" in f:
        return False

    # Exclude LAPACK support
    if f.endswith("_LAPACKE.h") or "lapack" in f:
        return False

    # Exclude MKL support
    if "MKL" in f:
        return False

    # Include architectures we care about
    if "Core/arch/" in abspath:
        return (
            "arch/AVX/" in abspath
            or "arch/Default" in abspath
            or "arch/NEON" in abspath
            or "arch/SSE" in abspath
        )

    # Include the following modules
    modules = [
        "Cholesky",
        "Core",
        "Eigenvalues",
        "Householder",
        "IterativeLinearSolvers",
        "Jacobi",
        "LU",
        "OrderingMethods",
        "QR",
        "SVD",
        "SparseCholesky",
        "SparseCore",
        "SparseLU",
        "SparseQR",
        "StlSupport",
        "misc",
        "plugins",
    ]
    modules_rgx = r"|".join("/" + m for m in modules)

    # "Std" matches StdDeque, StdList, and StdVector headers. Other modules are excluded.
    return bool(re.search(modules_rgx, abspath) or "Std" in f)


def unsupported_inclusions(dp, f):
    """Returns true if the given file in the "unsupported" include directory of
    the Eigen git repo should be copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    if not dp.startswith("./unsupported"):
        return False

    abspath = os.path.join(dp, f)

    # Exclude build system and READMEs
    if f == "CMakeLists.txt" or "README" in f:
        return False

    # Include the MatrixFunctions module
    return "MatrixFunctions" in abspath


def main():
    upstream_root = clone_repo("https://gitlab.com/libeigen/eigen.git", "3.4.0")
    wpilib_root = get_repo_root()
    wpimath = os.path.join(wpilib_root, "wpimath")

    # Apply patches to upstream Git repo
    os.chdir(upstream_root)
    for f in ["0001-Disable-warnings.patch"]:
        git_am(os.path.join(wpilib_root, "upstream_utils/eigen_patches", f))

    # Delete old install
    for d in [
        "src/main/native/thirdparty/eigen/include/Eigen",
        "src/main/native/thirdparty/eigen/include/unsupported",
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy Eigen headers into allwpilib
    eigen_files = walk_cwd_and_copy_if(
        eigen_inclusions,
        os.path.join(wpimath, "src/main/native/thirdparty/eigen/include"),
    )

    # Copy unsupported headers into allwpilib
    unsupported_files = walk_cwd_and_copy_if(
        unsupported_inclusions,
        os.path.join(wpimath, "src/main/native/thirdparty/eigen/include"),
    )

    for f in eigen_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/thirdparty/eigen/include")]
        )
    for f in unsupported_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/thirdparty/eigen/include")]
        )


if __name__ == "__main__":
    main()
