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
#include "Simd/SimdInterleave.h"
#include "Simd/SimdYuvToBgr.h"

namespace Simd
{
#ifdef SIMD_AVX2_ENABLE    
    namespace Avx2
    {
        template <bool align, class T> SIMD_YUV_TO_BGR_INLINE void YuvToBgrV2(__m256i y8, __m256i u8, __m256i v8, __m256i* bgr)
        {
            __m256i blue = YuvToBlue<T>(y8, u8);
            __m256i green = YuvToGreen<T>(y8, u8, v8);
            __m256i red = YuvToRed<T>(y8, v8);
            Store<align>(bgr + 0, InterleaveBgr<0>(blue, green, red));
            Store<align>(bgr + 1, InterleaveBgr<1>(blue, green, red));
            Store<align>(bgr + 2, InterleaveBgr<2>(blue, green, red));
        }

        template <bool align, class T> SIMD_INLINE void Yuv422pToBgrV2(const uint8_t* y, const __m256i& u, const __m256i& v,
            uint8_t* bgr)
        {
            YuvToBgrV2<align, T>(Load<align>((__m256i*)y + 0), _mm256_unpacklo_epi8(u, u), _mm256_unpacklo_epi8(v, v), (__m256i*)bgr + 0);
            YuvToBgrV2<align, T>(Load<align>((__m256i*)y + 1), _mm256_unpackhi_epi8(u, u), _mm256_unpackhi_epi8(v, v), (__m256i*)bgr + 3);
        }

        template <bool align, class T> void Yuv420pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride)
        {
            assert((width % 2 == 0) && (height % 2 == 0) && (width >= DA) && (height >= 2));
            if (align)
            {
                assert(Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride));
                assert(Aligned(v) && Aligned(vStride) && Aligned(bgr) && Aligned(bgrStride));
            }

            size_t bodyWidth = AlignLo(width, DA);
            size_t tail = width - bodyWidth;
            for (size_t row = 0; row < height; row += 2)
            {
                for (size_t colUV = 0, colY = 0, colBgr = 0; colY < bodyWidth; colY += DA, colUV += A, colBgr += A * 6)
                {
                    __m256i u_ = LoadPermuted<align>((__m256i*)(u + colUV));
                    __m256i v_ = LoadPermuted<align>((__m256i*)(v + colUV));
                    Yuv422pToBgrV2<align, T>(y + colY, u_, v_, bgr + colBgr);
                    Yuv422pToBgrV2<align, T>(y + colY + yStride, u_, v_, bgr + colBgr + bgrStride);
                }
                if (tail)
                {
                    size_t offset = width - DA;
                    __m256i u_ = LoadPermuted<false>((__m256i*)(u + offset / 2));
                    __m256i v_ = LoadPermuted<false>((__m256i*)(v + offset / 2));
                    Yuv422pToBgrV2<false, T>(y + offset, u_, v_, bgr + 3 * offset);
                    Yuv422pToBgrV2<false, T>(y + offset + yStride, u_, v_, bgr + 3 * offset + bgrStride);
                }
                y += 2 * yStride;
                u += uStride;
                v += vStride;
                bgr += 2 * bgrStride;
            }
        }

        template <bool align> void Yuv420pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: Yuv420pToBgrV2<align, Base::Bt601>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvBt709: Yuv420pToBgrV2<align, Base::Bt709>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvBt2020: Yuv420pToBgrV2<align, Base::Bt2020>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvTrect871: Yuv420pToBgrV2<align, Base::Trect871>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            default:
                assert(0);
            }
        }

        void Yuv420pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
        {
            if (Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride)
                && Aligned(v) && Aligned(vStride) && Aligned(bgr) && Aligned(bgrStride))
                Yuv420pToBgrV2<true>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
            else
                Yuv420pToBgrV2<false>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
        }

        //-------------------------------------------------------------------------------------------------

        template <bool align, class T> void Yuv422pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride)
        {
            assert((width % 2 == 0) && (width >= DA));
            if (align)
            {
                assert(Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride));
                assert(Aligned(v) && Aligned(vStride) && Aligned(bgr) && Aligned(bgrStride));
            }

            size_t bodyWidth = AlignLo(width, DA);
            size_t tail = width - bodyWidth;
            for (size_t row = 0; row < height; row += 1)
            {
                for (size_t colUV = 0, colY = 0, colBgr = 0; colY < bodyWidth; colY += DA, colUV += A, colBgr += A * 6)
                {
                    __m256i u_ = LoadPermuted<align>((__m256i*)(u + colUV));
                    __m256i v_ = LoadPermuted<align>((__m256i*)(v + colUV));
                    Yuv422pToBgrV2<align, T>(y + colY, u_, v_, bgr + colBgr);
                }
                if (tail)
                {
                    size_t offset = width - DA;
                    __m256i u_ = LoadPermuted<false>((__m256i*)(u + offset / 2));
                    __m256i v_ = LoadPermuted<false>((__m256i*)(v + offset / 2));
                    Yuv422pToBgrV2<false, T>(y + offset, u_, v_, bgr + 3 * offset);
                }
                y += yStride;
                u += uStride;
                v += vStride;
                bgr += bgrStride;
            }
        }

        template <bool align> void Yuv422pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: Yuv422pToBgrV2<align, Base::Bt601>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvBt709: Yuv422pToBgrV2<align, Base::Bt709>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvBt2020: Yuv422pToBgrV2<align, Base::Bt2020>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvTrect871: Yuv422pToBgrV2<align, Base::Trect871>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            default:
                assert(0);
            }
        }

        void Yuv422pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
        {
            if (Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride)
                && Aligned(v) && Aligned(vStride) && Aligned(bgr) && Aligned(bgrStride))
                Yuv422pToBgrV2<true>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
            else
                Yuv422pToBgrV2<false>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
        }

        //-------------------------------------------------------------------------------------------------

        template <bool align, class T> SIMD_INLINE void Yuv444pToBgrV2(const uint8_t* y, const uint8_t* u, const uint8_t* v, uint8_t* bgr)
        {
            YuvToBgrV2<align, T>(Load<align>((__m256i*)y), Load<align>((__m256i*)u), Load<align>((__m256i*)v), (__m256i*)bgr);
        }

        template <bool align, class T> void Yuv444pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride)
        {
            if (align)
            {
                assert(Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride));
                assert(Aligned(v) && Aligned(vStride) && Aligned(bgr) && Aligned(bgrStride));
            }

            size_t bodyWidth = AlignLo(width, A);
            size_t tail = width - bodyWidth;
            for (size_t row = 0; row < height; row += 1)
            {
                for (size_t col = 0, colBgr = 0; col < bodyWidth; col += A, colBgr += A * 3)
                    Yuv444pToBgrV2<align, T>(y + col, u + col, v + col, bgr + colBgr);
                if (tail)
                {
                    size_t offset = width - A;
                    Yuv444pToBgrV2<false, T>(y + offset, u + offset, v + offset, bgr + 3 * offset);
                }
                y += yStride;
                u += uStride;
                v += vStride;
                bgr += bgrStride;
            }
        }

        template <bool align> void Yuv444pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: Yuv444pToBgrV2<align, Base::Bt601>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvBt709: Yuv444pToBgrV2<align, Base::Bt709>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvBt2020: Yuv444pToBgrV2<align, Base::Bt2020>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            case SimdYuvTrect871: Yuv444pToBgrV2<align, Base::Trect871>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride); break;
            default:
                assert(0);
            }
        }

        void Yuv444pToBgrV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* bgr, size_t bgrStride, SimdYuvType yuvType)
        {
            if (Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride)
                && Aligned(v) && Aligned(vStride) && Aligned(bgr) && Aligned(bgrStride))
                Yuv444pToBgrV2<true>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
            else
                Yuv444pToBgrV2<false>(y, yStride, u, uStride, v, vStride, width, height, bgr, bgrStride, yuvType);
        }        
        
        //-------------------------------------------------------------------------------------------------

        template <bool align, class T> SIMD_YUV_TO_BGR_INLINE void YuvToRgbV2(__m256i y8, __m256i u8, __m256i v8, __m256i* rgb)
        {
            __m256i blue = YuvToBlue<T>(y8, u8);
            __m256i green = YuvToGreen<T>(y8, u8, v8);
            __m256i red = YuvToRed<T>(y8, v8);
            Store<align>(rgb + 0, InterleaveBgr<0>(red, green, blue));
            Store<align>(rgb + 1, InterleaveBgr<1>(red, green, blue));
            Store<align>(rgb + 2, InterleaveBgr<2>(red, green, blue));
        }

        template <bool align, class T> SIMD_INLINE void Yuv422pToRgbV2(const uint8_t* y, const __m256i& u, const __m256i& v,
            uint8_t* rgb)
        {
            YuvToRgbV2<align, T>(Load<align>((__m256i*)y + 0), _mm256_unpacklo_epi8(u, u), _mm256_unpacklo_epi8(v, v), (__m256i*)rgb + 0);
            YuvToRgbV2<align, T>(Load<align>((__m256i*)y + 1), _mm256_unpackhi_epi8(u, u), _mm256_unpackhi_epi8(v, v), (__m256i*)rgb + 3);
        }

        template <bool align, class T> void Yuv420pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride)
        {
            assert((width % 2 == 0) && (height % 2 == 0) && (width >= DA) && (height >= 2));
            if (align)
            {
                assert(Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride));
                assert(Aligned(v) && Aligned(vStride) && Aligned(rgb) && Aligned(rgbStride));
            }

            size_t bodyWidth = AlignLo(width, DA);
            size_t tail = width - bodyWidth;
            for (size_t row = 0; row < height; row += 2)
            {
                for (size_t colUV = 0, colY = 0, colRgb = 0; colY < bodyWidth; colY += DA, colUV += A, colRgb += A * 6)
                {
                    __m256i u_ = LoadPermuted<align>((__m256i*)(u + colUV));
                    __m256i v_ = LoadPermuted<align>((__m256i*)(v + colUV));
                    Yuv422pToRgbV2<align, T>(y + colY, u_, v_, rgb + colRgb);
                    Yuv422pToRgbV2<align, T>(y + colY + yStride, u_, v_, rgb + colRgb + rgbStride);
                }
                if (tail)
                {
                    size_t offset = width - DA;
                    __m256i u_ = LoadPermuted<false>((__m256i*)(u + offset / 2));
                    __m256i v_ = LoadPermuted<false>((__m256i*)(v + offset / 2));
                    Yuv422pToRgbV2<false, T>(y + offset, u_, v_, rgb + 3 * offset);
                    Yuv422pToRgbV2<false, T>(y + offset + yStride, u_, v_, rgb + 3 * offset + rgbStride);
                }
                y += 2 * yStride;
                u += uStride;
                v += vStride;
                rgb += 2 * rgbStride;
            }
        }

        template <bool align> void Yuv420pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: Yuv420pToRgbV2<align, Base::Bt601>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvBt709: Yuv420pToRgbV2<align, Base::Bt709>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvBt2020: Yuv420pToRgbV2<align, Base::Bt2020>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvTrect871: Yuv420pToRgbV2<align, Base::Trect871>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            default:
                assert(0);
            }
        }

        void Yuv420pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
        {
            if (Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride)
                && Aligned(v) && Aligned(vStride) && Aligned(rgb) && Aligned(rgbStride))
                Yuv420pToRgbV2<true>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
            else
                Yuv420pToRgbV2<false>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
        }

        //-------------------------------------------------------------------------------------------------

        template <bool align, class T> void Yuv422pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride)
        {
            assert((width % 2 == 0) && (width >= DA));
            if (align)
            {
                assert(Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride));
                assert(Aligned(v) && Aligned(vStride) && Aligned(rgb) && Aligned(rgbStride));
            }

            size_t bodyWidth = AlignLo(width, DA);
            size_t tail = width - bodyWidth;
            for (size_t row = 0; row < height; row += 1)
            {
                for (size_t colUV = 0, colY = 0, colRgb = 0; colY < bodyWidth; colY += DA, colUV += A, colRgb += A * 6)
                {
                    __m256i u_ = LoadPermuted<align>((__m256i*)(u + colUV));
                    __m256i v_ = LoadPermuted<align>((__m256i*)(v + colUV));
                    Yuv422pToRgbV2<align, T>(y + colY, u_, v_, rgb + colRgb);
                }
                if (tail)
                {
                    size_t offset = width - DA;
                    __m256i u_ = LoadPermuted<false>((__m256i*)(u + offset / 2));
                    __m256i v_ = LoadPermuted<false>((__m256i*)(v + offset / 2));
                    Yuv422pToRgbV2<false, T>(y + offset, u_, v_, rgb + 3 * offset);
                }
                y += yStride;
                u += uStride;
                v += vStride;
                rgb += rgbStride;
            }
        }

        template <bool align> void Yuv422pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: Yuv422pToRgbV2<align, Base::Bt601>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvBt709: Yuv422pToRgbV2<align, Base::Bt709>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvBt2020: Yuv422pToRgbV2<align, Base::Bt2020>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvTrect871: Yuv422pToRgbV2<align, Base::Trect871>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            default:
                assert(0);
            }
        }

        void Yuv422pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
        {
            if (Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride)
                && Aligned(v) && Aligned(vStride) && Aligned(rgb) && Aligned(rgbStride))
                Yuv422pToRgbV2<true>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
            else
                Yuv422pToRgbV2<false>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
        }

        //-------------------------------------------------------------------------------------------------

        template <bool align, class T> SIMD_INLINE void Yuv444pToRgbV2(const uint8_t* y, const uint8_t* u, const uint8_t* v, uint8_t* rgb)
        {
            YuvToRgbV2<align, T>(Load<align>((__m256i*)y), Load<align>((__m256i*)u), Load<align>((__m256i*)v), (__m256i*)rgb);
        }

        template <bool align, class T> void Yuv444pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride)
        {
            if (align)
            {
                assert(Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride));
                assert(Aligned(v) && Aligned(vStride) && Aligned(rgb) && Aligned(rgbStride));
            }

            size_t bodyWidth = AlignLo(width, A);
            size_t tail = width - bodyWidth;
            for (size_t row = 0; row < height; row += 1)
            {
                for (size_t col = 0, colRgb = 0; col < bodyWidth; col += A, colRgb += A * 3)
                    Yuv444pToRgbV2<align, T>(y + col, u + col, v + col, rgb + colRgb);
                if (tail)
                {
                    size_t offset = width - A;
                    Yuv444pToRgbV2<false, T>(y + offset, u + offset, v + offset, rgb + 3 * offset);
                }
                y += yStride;
                u += uStride;
                v += vStride;
                rgb += rgbStride;
            }
        }

        template <bool align> void Yuv444pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: Yuv444pToRgbV2<align, Base::Bt601>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvBt709: Yuv444pToRgbV2<align, Base::Bt709>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvBt2020: Yuv444pToRgbV2<align, Base::Bt2020>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            case SimdYuvTrect871: Yuv444pToRgbV2<align, Base::Trect871>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride); break;
            default:
                assert(0);
            }
        }

        void Yuv444pToRgbV2(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride,
            size_t width, size_t height, uint8_t* rgb, size_t rgbStride, SimdYuvType yuvType)
        {
            if (Aligned(y) && Aligned(yStride) && Aligned(u) && Aligned(uStride)
                && Aligned(v) && Aligned(vStride) && Aligned(rgb) && Aligned(rgbStride))
                Yuv444pToRgbV2<true>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
            else
                Yuv444pToRgbV2<false>(y, yStride, u, uStride, v, vStride, width, height, rgb, rgbStride, yuvType);
        }
    }
#endif// SIMD_AVX2_ENABLE
}
