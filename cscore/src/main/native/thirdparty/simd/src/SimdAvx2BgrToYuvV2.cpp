/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2023 Yermalayeu Ihar.
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
#include "Simd/SimdMemory.h"
#include "Simd/SimdStore.h"
#include "Simd/SimdConversion.h"
#include "Simd/SimdYuvToBgr.h"
#include "Simd/SimdBase.h"

namespace Simd
{
#ifdef SIMD_AVX2_ENABLE    
    namespace Avx2
    {
        template <bool align> SIMD_INLINE void LoadBgr(const __m256i * p, __m256i & blue, __m256i & green, __m256i & red)
        {
            __m256i bgr[3];
            bgr[0] = Load<align>(p + 0);
            bgr[1] = Load<align>(p + 1);
            bgr[2] = Load<align>(p + 2);
            blue = BgrToBlue(bgr);
            green = BgrToGreen(bgr);
            red = BgrToRed(bgr);
        }

#if defined(_MSC_VER) // Workaround for Visual Studio 2012 compiler bug in release mode:
        SIMD_INLINE __m256i Average16(const __m256i & s0, const __m256i & s1)
        {
            return _mm256_srli_epi16(_mm256_add_epi16(_mm256_add_epi16(
                _mm256_hadd_epi16(_mm256_unpacklo_epi8(s0, K_ZERO), _mm256_unpackhi_epi8(s0, K_ZERO)),
                _mm256_hadd_epi16(_mm256_unpacklo_epi8(s1, K_ZERO), _mm256_unpackhi_epi8(s1, K_ZERO))), K16_0002), 2);
        }
#else
        SIMD_INLINE __m256i Average16(const __m256i & s0, const __m256i & s1)
        {
            return _mm256_srli_epi16(_mm256_add_epi16(_mm256_add_epi16(_mm256_maddubs_epi16(s0, K8_01), _mm256_maddubs_epi16(s1, K8_01)), K16_0002), 2);
        }
#endif

        SIMD_INLINE void Average16(__m256i & a)
        {
#ifdef SIMD_MADDUBS_ERROR
            a = _mm256_srli_epi16(_mm256_add_epi16(_mm256_hadd_epi16(_mm256_unpacklo_epi8(a, K_ZERO), _mm256_unpackhi_epi8(a, K_ZERO)), K16_0001), 1);
#else
            a = _mm256_srli_epi16(_mm256_add_epi16(_mm256_maddubs_epi16(a, K8_01), K16_0001), 1);
#endif
        }

        //-------------------------------------------------------------------------------------------------

        template <class T> SIMD_INLINE void BgrToYuv420pV2(const uint8_t* bgr0, size_t bgrStride, uint8_t* y0, size_t yStride, uint8_t* u, uint8_t* v)
        {
            const uint8_t* bgr1 = bgr0 + bgrStride;
            uint8_t* y1 = y0 + yStride;

            __m256i blue[2][2], green[2][2], red[2][2];

            LoadBgr<false>((__m256i*)bgr0 + 0, blue[0][0], green[0][0], red[0][0]);
            _mm256_storeu_si256((__m256i*)y0 + 0, BgrToY8<T>(blue[0][0], green[0][0], red[0][0]));

            LoadBgr<false>((__m256i*)bgr0 + 3, blue[0][1], green[0][1], red[0][1]);
            _mm256_storeu_si256((__m256i*)y0 + 1, BgrToY8<T>(blue[0][1], green[0][1], red[0][1]));

            LoadBgr<false>((__m256i*)bgr1 + 0, blue[1][0], green[1][0], red[1][0]);
            _mm256_storeu_si256((__m256i*)y1 + 0, BgrToY8<T>(blue[1][0], green[1][0], red[1][0]));

            LoadBgr<false>((__m256i*)bgr1 + 3, blue[1][1], green[1][1], red[1][1]);
            _mm256_storeu_si256((__m256i*)y1 + 1, BgrToY8<T>(blue[1][1], green[1][1], red[1][1]));

            blue[0][0] = Average16(blue[0][0], blue[1][0]);
            blue[0][1] = Average16(blue[0][1], blue[1][1]);
            green[0][0] = Average16(green[0][0], green[1][0]);
            green[0][1] = Average16(green[0][1], green[1][1]);
            red[0][0] = Average16(red[0][0], red[1][0]);
            red[0][1] = Average16(red[0][1], red[1][1]);

            _mm256_storeu_si256((__m256i*)u, PackI16ToU8(BgrToU16<T>(blue[0][0], green[0][0], red[0][0]), BgrToU16<T>(blue[0][1], green[0][1], red[0][1])));
            _mm256_storeu_si256((__m256i*)v, PackI16ToU8(BgrToV16<T>(blue[0][0], green[0][0], red[0][0]), BgrToV16<T>(blue[0][1], green[0][1], red[0][1])));
        }

        template <class T>  void BgrToYuv420pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride)
        {
            assert((width % 2 == 0) && (height % 2 == 0) && (width >= DA) && (height >= 2));

            size_t widthDA = AlignLo(width, DA);
            for (size_t row = 0; row < height; row += 2)
            {
                for (size_t colUV = 0, colY = 0, colBgr = 0; colY < widthDA; colY += DA, colUV += A, colBgr += A * 6)
                    BgrToYuv420pV2<T>(bgr + colBgr, bgrStride, y + colY, yStride, u + colUV, v + colUV);
                if (width != widthDA)
                {
                    size_t colY = width - DA;
                    BgrToYuv420pV2<T>(bgr + colY * 3, bgrStride, y + colY, yStride, u + colY / 2, v + colY / 2);
                }
                y += 2 * yStride;
                u += uStride;
                v += vStride;
                bgr += 2 * bgrStride;
            }
        }

        void BgrToYuv420pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
        {
#if defined(SIMD_X86_ENABLE) && defined(NDEBUG) && defined(_MSC_VER) && _MSC_VER <= 1900
            Base::BgrToYuv420pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
#else
            switch (yuvType)
            {
            case SimdYuvBt601: BgrToYuv420pV2<Base::Bt601>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt709: BgrToYuv420pV2<Base::Bt709>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt2020: BgrToYuv420pV2<Base::Bt2020>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvTrect871: BgrToYuv420pV2<Base::Trect871>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            default:
                assert(0);
            }
#endif
        }

        //-------------------------------------------------------------------------------------------------

        template <class T> SIMD_INLINE void BgrToYuv422pV2(const uint8_t* bgr, uint8_t* y, uint8_t* u, uint8_t* v)
        {
            __m256i blue[2], green[2], red[2];

            LoadBgr<false>((__m256i*)bgr + 0, blue[0], green[0], red[0]);
            _mm256_storeu_si256((__m256i*)y + 0, BgrToY8<T>(blue[0], green[0], red[0]));

            LoadBgr<false>((__m256i*)bgr + 3, blue[1], green[1], red[1]);
            _mm256_storeu_si256((__m256i*)y + 1, BgrToY8<T>(blue[1], green[1], red[1]));

            Average16(blue[0]);
            Average16(blue[1]);
            Average16(green[0]);
            Average16(green[1]);
            Average16(red[0]);
            Average16(red[1]);

            _mm256_storeu_si256((__m256i*)u, PackI16ToU8(BgrToU16<T>(blue[0], green[0], red[0]), BgrToU16<T>(blue[1], green[1], red[1])));
            _mm256_storeu_si256((__m256i*)v, PackI16ToU8(BgrToV16<T>(blue[0], green[0], red[0]), BgrToV16<T>(blue[1], green[1], red[1])));
        }

        template <class T>  void BgrToYuv422pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride)
        {
            assert((width % 2 == 0) && (width >= DA));

            size_t widthDA = AlignLo(width, DA);
            for (size_t row = 0; row < height; row += 1)
            {
                for (size_t colUV = 0, colY = 0, colBgr = 0; colY < widthDA; colY += DA, colUV += A, colBgr += A * 6)
                    BgrToYuv422pV2<T>(bgr + colBgr, y + colY, u + colUV, v + colUV);
                if (width != widthDA)
                {
                    size_t colY = width - DA;
                    BgrToYuv422pV2<T>(bgr + colY * 3, y + colY, u + colY / 2, v + colY / 2);
                }
                y += yStride;
                u += uStride;
                v += vStride;
                bgr += bgrStride;
            }
        }

        void BgrToYuv422pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
        {
#if defined(SIMD_X86_ENABLE) && defined(NDEBUG) && defined(_MSC_VER) && _MSC_VER <= 1900
            Base::BgrToYuv422pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
#else
            switch (yuvType)
            {
            case SimdYuvBt601: BgrToYuv422pV2<Base::Bt601>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt709: BgrToYuv422pV2<Base::Bt709>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt2020: BgrToYuv422pV2<Base::Bt2020>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvTrect871: BgrToYuv422pV2<Base::Trect871>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            default:
                assert(0);
            }
#endif
        }

        //-------------------------------------------------------------------------------------------------

        template <class T> SIMD_INLINE void BgrToYuv444pV2(const uint8_t* bgr, uint8_t* y, uint8_t* u, uint8_t* v)
        {
            __m256i blue, green, red;
            LoadBgr<false>((__m256i*)bgr, blue, green, red);
            _mm256_storeu_si256((__m256i*)y, BgrToY8<T>(blue, green, red));
            _mm256_storeu_si256((__m256i*)u, BgrToU8<T>(blue, green, red));
            _mm256_storeu_si256((__m256i*)v, BgrToV8<T>(blue, green, red));
        }

        template <class T>  void BgrToYuv444pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride)
        {
            assert(width >= A);

            size_t widthA = AlignLo(width, A);
            for (size_t row = 0; row < height; row += 1)
            {
                for (size_t col = 0; col < widthA; col += A)
                    BgrToYuv444pV2<T>(bgr + col * 3, y + col, u + col, v + col);
                if (width != widthA)
                {
                    size_t col = width - A;
                    BgrToYuv444pV2<T>(bgr + col * 3, y + col, u + col, v + col);
                }
                y += yStride;
                u += uStride;
                v += vStride;
                bgr += bgrStride;
            }
        }

        void BgrToYuv444pV2(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
        {
#if defined(SIMD_X86_ENABLE) && defined(NDEBUG) && defined(_MSC_VER) && _MSC_VER <= 1900
            Base::BgrToYuv444pV2(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride, yuvType);
#else
            switch (yuvType)
            {
            case SimdYuvBt601: BgrToYuv444pV2<Base::Bt601>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt709: BgrToYuv444pV2<Base::Bt709>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt2020: BgrToYuv444pV2<Base::Bt2020>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvTrect871: BgrToYuv444pV2<Base::Trect871>(bgr, bgrStride, width, height, y, yStride, u, uStride, v, vStride); break;
            default:
                assert(0);
            }
#endif
        }
    }
#endif// SIMD_AVX2_ENABLE
}
