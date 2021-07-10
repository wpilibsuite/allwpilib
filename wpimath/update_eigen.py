#!/usr/bin/env python3

import os
import re
import subprocess

from upstream_utils import clone_repo, comment_out_invalid_includes, copy_to, walk_if


def eigen_inclusions(dp, f):
    """Returns true if the given file in the "Eigen" include directory of the
    Eigen git repo should be copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    abspath = os.path.join(dp, f)

    # Exclude build system
    if f == "CMakeLists.txt":
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
    abspath = os.path.join(dp, f)

    # Exclude build system and READMEs
    if f == "CMakeLists.txt" or "README" in f:
        return False

    # Include the AutoDiff and MatrixFunctions modules
    return "AutoDiff" in abspath or "MatrixFunctions" in abspath


def main():
    wpimath = os.getcwd()
    clone_repo("https://gitlab.com/libeigen/eigen.git", "3.3.9")
    repo = os.getcwd()

    # Copy Eigen headers into allwpilib
    os.chdir(os.path.join(repo, "Eigen"))
    files = walk_if(".", eigen_inclusions)
    dest = os.path.join(wpimath, "src/main/native/eigeninclude/Eigen")
    copy_to(files, dest)

    # Files moved to dest
    for i in range(len(files)):
        files[i] = os.path.join(dest, files[i])

    for f in files:
        comment_out_invalid_includes(f, [dest])

    # Copy unsupported headers into allwpilib
    os.chdir(os.path.join(repo, "unsupported"))
    files = walk_if(".", unsupported_inclusions)
    dest = os.path.join(wpimath, "src/main/native/eigeninclude/unsupported")
    copy_to(files, dest)

    # Files moved to dest
    for i in range(len(files)):
        files[i] = os.path.join(dest, files[i])

    for f in files:
        comment_out_invalid_includes(f, [dest])

    # Apply patches
    os.chdir(wpimath)
    subprocess.check_output(["git", "apply", "eigen-maybe-uninitialized.patch"])


if __name__ == "__main__":
    main()
