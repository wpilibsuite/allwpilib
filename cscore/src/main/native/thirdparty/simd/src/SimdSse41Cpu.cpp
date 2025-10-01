/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2022 Yermalayeu Ihar.
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

#if defined(_MSC_VER)
#include <windows.h>
#endif

namespace Simd
{
#ifdef SIMD_SSE41_ENABLE
    namespace Sse41
    {
        SIMD_INLINE bool SupportedByCPU()
        {
            return 
                Base::CheckBit(1, 0, Cpuid::Edx, Cpuid::SSE) &&
                Base::CheckBit(1, 0, Cpuid::Edx, Cpuid::SSE2) &&
                Base::CheckBit(1, 0, Cpuid::Ecx, Cpuid::SSE3) &&
                Base::CheckBit(1, 0, Cpuid::Ecx, Cpuid::SSSE3) &&
                Base::CheckBit(1, 0, Cpuid::Ecx, Cpuid::SSE41) &&
                Base::CheckBit(1, 0, Cpuid::Ecx, Cpuid::SSE42);
        }

        SIMD_INLINE bool SupportedByOS()
        {
#if defined(_MSC_VER)
            __try
            {
                int value = _mm_testz_si128(_mm_set1_epi8(0), _mm_set1_epi8(-1)); // try to execute of SSE41 instructions;
                uint32_t crc = _mm_crc32_u8(0, 1); // try to execute of SSE42 instructions;
                return true;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return false;
            }
#else
            return true;
#endif
        }

        bool GetEnable()
        {
            return SupportedByCPU() && SupportedByOS();
        }
    }
#endif
}
