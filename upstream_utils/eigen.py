#!/usr/bin/env python3

import shutil
from pathlib import Path

from upstream_utils import (
    Lib,
    comment_out_invalid_includes,
    has_prefix,
    walk_cwd_and_copy_if,
)


def eigen_inclusions(dp: Path, f: str):
    """Returns true if the given file in the "Eigen" include directory of the
    Eigen git repo should be copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    if not has_prefix(dp, Path("Eigen")):
        return False

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

    # Include architectures we care about by filtering for Core/arch
    if "Core" in dp.parts and "arch" in dp.parts:
        return (
            "AVX" in dp.parts
            or "Default" in dp.parts
            or "NEON" in dp.parts
            or "SSE" in dp.parts
        )

    if (
        "StlSupport" in dp.parts
        and not f.endswith("StdList.h")
        and not f.endswith("StdDeque.h")
    ):
        return True

    # Include the following modules
    modules = [
        "Cholesky",
        "Core",
        "Dense",
        "Eigenvalues",
        "Geometry",
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
        "StdVector",
        "misc",
        "plugins",
    ]
    for m in modules:
        if m in dp.parts or f == m:
            return True
    return False


def unsupported_inclusions(dp: Path, f: str):
    """Returns true if the given file in the "unsupported" include directory of
    the Eigen git repo should be copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    if not has_prefix(dp, Path("unsupported")):
        return False

    abspath = dp / f

    # Exclude build system and READMEs
    if f == "CMakeLists.txt" or "README" in f:
        return False

    # Include the MatrixFunctions module
    return "MatrixFunctions" in abspath.parts


def copy_upstream_src(wpilib_root: Path):
    wpimath = wpilib_root / "wpimath"

    # Delete old install
    for d in ["src/main/native/thirdparty/eigen/include"]:
        shutil.rmtree(wpimath / d, ignore_errors=True)

    # Copy Eigen headers into allwpilib
    eigen_files = walk_cwd_and_copy_if(
        eigen_inclusions,
        wpimath / "src/main/native/thirdparty/eigen/include",
    )

    # Copy unsupported headers into allwpilib
    unsupported_files = walk_cwd_and_copy_if(
        unsupported_inclusions,
        wpimath / "src/main/native/thirdparty/eigen/include",
    )

    for f in eigen_files:
        comment_out_invalid_includes(
            f, [wpimath / "src/main/native/thirdparty/eigen/include"]
        )
    for f in unsupported_files:
        comment_out_invalid_includes(
            f, [wpimath / "src/main/native/thirdparty/eigen/include"]
        )

    shutil.copyfile(
        ".clang-format",
        wpimath / "src/main/native/thirdparty/eigen/include/.clang-format",
    )


def main():
    name = "eigen"
    url = "https://gitlab.com/libeigen/eigen.git"
    # master on 2025-05-18
    tag = "d81aa18f4dc56264b2cd7e2f230807d776a2d385"

    eigen = Lib(name, url, tag, copy_upstream_src)
    eigen.main()


if __name__ == "__main__":
    main()
