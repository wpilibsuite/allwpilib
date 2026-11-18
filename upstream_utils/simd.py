#!/usr/bin/env python3

import glob
import os
import re
import shutil
import subprocess

from upstream_utils import Lib, copy_to, walk_if

use_src_files = set(
    [
        "Simd*BgrToBgra.cpp",
        "Simd*BgrToGray.cpp",
        "Simd*BgrToHsl.cpp",
        "Simd*BgrToHsv.cpp",
        "Simd*BgrToRgb.cpp",
        "Simd*BgrToYuv.cpp",
        "Simd*BgrToYuvV2.cpp",
        "Simd*BgraToBgr.cpp",
        "Simd*BgraToGray.cpp",
        "Simd*BgraToYuv.cpp",
        "Simd*BgraToYuvV2.cpp",
        "Simd*Copy.cpp",
        "Simd*Cpu.cpp",
        "Simd*Deinterleave.cpp",
        "Simd*Fill.cpp",
        "Simd*GrayToBgr.cpp",
        "Simd*GrayToBgra.cpp",
        "Simd*GrayToY.cpp",
        "Simd*ImageLoad.cpp",
        "Simd*ImageLoadJpeg.cpp",
        "Simd*ImageSave.cpp",
        "Simd*ImageSaveJpeg.cpp",
        "Simd*Int16ToGray.cpp",
        "Simd*Interleave.cpp",
        "Simd*Performance.cpp",
        "Simd*Resizer.cpp",
        "Simd*ResizerArea.cpp",
        "Simd*ResizerBicubic.cpp",
        "Simd*ResizerBilinear.cpp",
        "Simd*ResizerNearest.cpp",
        "Simd*Thread.cpp",
        "Simd*UyvyToBgr.cpp",
        "Simd*UyvyToYuv.cpp",
        "Simd*YToGray.cpp",
        "Simd*YuvToBgr.cpp",
        "Simd*YuvToBgrV2.cpp",
        "Simd*YuvToBgra.cpp",
        "Simd*YuvToBgraV2.cpp",
        "Simd*YuvToHsl.cpp",
        "Simd*YuvToHsv.cpp",
        "Simd*YuvToHue.cpp",
        "Simd*YuvToUyvy.cpp",
        "Simd*YuvaToBgraV2.cpp",
        "SimdAlignment.h",
        "SimdAllocator.hpp",
        "SimdArray.h",
        "SimdAvx2.h",
        "SimdAvx512bw.h",
        "SimdBase.h",
        "SimdConfig.h",
        "SimdConst.h",
        "SimdConversion.h",
        "SimdCopy.h",
        "SimdCpu.h",
        "SimdDefs.h",
        "SimdDeinterleave.h",
        "SimdEmpty.h",
        "SimdEnable.h",
        "SimdExtract.h",
        "SimdGrayToY.h",
        "SimdImageLoad.h",
        "SimdImageLoadJpeg.h",
        "SimdImageMatcher.hpp",
        "SimdImageSave.h",
        "SimdImageSaveJpeg.h",
        "SimdInit.h",
        "SimdInterleave.h",
        "SimdLib.cpp",
        "SimdLoad.h",
        "SimdLoadBlock.h",
        "SimdLog.h",
        "SimdMath.h",
        "SimdMemory.h",
        "SimdMemoryStream.h",
        "SimdNeon.h",
        "SimdParallel.hpp",
        "SimdPerformance.h",
        "SimdPoint.hpp",
        "SimdPrefetch.h",
        "SimdRectangle.hpp",
        "SimdResizer.h",
        "SimdResizerCommon.h",
        "SimdSet.h",
        "SimdShuffle.h",
        "SimdSse41.h",
        "SimdStore.h",
        "SimdStream.h",
        "SimdTime.h",
        "SimdUnpack.h",
        "SimdUpdate.h",
        "SimdVersion.h",
        "SimdView.hpp",
        "SimdYuvToBgr.h",
    ]
)

use_include_files = set(
    [
        "SimdLib.h",
    ]
)

def dos2unix():
    for file_path in glob.glob("src/Simd/*"):
        with open(file_path, 'rb') as f:
            content = f.read()
        content = content.replace(b'\r\n', b'\n')
        with open(file_path, 'wb') as f:
            f.write(content)
    subprocess.run(["git", "commit", "-a", "-m", "Unix line endings"], stdout=subprocess.DEVNULL)

def remove_if0(file_path):
    with open(file_path, 'rt') as f:
        content = f.read()
    if_stack = []
    do_output = True
    output = []
    for line in content.split('\n'):
        m = re.match(r'\s*#if(.*)', line)
        if m is not None:
            if_stack.append(do_output)
            if re.match(r'\s+0\s*', m[1]):
                do_output = False
        elif re.match(r'\s*#endif\s*', line) and if_stack:
            if not do_output:
                line = ''
            do_output = if_stack.pop()
        if do_output:
            output.append(line)
    with open(file_path, 'wt') as f:
        f.write('\n'.join(output))

def copy_upstream_src(wpilib_root):
    upstream_root = os.path.abspath(".")
    cscore = os.path.join(wpilib_root, "cscore")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/simd/src",
        "src/main/native/thirdparty/simd/include",
    ]:
        shutil.rmtree(os.path.join(cscore, d), ignore_errors=True)

    # Copy simd source files into allwpilib
    src_files = [
        f[9:]
        for fn in use_src_files
        for f in glob.glob(os.path.join("src/Simd", fn))
    ]
    os.chdir(os.path.join(upstream_root, "src/Simd"))
    copy_to(src_files, os.path.join(cscore, "src/main/native/thirdparty/simd/src"))

    # Remove #if 0 blocks from source files
    for fn in glob.glob(os.path.join(cscore, "src/main/native/thirdparty/simd/src/*")):
        remove_if0(fn)

    # Copy simd header files into allwpilib
    include_files = [f for f in use_include_files]
    os.chdir(os.path.join(upstream_root, "src/Simd"))
    copy_to(
        include_files,
        os.path.join(cscore, "src/main/native/thirdparty/simd/include"),
    )

    # Remove #if 0 blocks from header files
    for fn in glob.glob(os.path.join(cscore, "src/main/native/thirdparty/simd/include/*")):
        remove_if0(fn)


def main():
    name = "simd"
    url = "https://github.com/ermig1979/Simd"
    tag = "cc66d28534f761116d845f0b90191a37751da64f"

    simd = Lib(name, url, tag, copy_upstream_src, pre_patch_hook=dos2unix)
    simd.main()


if __name__ == "__main__":
    main()
