#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, walk_cwd_and_copy_if


def copy_upstream_src(wpilib_root):
    wpical = os.path.join(wpilib_root, "wpical")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/mrcal_java/src",
        "src/main/native/thirdparty/mrcal_java/include",
    ]:
        shutil.rmtree(os.path.join(wpical, d), ignore_errors=True)

    os.chdir("src")
    files = walk_cwd_and_copy_if(
        lambda dp, f: f.endswith("mrcal_wrapper.h"),
        os.path.join(wpical, "src/main/native/thirdparty/mrcal_java/include"),
    )

    files += walk_cwd_and_copy_if(
        lambda dp, f: f.endswith("mrcal_wrapper.cpp"),
        os.path.join(wpical, "src/main/native/thirdparty/mrcal_java/src"),
    )

    for f in files:
        with open(f) as file:
            content = file.read()
            content = content.replace("#include <malloc.h>", "")
            content = content.replace(
                "suitesparse/cholmod_core.h", "suitesparse/cholmod.h"
            )
            content = content.replace(
                "// mrcal_point3_t *c_observations_point_pool = observations_point;",
                "mrcal_point3_t *c_observations_point_pool = observations_point;",
            )
            content = content.replace(
                "// mrcal_observation_point_triangulated_t *observations_point_triangulated =",
                "mrcal_observation_point_triangulated_t *observations_point_triangulated = NULL;",
            )
            content = content.replace(
                "// observations_point_triangulated,",
                "observations_point_triangulated,",
            )
            content = content.replace(
                "// 0, // hard-coded to 0", "0, // hard-coded to 0"
            )
            content = content.replace(
                "// observations_point_triangulated, -1,",
                "observations_point_triangulated, -1,",
            )
            content = content.replace(
                "c_observations_board_pool, &mrcal_lensmodel, c_imagersizes,",
                "c_observations_board_pool, c_observations_point_pool, &mrcal_lensmodel, c_imagersizes,",
            )
        with open(f, "w") as file:
            file.write(content)


def main():
    name = "mrcal_java"
    url = "https://github.com/PhotonVision/mrcal-java"
    tag = "5f9d3168ccf1ecdfca48da13ea07fffa47f95d00"

    mrcal_java = Lib(name, url, tag, copy_upstream_src)
    mrcal_java.main()


if __name__ == "__main__":
    main()
