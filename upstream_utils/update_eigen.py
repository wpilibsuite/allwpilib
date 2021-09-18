#!/usr/bin/env python3

import os
import re
import shutil

from upstream_utils import setup_upstream_repo, comment_out_invalid_includes, walk_cwd_and_copy_if, apply_patches


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
        return ("arch/AVX/" in abspath or "arch/Default" in abspath or
                "arch/NEON" in abspath or "arch/SSE" in abspath)

    # Include the following modules
    modules = [
        "Cholesky",
        "Core",
        "Eigenvalues",
        "Householder",
        "Jacobi",
        "LU",
        "QR",
        "SVD",
        "StlSupport",
        "misc",
        "plugins",
    ]
    modules_rgx = r"|".join("/" + m for m in modules)

    # "Std" matches StdDeque, StdList, and StdVector headers
    if re.search(modules_rgx, abspath) or "Std" in f:
        return True
    else:
        # Exclude all other modules
        return False


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

    # Include the AutoDiff and MatrixFunctions modules
    return "AutoDiff" in abspath or "MatrixFunctions" in abspath


def main():
    root, repo = setup_upstream_repo("https://gitlab.com/libeigen/eigen.git",
                                     "3.4.0")
    wpimath = os.path.join(root, "wpimath")

    # Delete old install
    for d in [
            "src/main/native/eigeninclude/Eigen",
            "src/main/native/eigeninclude/unsupported"
    ]:
        shutil.rmtree(os.path.join(wpimath, d), ignore_errors=True)

    # Copy Eigen headers into allwpilib
    eigen_files = walk_cwd_and_copy_if(
        eigen_inclusions, os.path.join(wpimath, "src/main/native/eigeninclude"))

    # Copy unsupported headers into allwpilib
    unsupported_files = walk_cwd_and_copy_if(
        unsupported_inclusions,
        os.path.join(wpimath, "src/main/native/eigeninclude"))

    for f in eigen_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/eigeninclude")])
    for f in unsupported_files:
        comment_out_invalid_includes(
            f, [os.path.join(wpimath, "src/main/native/eigeninclude")])

    apply_patches(root, ["upstream_utils/eigen-maybe-uninitialized.patch"])


if __name__ == "__main__":
    main()
