/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2025 Yermalayeu Ihar,
*               2014-2018 Antonenka Mikhail,
*               2018-2018 Radchenko Andrey,
*               2019-2019 Facundo Galan.
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
#include "Simd/SimdConfig.h"

#ifndef SIMD_LIB_CPP
#define SIMD_LIB_CPP
#endif

#if defined(_WIN32) && !defined(SIMD_STATIC)

#define SIMD_EXPORTS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
    switch (dwReasonForCall)
    {
    case DLL_PROCESS_DETACH:
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return TRUE;
    }
    return TRUE;
}
#endif//_WIN32

#include "Simd/SimdLib.h"

#include "Simd/SimdMemory.h"
#include "Simd/SimdCpu.h"
#include "Simd/SimdEnable.h"
#include "Simd/SimdAlignment.h"
#include "Simd/SimdConst.h"
#include "Simd/SimdLog.h"
#include "Simd/SimdPerformance.h"
#include "Simd/SimdEmpty.h"

#include "Simd/SimdImageLoad.h"
#include "Simd/SimdImageSave.h"
#include "Simd/SimdResizer.h"

#include "Simd/SimdBase.h"
#include "Simd/SimdSse41.h"
#include "Simd/SimdAvx2.h"
#include "Simd/SimdAvx512bw.h"
#include "Simd/SimdNeon.h"

#if !defined(SIMD_VERSION)
#include "Simd/SimdVersion.h"
#endif

namespace Simd
{
    const size_t ALIGNMENT = GetAlignment();
}

SIMD_API const char * SimdVersion()
{
    return SIMD_VERSION;
}

using namespace Simd;

SIMD_API const char* SimdCpuDesc(SimdCpuDescType type)
{
    switch (type)
    {
    case SimdCpuDescModel: return Cpu::CPU_MODEL.c_str();
    default:
        return NULL;
    }
}

SIMD_API uint64_t SimdCpuInfo(SimdCpuInfoType type)
{
    switch (type)
    {
    case SimdCpuInfoSockets: return Cpu::SOCKET_NUMBER;
    case SimdCpuInfoCores: return Cpu::CORE_NUMBER;
    case SimdCpuInfoThreads: return Cpu::THREAD_NUMBER;
    case SimdCpuInfoCacheL1: return Cpu::L1_CACHE_SIZE;
    case SimdCpuInfoCacheL2: return Cpu::L2_CACHE_SIZE;
    case SimdCpuInfoCacheL3: return Cpu::L3_CACHE_SIZE;
    case SimdCpuInfoRam: return Cpu::RAM_SIZE;
#ifdef SIMD_SSE41_ENABLE
    case SimdCpuInfoSse41: return Sse41::Enable ? 1 : 0;
#endif
#ifdef SIMD_AVX2_ENABLE
    case SimdCpuInfoAvx2: return Avx2::Enable ? 1 : 0;
#endif
#ifdef SIMD_AVX512BW_ENABLE
    case SimdCpuInfoAvx512bw: return Avx512bw::Enable ? 1 : 0;
#endif
#ifdef SIMD_AVX512VNNI_ENABLE
    case SimdCpuInfoAvx512vnni: return Avx512vnni::Enable ? 1 : 0;
#endif
#ifdef SIMD_AMXBF16_ENABLE
    case SimdCpuInfoAmxBf16: return AmxBf16::Enable ? 1 : 0;
#endif
#ifdef SIMD_NEON_ENABLE
    case SimdCpuInfoNeon: return Neon::Enable ? 1 : 0;
#endif
    case SimdCpuInfoCurrentFrequency: return Base::CpuCurrentFrequency();
    default:
        return 0;
    }
}

SIMD_API const char * SimdPerformanceStatistic()
{
#if defined(SIMD_PERFORMANCE_STATISTIC) && (defined(NDEBUG) || defined(SIMD_PERF_STAT_IN_DEBUG))
    return Base::PerformanceMeasurerStorage::s_storage.PerformanceStatistic();
#else
    return "";
#endif
}

SIMD_API void * SimdAllocate(size_t size, size_t align)
{
    return Allocate(size, align);
}

SIMD_API void SimdFree(void * ptr)
{
    Free(ptr);
}

SIMD_API size_t SimdAlign(size_t size, size_t align)
{
    return AlignHi(size, align);
}

SIMD_API size_t SimdAlignment()
{
    return Simd::ALIGNMENT;
}

SIMD_API void SimdRelease(void * context)
{
    delete (Deletable*)context;
}

SIMD_API size_t SimdGetThreadNumber()
{
    return Base::GetThreadNumber();
}

SIMD_API void SimdSetThreadNumber(size_t threadNumber)
{
    Base::SetThreadNumber(threadNumber);
}

SIMD_API SimdBool SimdGetFastMode()
{
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable)
        return Sse41::GetFastMode();
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable)
        return Neon::GetFastMode();
    else
#endif
        return SimdFalse;
}

SIMD_API void SimdSetFastMode(SimdBool value)
{
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable)
        Sse41::SetFastMode(value);
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable)
        Neon::SetFastMode(value);
#endif
}

SIMD_API void SimdBgraToBgr(const uint8_t * bgra, size_t width, size_t height, size_t bgraStride, uint8_t * bgr, size_t bgrStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToBgr(bgra, width, height, bgraStride, bgr, bgrStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::F)
        Avx2::BgraToBgr(bgra, width, height, bgraStride, bgr, bgrStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::BgraToBgr(bgra, width, height, bgraStride, bgr, bgrStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgraToBgr(bgra, width, height, bgraStride, bgr, bgrStride);
    else
#endif
        Base::BgraToBgr(bgra, width, height, bgraStride, bgr, bgrStride);
}

SIMD_API void SimdBgraToGray(const uint8_t *bgra, size_t width, size_t height, size_t bgraStride, uint8_t *gray, size_t grayStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToGray(bgra, width, height, bgraStride, gray, grayStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::BgraToGray(bgra, width, height, bgraStride, gray, grayStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::BgraToGray(bgra, width, height, bgraStride, gray, grayStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::HA)
        Neon::BgraToGray(bgra, width, height, bgraStride, gray, grayStride);
    else
#endif
        Base::BgraToGray(bgra, width, height, bgraStride, gray, grayStride);
}

SIMD_API void SimdBgraToRgb(const uint8_t* bgra, size_t width, size_t height, size_t bgraStride, uint8_t* rgb, size_t rgbStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToRgb(bgra, width, height, bgraStride, rgb, rgbStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::F)
        Avx2::BgraToRgb(bgra, width, height, bgraStride, rgb, rgbStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::BgraToRgb(bgra, width, height, bgraStride, rgb, rgbStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgraToRgb(bgra, width, height, bgraStride, rgb, rgbStride);
    else
#endif
        Base::BgraToRgb(bgra, width, height, bgraStride, rgb, rgbStride);
}

SIMD_API void SimdBgraToRgba(const uint8_t* bgra, size_t width, size_t height, size_t bgraStride, uint8_t* rgba, size_t rgbaStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToRgba(bgra, width, height, bgraStride, rgba, rgbaStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::BgraToRgba(bgra, width, height, bgraStride, rgba, rgbaStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::BgraToRgba(bgra, width, height, bgraStride, rgba, rgbaStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgraToRgba(bgra, width, height, bgraStride, rgba, rgbaStride);
    else
#endif
        Base::BgraToRgba(bgra, width, height, bgraStride, rgba, rgbaStride);
}

SIMD_API void SimdBgraToYuv420pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height,
    uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToYuv420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::BgraToYuv420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::BgraToYuv420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::BgraToYuv420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
        Base::BgraToYuv420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
}

SIMD_API void SimdBgraToYuv422pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height,
    uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToYuv422pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::BgraToYuv422pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::BgraToYuv422pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::BgraToYuv422pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
        Base::BgraToYuv422pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
}

SIMD_API void SimdBgraToYuv444pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height, 
    uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToYuv444pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::BgraToYuv444pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::BgraToYuv444pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgraToYuv444pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
        Base::BgraToYuv444pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
}

SIMD_API void SimdBgraToYuva420pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height,
    uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, uint8_t* a, size_t aStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgraToYuva420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::BgraToYuva420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::BgraToYuva420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::BgraToYuva420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride, yuvType);
    else
#endif
        Base::BgraToYuva420pV2(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride, yuvType);
}

SIMD_API void SimdBgrToBgra(const uint8_t *bgr, size_t width, size_t height, size_t bgrStride, uint8_t *bgra, size_t bgraStride, uint8_t alpha)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgrToBgra(bgr, width, height, bgrStride, bgra, bgraStride, alpha);
    else
#endif
#if defined(SIMD_AVX2_ENABLE) && !defined(SIMD_CLANG_AVX2_BGR_TO_BGRA_ERROR)
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::BgrToBgra(bgr, width, height, bgrStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::BgrToBgra(bgr, width, height, bgrStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgrToBgra(bgr, width, height, bgrStride, bgra, bgraStride, alpha);
    else
#endif
        Base::BgrToBgra(bgr, width, height, bgrStride, bgra, bgraStride, alpha);
}

SIMD_API void SimdBgr48pToBgra32(const uint8_t * blue, size_t blueStride, size_t width, size_t height,
    const uint8_t * green, size_t greenStride, const uint8_t * red, size_t redStride, uint8_t * bgra, size_t bgraStride, uint8_t alpha)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Bgr48pToBgra32(blue, blueStride, width, height, green, greenStride, red, redStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::HA)
        Avx2::Bgr48pToBgra32(blue, blueStride, width, height, green, greenStride, red, redStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::HA)
        Sse41::Bgr48pToBgra32(blue, blueStride, width, height, green, greenStride, red, redStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::Bgr48pToBgra32(blue, blueStride, width, height, green, greenStride, red, redStride, bgra, bgraStride, alpha);
    else
#endif
        Base::Bgr48pToBgra32(blue, blueStride, width, height, green, greenStride, red, redStride, bgra, bgraStride, alpha);
}

SIMD_API void SimdBgrToGray(const uint8_t *bgr, size_t width, size_t height, size_t bgrStride, uint8_t *gray, size_t grayStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgrToGray(bgr, width, height, bgrStride, gray, grayStride);
    else
#endif
#if defined(SIMD_AVX2_ENABLE) && !defined(SIMD_CLANG_AVX2_BGR_TO_BGRA_ERROR)
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::BgrToGray(bgr, width, height, bgrStride, gray, grayStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::BgrToGray(bgr, width, height, bgrStride, gray, grayStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgrToGray(bgr, width, height, bgrStride, gray, grayStride);
    else
#endif
        Base::BgrToGray(bgr, width, height, bgrStride, gray, grayStride);
}

SIMD_API void SimdBgrToHsl(const uint8_t * bgr, size_t width, size_t height, size_t bgrStride, uint8_t * hsl, size_t hslStride)
{
    SIMD_EMPTY();
    Base::BgrToHsl(bgr, width, height, bgrStride, hsl, hslStride);
}

SIMD_API void SimdBgrToHsv(const uint8_t * bgr, size_t width, size_t height, size_t bgrStride, uint8_t * hsv, size_t hsvStride)
{
    SIMD_EMPTY();
    Base::BgrToHsv(bgr, width, height, bgrStride, hsv, hsvStride);
}

SIMD_API void SimdBgrToRgb(const uint8_t *bgr, size_t width, size_t height, size_t bgrStride, uint8_t * rgb, size_t rgbStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgrToRgb(bgr, width, height, bgrStride, rgb, rgbStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::BgrToRgb(bgr, width, height, bgrStride, rgb, rgbStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::BgrToRgb(bgr, width, height, bgrStride, rgb, rgbStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgrToRgb(bgr, width, height, bgrStride, rgb, rgbStride);
    else
#endif
        Base::BgrToRgb(bgr, width, height, bgrStride, rgb, rgbStride);
}

SIMD_API void SimdBgrToYuv420pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height,
    uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgrToYuv420pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::BgrToYuv420pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::BgrToYuv420pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::BgrToYuv420pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
        Base::BgrToYuv420pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
}

SIMD_API void SimdBgrToYuv422pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height,
    uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgrToYuv422pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::BgrToYuv422pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::BgrToYuv422pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::BgrToYuv422pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
        Base::BgrToYuv422pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
}

SIMD_API void SimdBgrToYuv444pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height,
    uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::BgrToYuv444pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::BgrToYuv444pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::BgrToYuv444pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::BgrToYuv444pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
    else
#endif
        Base::BgrToYuv444pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
}

SIMD_API void SimdCopy(const uint8_t * src, size_t srcStride, size_t width, size_t height, size_t pixelSize, uint8_t * dst, size_t dstStride)
{
    SIMD_EMPTY();
    Base::Copy(src, srcStride, width, height, pixelSize, dst, dstStride);
}

SIMD_API void SimdCopyFrame(const uint8_t * src, size_t srcStride, size_t width, size_t height, size_t pixelSize,
                           size_t frameLeft, size_t frameTop, size_t frameRight, size_t frameBottom, uint8_t * dst, size_t dstStride)
{
    SIMD_EMPTY();
    Base::CopyFrame(src, srcStride, width, height, pixelSize, frameLeft, frameTop, frameRight, frameBottom, dst, dstStride);
}

SIMD_API void SimdDeinterleaveUv(const uint8_t * uv, size_t uvStride, size_t width, size_t height,
                    uint8_t * u, size_t uStride, uint8_t * v, size_t vStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AMXBF16_ENABLE
    if (AmxBf16::Enable)
        AmxBf16::DeinterleaveUv(uv, uvStride, width, height, u, uStride, v, vStride);
    else
#endif
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::DeinterleaveUv(uv, uvStride, width, height, u, uStride, v, vStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::DeinterleaveUv(uv, uvStride, width, height, u, uStride, v, vStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::DeinterleaveUv(uv, uvStride, width, height, u, uStride, v, vStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::DeinterleaveUv(uv, uvStride, width, height, u, uStride, v, vStride);
    else
#endif
        Base::DeinterleaveUv(uv, uvStride, width, height, u, uStride, v, vStride);
}

SIMD_API void SimdDeinterleaveBgr(const uint8_t * bgr, size_t bgrStride, size_t width, size_t height,
    uint8_t * b, size_t bStride, uint8_t * g, size_t gStride, uint8_t * r, size_t rStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AMXBF16_ENABLE
    if (AmxBf16::Enable)
        AmxBf16::DeinterleaveBgr(bgr, bgrStride, width, height, b, bStride, g, gStride, r, rStride);
    else
#endif
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::DeinterleaveBgr(bgr, bgrStride, width, height, b, bStride, g, gStride, r, rStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::DeinterleaveBgr(bgr, bgrStride, width, height, b, bStride, g, gStride, r, rStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::DeinterleaveBgr(bgr, bgrStride, width, height, b, bStride, g, gStride, r, rStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::DeinterleaveBgr(bgr, bgrStride, width, height, b, bStride, g, gStride, r, rStride);
    else
#endif
        Base::DeinterleaveBgr(bgr, bgrStride, width, height, b, bStride, g, gStride, r, rStride);
}

SIMD_API void SimdDeinterleaveBgra(const uint8_t * bgra, size_t bgraStride, size_t width, size_t height,
    uint8_t * b, size_t bStride, uint8_t * g, size_t gStride, uint8_t * r, size_t rStride, uint8_t * a, size_t aStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AMXBF16_ENABLE
    if (AmxBf16::Enable)
        AmxBf16::DeinterleaveBgra(bgra, bgraStride, width, height, b, bStride, g, gStride, r, rStride, a, aStride);
    else
#endif
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::DeinterleaveBgra(bgra, bgraStride, width, height, b, bStride, g, gStride, r, rStride, a, aStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::DeinterleaveBgra(bgra, bgraStride, width, height, b, bStride, g, gStride, r, rStride, a, aStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::DeinterleaveBgra(bgra, bgraStride, width, height, b, bStride, g, gStride, r, rStride, a, aStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::DeinterleaveBgra(bgra, bgraStride, width, height, b, bStride, g, gStride, r, rStride, a, aStride);
    else
#endif
        Base::DeinterleaveBgra(bgra, bgraStride, width, height, b, bStride, g, gStride, r, rStride, a, aStride);
}

SIMD_API void SimdFill(uint8_t * dst, size_t stride, size_t width, size_t height, size_t pixelSize, uint8_t value)
{
    SIMD_EMPTY();
    Base::Fill(dst, stride, width, height, pixelSize, value);
}

SIMD_API void SimdFillFrame(uint8_t * dst, size_t stride, size_t width, size_t height, size_t pixelSize,
                           size_t frameLeft, size_t frameTop, size_t frameRight, size_t frameBottom, uint8_t value)
{
    SIMD_EMPTY();
    Base::FillFrame(dst, stride, width, height, pixelSize, frameLeft, frameTop, frameRight, frameBottom, value);
}

SIMD_API void SimdFillBgr(uint8_t * dst, size_t stride, size_t width, size_t height, uint8_t blue, uint8_t green, uint8_t red)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::FillBgr(dst, stride, width, height, blue, green, red);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::FillBgr(dst, stride, width, height, blue, green, red);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::FillBgr(dst, stride, width, height, blue, green, red);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::FillBgr(dst, stride, width, height, blue, green, red);
    else
#endif
        Base::FillBgr(dst, stride, width, height, blue, green, red);
}

SIMD_API void SimdFillBgra(uint8_t * dst, size_t stride, size_t width, size_t height, uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::FillBgra(dst, stride, width, height, blue, green, red, alpha);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::F)
        Avx2::FillBgra(dst, stride, width, height, blue, green, red, alpha);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::F)
        Sse41::FillBgra(dst, stride, width, height, blue, green, red, alpha);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::F)
        Neon::FillBgra(dst, stride, width, height, blue, green, red, alpha);
    else
#endif
        Base::FillBgra(dst, stride, width, height, blue, green, red, alpha);
}

SIMD_API void SimdFillPixel(uint8_t * dst, size_t stride, size_t width, size_t height, const uint8_t * pixel, size_t pixelSize)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::FillPixel(dst, stride, width, height, pixel, pixelSize);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::FillPixel(dst, stride, width, height, pixel, pixelSize);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::FillPixel(dst, stride, width, height, pixel, pixelSize);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::FillPixel(dst, stride, width, height, pixel, pixelSize);
    else
#endif
        Base::FillPixel(dst, stride, width, height, pixel, pixelSize);
}

SIMD_API void SimdGrayToBgr(const uint8_t * gray, size_t width, size_t height, size_t grayStride, uint8_t * bgr, size_t bgrStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::GrayToBgr(gray, width, height, grayStride, bgr, bgrStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::GrayToBgr(gray, width, height, grayStride, bgr, bgrStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::GrayToBgr(gray, width, height, grayStride, bgr, bgrStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::GrayToBgr(gray, width, height, grayStride, bgr, bgrStride);
    else
#endif
        Base::GrayToBgr(gray, width, height, grayStride, bgr, bgrStride);
}

SIMD_API void SimdGrayToBgra(const uint8_t * gray, size_t width, size_t height, size_t grayStride, uint8_t * bgra, size_t bgraStride, uint8_t alpha)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
    else
#endif
        Base::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
}

SIMD_API void SimdGrayToY(const uint8_t * gray, size_t grayStride, size_t width, size_t height, uint8_t * y, size_t yStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::GrayToY(gray, grayStride, width, height, y, yStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::GrayToY(gray, grayStride, width, height, y, yStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::GrayToY(gray, grayStride, width, height, y, yStride);
    else
#endif
//#ifdef SIMD_NEON_ENABLE
//    if (Neon::Enable && width >= Neon::A)
//        Neon::GrayToY(gray, grayStride, width, height, y, yStride);
//    else
//#endif
        Base::GrayToY(gray, grayStride, width, height, y, yStride);
}

SIMD_API uint8_t* SimdImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size)
{
    SIMD_EMPTY();
    const static Simd::ImageSaveToMemoryPtr imageSaveToMemory = SIMD_FUNC4(ImageSaveToMemory, SIMD_AVX512BW_FUNC, SIMD_AVX2_FUNC, SIMD_SSE41_FUNC, SIMD_NEON_FUNC);

    return imageSaveToMemory(src, stride, width, height, format, file, quality, size);
}

SIMD_API uint8_t* SimdNv12SaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size)
{
    SIMD_EMPTY();
    typedef uint8_t*(*SimdNv12SaveAsJpegToMemoryPtr) (const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);
    const static SimdNv12SaveAsJpegToMemoryPtr simdNv12SaveAsJpegToMemory = SIMD_FUNC4(Nv12SaveAsJpegToMemory, SIMD_AVX512BW_FUNC, SIMD_AVX2_FUNC, SIMD_SSE41_FUNC, SIMD_NEON_FUNC);

    return simdNv12SaveAsJpegToMemory(y, yStride, uv, uvStride, width, height, yuvType, quality, size);
}

SIMD_API uint8_t* SimdYuv420pSaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride, 
    size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size)
{
    SIMD_EMPTY();
    typedef uint8_t* (*SimdYuv420pSaveAsJpegToMemoryPtr) (const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, 
        const uint8_t* v, size_t vStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);
    const static SimdYuv420pSaveAsJpegToMemoryPtr simdYuv420pSaveAsJpegToMemory = SIMD_FUNC4(Yuv420pSaveAsJpegToMemory, SIMD_AVX512BW_FUNC, SIMD_AVX2_FUNC, SIMD_SSE41_FUNC, SIMD_NEON_FUNC);

    return simdYuv420pSaveAsJpegToMemory(y, yStride, u, uStride, v, vStride, width, height, yuvType, quality, size);
}

SIMD_API uint8_t* SimdImageLoadFromMemory(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format)
{
    SIMD_EMPTY();
    const static Simd::ImageLoadFromMemoryPtr imageLoadFromMemory = SIMD_FUNC4(ImageLoadFromMemory, SIMD_AVX512BW_FUNC, SIMD_AVX2_FUNC, SIMD_SSE41_FUNC, SIMD_NEON_FUNC);

    return imageLoadFromMemory(data, size, stride, width, height, format);
}

SIMD_API void SimdInt16ToGray(const uint8_t * src, size_t width, size_t height, size_t srcStride, uint8_t * dst, size_t dstStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Int16ToGray(src, width, height, srcStride, dst, dstStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::Int16ToGray(src, width, height, srcStride, dst, dstStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::Int16ToGray(src, width, height, srcStride, dst, dstStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::HA)
        Neon::Int16ToGray(src, width, height, srcStride, dst, dstStride);
    else
#endif
        Base::Int16ToGray(src, width, height, srcStride, dst, dstStride);
}

SIMD_API void * SimdResizerInit(size_t srcX, size_t srcY, size_t dstX, size_t dstY, size_t channels, SimdResizeChannelType type, SimdResizeMethodType method)
{
    SIMD_EMPTY();
    typedef void*(*SimdResizerInitPtr) (size_t srcX, size_t srcY, size_t dstX, size_t dstY, size_t channels, SimdResizeChannelType type, SimdResizeMethodType method);
    const static SimdResizerInitPtr simdResizerInit = SIMD_FUNC4(ResizerInit, SIMD_AVX512BW_FUNC, SIMD_AVX2_FUNC, SIMD_SSE41_FUNC, SIMD_NEON_FUNC);

    return simdResizerInit(srcX, srcY, dstX, dstY, channels, type, method);
}

SIMD_API void SimdResizerRun(const void * resizer, const uint8_t * src, size_t srcStride, uint8_t * dst, size_t dstStride)
{
    SIMD_EMPTY();
    ((Resizer*)resizer)->Run(src, srcStride, dst, dstStride);
}

SIMD_API void SimdRgbToBgra(const uint8_t* rgb, size_t width, size_t height, size_t rgbStride, uint8_t* bgra, size_t bgraStride, uint8_t alpha)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::RgbToBgra(rgb, width, height, rgbStride, bgra, bgraStride, alpha);
    else
#endif
#if defined(SIMD_AVX2_ENABLE) && !defined(SIMD_CLANG_AVX2_BGR_TO_BGRA_ERROR)
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::RgbToBgra(rgb, width, height, rgbStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::RgbToBgra(rgb, width, height, rgbStride, bgra, bgraStride, alpha);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::RgbToBgra(rgb, width, height, rgbStride, bgra, bgraStride, alpha);
    else
#endif
        Base::RgbToBgra(rgb, width, height, rgbStride, bgra, bgraStride, alpha);
}

SIMD_API void SimdRgbToGray(const uint8_t* rgb, size_t width, size_t height, size_t rgbStride, uint8_t* gray, size_t grayStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::RgbToGray(rgb, width, height, rgbStride, gray, grayStride);
    else
#endif
#if defined(SIMD_AVX2_ENABLE) && !defined(SIMD_CLANG_AVX2_BGR_TO_BGRA_ERROR)
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::RgbToGray(rgb, width, height, rgbStride, gray, grayStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::RgbToGray(rgb, width, height, rgbStride, gray, grayStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::RgbToGray(rgb, width, height, rgbStride, gray, grayStride);
    else
#endif
        Base::RgbToGray(rgb, width, height, rgbStride, gray, grayStride);
}

SIMD_API void SimdRgbaToGray(const uint8_t* rgba, size_t width, size_t height, size_t rgbaStride, uint8_t* gray, size_t grayStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::RgbaToGray(rgba, width, height, rgbaStride, gray, grayStride);
    else
#endif
#if defined(SIMD_AVX2_ENABLE)
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::RgbaToGray(rgba, width, height, rgbaStride, gray, grayStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::RgbaToGray(rgba, width, height, rgbaStride, gray, grayStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::RgbaToGray(rgba, width, height, rgbaStride, gray, grayStride);
    else
#endif
        Base::RgbaToGray(rgba, width, height, rgbaStride, gray, grayStride);
}

SIMD_API void SimdUyvy422ToBgr(const uint8_t* uyvy, size_t uyvyStride, size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Uyvy422ToBgr(uyvy, uyvyStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Uyvy422ToBgr(uyvy, uyvyStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Uyvy422ToBgr(uyvy, uyvyStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Uyvy422ToBgr(uyvy, uyvyStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
        Base::Uyvy422ToBgr(uyvy, uyvyStride, width, height, bgr, bgrStride, yuvType);
}

SIMD_API void SimdUyvy422ToYuv420p(const uint8_t* uyvy, size_t uyvyStride, size_t width, size_t height, uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Uyvy422ToYuv420p(uyvy, uyvyStride, width, height, y, yStride, u, uStride, v, vStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Uyvy422ToYuv420p(uyvy, uyvyStride, width, height, y, yStride, u, uStride, v, vStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Uyvy422ToYuv420p(uyvy, uyvyStride, width, height, y, yStride, u, uStride, v, vStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Uyvy422ToYuv420p(uyvy, uyvyStride, width, height, y, yStride, u, uStride, v, vStride);
    else
#endif
        Base::Uyvy422ToYuv420p(uyvy, uyvyStride, width, height, y, yStride, u, uStride, v, vStride);
}

SIMD_API void SimdYToGray(const uint8_t* y, size_t yStride, size_t width, size_t height, uint8_t* gray, size_t grayStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::YToGray(y, yStride, width, height, gray, grayStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::YToGray(y, yStride, width, height, gray, grayStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::YToGray(y, yStride, width, height, gray, grayStride);
    else
#endif
//#ifdef SIMD_NEON_ENABLE
//    if (Neon::Enable && width >= Neon::A)
//        Neon::YToGray(y, yStride, width, height, gray, grayStride);
//    else
//#endif
        Base::YToGray(y, yStride, width, height, gray, grayStride);
}

SIMD_API void SimdYuva420pToBgraV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    const uint8_t* a, size_t aStride, size_t width, size_t height, uint8_t* bgra, size_t bgraStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuva420pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuva420pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuva420pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuva420pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
        Base::Yuva420pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
}

SIMD_API void SimdYuva422pToBgraV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    const uint8_t* a, size_t aStride, size_t width, size_t height, uint8_t* bgra, size_t bgraStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuva422pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuva422pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuva422pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuva422pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
        Base::Yuva422pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
}

SIMD_API void SimdYuva444pToBgraV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    const uint8_t* a, size_t aStride, size_t width, size_t height, uint8_t* bgra, size_t bgraStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuva444pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::Yuva444pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::Yuva444pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::Yuva444pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
    else
#endif
        Base::Yuva444pToBgraV2(y, yStride, u, uStride, v, vStride, a, aStride, width, height, bgra, bgraStride, yuvType);
}

SIMD_API void SimdYuv420pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv420pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv420pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv420pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv420pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
        Base::Yuv420pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
}

SIMD_API void SimdYuv422pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv422pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv422pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv422pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv422pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
        Base::Yuv422pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
}

SIMD_API void SimdYuv444pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv444pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::Yuv444pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::Yuv444pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::Yuv444pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
    else
#endif
        Base::Yuv444pToBgrV2(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
}

SIMD_API void SimdYuv420pToBgraV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* bgra, size_t bgraStride, uint8_t alpha, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv420pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv420pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv420pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv420pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
        Base::Yuv420pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
}

SIMD_API void SimdYuv422pToBgraV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* bgra, size_t bgraStride, uint8_t alpha, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv422pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv422pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv422pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv422pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
        Base::Yuv422pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
}

SIMD_API void SimdYuv444pToBgraV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* bgra, size_t bgraStride, uint8_t alpha, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv444pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::Yuv444pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::Yuv444pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::Yuv444pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
    else
#endif
        Base::Yuv444pToBgraV2(y, yStride, u, uStride, v, vStride, width, height, bgra, bgraStride, alpha, yuvType);
}

SIMD_API void SimdYuv444pToHsl(const uint8_t * y, size_t yStride, const uint8_t * u, size_t uStride, const uint8_t * v, size_t vStride,
                               size_t width, size_t height, uint8_t * hsl, size_t hslStride)
{
    SIMD_EMPTY();
    Base::Yuv444pToHsl(y, yStride, u, uStride, v, vStride, width, height, hsl, hslStride);
}

SIMD_API void SimdYuv444pToHsv(const uint8_t * y, size_t yStride, const uint8_t * u, size_t uStride, const uint8_t * v, size_t vStride,
                               size_t width, size_t height, uint8_t * hsv, size_t hsvStride)
{
    SIMD_EMPTY();
    Base::Yuv444pToHsv(y, yStride, u, uStride, v, vStride, width, height, hsv, hsvStride);
}

SIMD_API void SimdYuv420pToHue(const uint8_t * y, size_t yStride, const uint8_t * u, size_t uStride, const uint8_t * v, size_t vStride,
                 size_t width, size_t height, uint8_t * hue, size_t hueStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv420pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv420pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv420pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv420pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
        Base::Yuv420pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
}

SIMD_API void SimdYuv444pToHue(const uint8_t * y, size_t yStride, const uint8_t * u, size_t uStride, const uint8_t * v, size_t vStride,
                 size_t width, size_t height, uint8_t * hue, size_t hueStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv444pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if(Avx2::Enable && width >= Avx2::A)
        Avx2::Yuv444pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if(Sse41::Enable && width >= Sse41::A)
        Sse41::Yuv444pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::Yuv444pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
    else
#endif
        Base::Yuv444pToHue(y, yStride, u, uStride, v, vStride, width, height, hue, hueStride);
}

SIMD_API void SimdYuv420pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv420pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv420pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv420pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv420pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
        Base::Yuv420pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
}

SIMD_API void SimdYuv422pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv422pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv422pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv422pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv422pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
        Base::Yuv422pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
}

SIMD_API void SimdYuv444pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable)
        Avx512bw::Yuv444pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::Yuv444pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::Yuv444pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::A)
        Neon::Yuv444pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
    else
#endif
        Base::Yuv444pToRgbV2(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
}

SIMD_API void SimdYuv444pToRgbaV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* rgba, size_t rgbaStride, uint8_t alpha, SimdYuvType yuvType)
{
    SIMD_EMPTY();
//#ifdef SIMD_AVX512BW_ENABLE
//    if (Avx512bw::Enable)
//        Avx512bw::Yuv444pToRgbaV2(y, yStride, u, uStride, v, vStride, width, height, rgba, rgbaStride, alpha, yuvType);
//    else
//#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::A)
        Avx2::Yuv444pToRgbaV2(y, yStride, u, uStride, v, vStride, width, height, rgba, rgbaStride, alpha, yuvType);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::A)
        Sse41::Yuv444pToRgbaV2(y, yStride, u, uStride, v, vStride, width, height, rgba, rgbaStride, alpha, yuvType);
    else
#endif
//#ifdef SIMD_NEON_ENABLE
//    if (Neon::Enable && width >= Neon::A)
//        Neon::Yuv444pToRgbaV2(y, yStride, u, uStride, v, vStride, width, height, rgba, rgbaStride, alpha, yuvType);
//    else
//#endif
        Base::Yuv444pToRgbaV2(y, yStride, u, uStride, v, vStride, width, height, rgba, rgbaStride, alpha, yuvType);
}

SIMD_API void SimdYuv420pToUyvy422(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
    size_t width, size_t height, uint8_t* uyvy, size_t uyvyStride)
{
    SIMD_EMPTY();
#ifdef SIMD_AVX512BW_ENABLE
    if (Avx512bw::Enable && width >= Avx512bw::DA)
        Avx512bw::Yuv420pToUyvy422(y, yStride, u, uStride, v, vStride, width, height, uyvy, uyvyStride);
    else
#endif
#ifdef SIMD_AVX2_ENABLE
    if (Avx2::Enable && width >= Avx2::DA)
        Avx2::Yuv420pToUyvy422(y, yStride, u, uStride, v, vStride, width, height, uyvy, uyvyStride);
    else
#endif
#ifdef SIMD_SSE41_ENABLE
    if (Sse41::Enable && width >= Sse41::DA)
        Sse41::Yuv420pToUyvy422(y, yStride, u, uStride, v, vStride, width, height, uyvy, uyvyStride);
    else
#endif
#ifdef SIMD_NEON_ENABLE
    if (Neon::Enable && width >= Neon::DA)
        Neon::Yuv420pToUyvy422(y, yStride, u, uStride, v, vStride, width, height, uyvy, uyvyStride);
    else
#endif
        Base::Yuv420pToUyvy422(y, yStride, u, uStride, v, vStride, width, height, uyvy, uyvyStride);
}




