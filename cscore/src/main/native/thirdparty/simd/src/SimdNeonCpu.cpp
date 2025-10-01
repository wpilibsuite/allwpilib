/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2022 Yermalayeu Ihar,
*               2022-2022 Fabien Spindler,
*               2022-2022 Souriya Trinh.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Simd/SimdEnable.h"
#include "Simd/SimdCpu.h"

#if defined(__GNUC__) && (defined(SIMD_ARM_ENABLE) || defined(SIMD_ARM64_ENABLE))
#include <fcntl.h>
#if !defined(__APPLE__)
#include <sys/auxv.h>
#if !defined(__FreeBSD__)
#include <asm/hwcap.h>
#endif
#endif
#endif

namespace Simd
{
#ifdef SIMD_NEON_ENABLE
    namespace Neon
    {
        SIMD_INLINE bool SupportedByCPU()
        {
#if defined(_MSC_VER)
            return true;
#elif defined(__GNUC__)
#if defined(SIMD_ARM64_ENABLE) || (TARGET_OS_IOS != 0) // Modification for iOS
            return true;
#else
            return Base::CheckBit(AT_HWCAP, HWCAP_NEON);
#endif
#else
#error Do not know how to detect NEON support!
#endif
        }

        bool GetEnable()
        {
            return SupportedByCPU();
        }
    }
#endif
}
