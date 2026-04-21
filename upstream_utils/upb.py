#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import (
    Lib,
    comment_out_invalid_includes,
    copy_to,
    walk_cwd_and_copy_if,
)


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/upb/include",
        "src/main/native/thirdparty/upb/src",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    os.chdir(upstream_root / "third_party/utf8_range")
    copy_to(
        [Path("utf8_range.h")],
        wpiutil / "src/main/native/thirdparty/upb/include",
    )
    copy_to(
        [
            Path("utf8_range.c"),
            Path("utf8_range_sse.inc"),
            Path("utf8_range_neon.inc"),
        ],
        wpiutil / "src/main/native/thirdparty/upb/src",
    )
    # Change directory to upb to minimize the number of files to walk
    os.chdir(upstream_root / "upb")
    include_files = walk_cwd_and_copy_if(
        lambda dp, f: (
            len(dp.parts) >= 1
            and (
                dp.parts[0] == "base"
                or dp.parts[0] == "hash"
                or dp.parts[0] == "mem"
                or (
                    dp.parts[0] == "message"
                    and "compare" not in f
                    and "promote" not in f
                    and "accessors_split64" not in f
                )
                or dp.parts[0] == "mini_descriptor"
                or (dp.parts[0] == "mini_table" and "compat" not in f)
                or dp.parts[0] == "port"
                or dp.parts[0] == "reflection"
                or (
                    dp.parts[0] == "wire"
                    and "byte_size" not in f
                    and "decode_fast" not in dp.parts
                    and "test_util" not in dp.parts
                )
            )
            and "cmake" not in dp.parts
        )
        and (f.endswith(".h") or f.endswith(".inc"))
        or f == "generated_code_support.h",
        wpiutil / "src/main/native/thirdparty/upb/include/upb",
    )
    for f in include_files:
        comment_out_invalid_includes(
            f, [wpiutil / "src/main/native/thirdparty/upb/include"]
        )

    walk_cwd_and_copy_if(
        lambda dp, f: (
            len(dp.parts) >= 1
            and (
                dp.parts[0] == "base"
                or dp.parts[0] == "hash"
                or dp.parts[0] == "mem"
                or (
                    dp.parts[0] == "message"
                    and "compare" not in f
                    and "promote" not in f
                )
                or dp.parts[0] == "mini_descriptor"
                or (dp.parts[0] == "mini_table" and "compat" not in f)
                or dp.parts[0] == "port"
                or dp.parts[0] == "reflection"
                or (
                    dp.parts[0] == "wire"
                    and "byte_size" not in f
                    and "decode_fast" not in dp.parts
                )
            )
        )
        and f.endswith(".c")
        and not f == "descriptor.upb_minitable.c",
        wpiutil / "src/main/native/thirdparty/upb/src",
    )


def main():
    name = "upb"
    url = "https://github.com/protocolbuffers/protobuf"
    tag = "v32.0"

    upb = Lib(name, url, tag, copy_upstream_src)
    upb.main()


if __name__ == "__main__":
    main()
