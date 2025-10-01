/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2024 Yermalayeu Ihar.
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
#include "Simd/SimdYuvToBgr.h"

namespace Simd
{
#ifdef SIMD_AVX512BW_ENABLE    
    namespace Avx512bw
    {
        SIMD_INLINE void Average16(__m512i & a, const __m512i & b)
        {
            a = _mm512_srli_epi16(_mm512_add_epi16(_mm512_add_epi16(a, b), K16_0002), 2);
        }

        SIMD_INLINE void Average16(__m512i a[2][2])
        {
            a[0][0] = _mm512_srli_epi16(_mm512_add_epi16(a[0][0], K16_0001), 1);
            a[0][1] = _mm512_srli_epi16(_mm512_add_epi16(a[0][1], K16_0001), 1);
            a[1][0] = _mm512_srli_epi16(_mm512_add_epi16(a[1][0], K16_0001), 1);
            a[1][1] = _mm512_srli_epi16(_mm512_add_epi16(a[1][1], K16_0001), 1);
        }

        //-------------------------------------------------------------------------------------------------

        template <class T, bool tail> SIMD_INLINE void BgraToYuv444pV2(const uint8_t* bgra, uint8_t* y, uint8_t* u, uint8_t* v, const __mmask64* tails)
        {
            __m512i _b16_r16[2][2], _g16_1[2][2];
            LoadPreparedBgra16<false, tail>(bgra + 0 * A, _b16_r16[0][0], _g16_1[0][0], tails + 0);
            LoadPreparedBgra16<false, tail>(bgra + 1 * A, _b16_r16[0][1], _g16_1[0][1], tails + 1);
            LoadPreparedBgra16<false, tail>(bgra + 2 * A, _b16_r16[1][0], _g16_1[1][0], tails + 2);
            LoadPreparedBgra16<false, tail>(bgra + 3 * A, _b16_r16[1][1], _g16_1[1][1], tails + 3);

            Store<false, tail>(y, Permuted2Pack16iTo8u(BgrToY16<T>(_b16_r16[0], _g16_1[0]), BgrToY16<T>(_b16_r16[1], _g16_1[1])), tails[4]);
            Store<false, tail>(u, Permuted2Pack16iTo8u(BgrToU16<T>(_b16_r16[0], _g16_1[0]), BgrToU16<T>(_b16_r16[1], _g16_1[1])), tails[4]);
            Store<false, tail>(v, Permuted2Pack16iTo8u(BgrToV16<T>(_b16_r16[0], _g16_1[0]), BgrToV16<T>(_b16_r16[1], _g16_1[1])), tails[4]);
        }

        template <class T> void BgraToYuv444pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height,
            uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride)
        {
            assert(width >= A);

            size_t widthA = AlignLo(width, A);
            size_t tail = width - widthA;
            __mmask64 tails[5];
            for (size_t i = 0; i < 4; ++i)
                tails[i] = TailMask64(tail * 4 - A * i);
            tails[4] = TailMask64(tail);
            for (size_t row = 0; row < height; ++row)
            {
                size_t col = 0;
                for (; col < widthA; col += A)
                    BgraToYuv444pV2<T, false>(bgra + col * 4, y + col, u + col, v + col, tails);
                if (tail)
                    BgraToYuv444pV2<T, true>(bgra + col * 4, y + col, u + col, v + col, tails);
                y += yStride;
                u += uStride;
                v += vStride;
                bgra += bgraStride;
            }
        }

        void BgraToYuv444pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height,
            uint8_t* y, size_t yStride, uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: BgraToYuv444pV2<Base::Bt601>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt709: BgraToYuv444pV2<Base::Bt709>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt2020: BgraToYuv444pV2<Base::Bt2020>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvTrect871: BgraToYuv444pV2<Base::Trect871>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            default:
                assert(0);
            }
        }

        //-------------------------------------------------------------------------------------------------

        template <class T, bool tail> SIMD_INLINE __m512i LoadAndBgrToY16(const uint8_t* bgra, __m512i& b16_r16, __m512i& g16_1, const __mmask64* tails)
        {
            static const __m512i Y_LO = SIMD_MM512_SET1_EPI16(T::Y_LO);
            __m512i _b16_r16[2], _g16_1[2];
            LoadPreparedBgra16<false, tail>(bgra + 0, _b16_r16[0], _g16_1[0], tails + 0);
            LoadPreparedBgra16<false, tail>(bgra + A, _b16_r16[1], _g16_1[1], tails + 1);
            b16_r16 = Hadd32(_b16_r16[0], _b16_r16[1]);
            g16_1 = Hadd32(_g16_1[0], _g16_1[1]);
            return BgrToY16<T>(_b16_r16, _g16_1);
        }

        template <class T, bool tail> SIMD_INLINE __m512i LoadAndBgrToY8(const uint8_t* bgra, __m512i b16_r16[2], __m512i g16_1[2], const __mmask64* tails)
        {
            __m512i lo = LoadAndBgrToY16<T, tail>(bgra + 0 * A, b16_r16[0], g16_1[0], tails + 0);
            __m512i hi = LoadAndBgrToY16<T, tail>(bgra + 2 * A, b16_r16[1], g16_1[1], tails + 2);
            return Permuted2Pack16iTo8u(lo, hi);
        }

        template <class T, bool tail> SIMD_INLINE void BgraToYuv422pV2(const uint8_t* bgra, uint8_t* y, uint8_t* u, uint8_t* v, const __mmask64* tails)
        {
            __m512i _b16_r16[2][2], _g16_1[2][2];
            Store<false, tail>(y + 0, LoadAndBgrToY8<T, tail>(bgra + 0 * A, _b16_r16[0], _g16_1[0], tails + 0), tails[8]);
            Store<false, tail>(y + A, LoadAndBgrToY8<T, tail>(bgra + 4 * A, _b16_r16[1], _g16_1[1], tails + 4), tails[9]);

            Average16(_b16_r16);
            Average16(_g16_1);

            Store<false, tail>(u, Permuted2Pack16iTo8u(BgrToU16<T>(_b16_r16[0], _g16_1[0]), BgrToU16<T>(_b16_r16[1], _g16_1[1])), tails[10]);
            Store<false, tail>(v, Permuted2Pack16iTo8u(BgrToV16<T>(_b16_r16[0], _g16_1[0]), BgrToV16<T>(_b16_r16[1], _g16_1[1])), tails[10]);
        }

        template <class T>  void BgraToYuv422pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride)
        {
            assert((width % 2 == 0));

            size_t widthDA = AlignLo(width, DA);
            size_t tail = width - widthDA;
            __mmask64 tails[11];
            for (size_t i = 0; i < 8; ++i)
                tails[i] = TailMask64(tail * 4 - A * i);
            for (size_t i = 0; i < 2; ++i)
                tails[8 + i] = TailMask64(tail - A * i);
            tails[10] = TailMask64(tail / 2);
            for (size_t row = 0; row < height; row += 1)
            {
                size_t colUV = 0, colY = 0, colBgra = 0;
                for (; colY < widthDA; colY += DA, colUV += A, colBgra += A * 8)
                    BgraToYuv422pV2<T, false>(bgra + colBgra, y + colY, u + colUV, v + colUV, tails);
                if (tail)
                    BgraToYuv422pV2<T, true>(bgra + colBgra, y + colY, u + colUV, v + colUV, tails);
                y += yStride;
                u += uStride;
                v += vStride;
                bgra += bgraStride;
            }
        }

        void BgraToYuv422pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: BgraToYuv422pV2<Base::Bt601>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt709: BgraToYuv422pV2<Base::Bt709>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt2020: BgraToYuv422pV2<Base::Bt2020>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvTrect871: BgraToYuv422pV2<Base::Trect871>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            default:
                assert(0);
            }
        }

        //-------------------------------------------------------------------------------------------------

        template <class T, bool tail> SIMD_INLINE void BgraToYuv420pV2(const uint8_t* bgra0, size_t bgraStride, uint8_t* y0, size_t yStride, uint8_t* u, uint8_t* v, const __mmask64* tails)
        {
            const uint8_t* bgra1 = bgra0 + bgraStride;
            uint8_t* y1 = y0 + yStride;

            __m512i _b16_r16[2][2][2], _g16_1[2][2][2];
            Store<false, tail>(y0 + 0, LoadAndBgrToY8<T, tail>(bgra0 + 0 * A, _b16_r16[0][0], _g16_1[0][0], tails + 0), tails[8]);
            Store<false, tail>(y0 + A, LoadAndBgrToY8<T, tail>(bgra0 + 4 * A, _b16_r16[0][1], _g16_1[0][1], tails + 4), tails[9]);
            Store<false, tail>(y1 + 0, LoadAndBgrToY8<T, tail>(bgra1 + 0 * A, _b16_r16[1][0], _g16_1[1][0], tails + 0), tails[8]);
            Store<false, tail>(y1 + A, LoadAndBgrToY8<T, tail>(bgra1 + 4 * A, _b16_r16[1][1], _g16_1[1][1], tails + 4), tails[9]);

            Average16(_b16_r16[0][0][0], _b16_r16[1][0][0]);
            Average16(_b16_r16[0][0][1], _b16_r16[1][0][1]);
            Average16(_b16_r16[0][1][0], _b16_r16[1][1][0]);
            Average16(_b16_r16[0][1][1], _b16_r16[1][1][1]);

            Average16(_g16_1[0][0][0], _g16_1[1][0][0]);
            Average16(_g16_1[0][0][1], _g16_1[1][0][1]);
            Average16(_g16_1[0][1][0], _g16_1[1][1][0]);
            Average16(_g16_1[0][1][1], _g16_1[1][1][1]);

            Store<false, tail>(u, Permuted2Pack16iTo8u(BgrToU16<T>(_b16_r16[0][0], _g16_1[0][0]), BgrToU16<T>(_b16_r16[0][1], _g16_1[0][1])), tails[10]);
            Store<false, tail>(v, Permuted2Pack16iTo8u(BgrToV16<T>(_b16_r16[0][0], _g16_1[0][0]), BgrToV16<T>(_b16_r16[0][1], _g16_1[0][1])), tails[10]);
        }

        template <class T>  void BgraToYuv420pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride)
        {
            assert((width % 2 == 0) && (height % 2 == 0));

            size_t widthDA = AlignLo(width, DA);
            size_t tail = width - widthDA;
            __mmask64 tails[11];
            for (size_t i = 0; i < 8; ++i)
                tails[i] = TailMask64(tail * 4 - A * i);
            for (size_t i = 0; i < 2; ++i)
                tails[8 + i] = TailMask64(tail - A * i);
            tails[10] = TailMask64(tail/ 2);
            for (size_t row = 0; row < height; row += 2)
            {
                size_t colUV = 0, colY = 0, colBgra = 0;
                for (; colY < widthDA; colY += DA, colUV += A, colBgra += A * 8)
                    BgraToYuv420pV2<T, false>(bgra + colBgra, bgraStride, y + colY, yStride, u + colUV, v + colUV, tails);
                if (tail)
                    BgraToYuv420pV2<T, true>(bgra + colBgra, bgraStride, y + colY, yStride, u + colUV, v + colUV, tails);
                y += 2 * yStride;
                u += uStride;
                v += vStride;
                bgra += 2 * bgraStride;
            }
        }

        void BgraToYuv420pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: BgraToYuv420pV2<Base::Bt601>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt709: BgraToYuv420pV2<Base::Bt709>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvBt2020: BgraToYuv420pV2<Base::Bt2020>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            case SimdYuvTrect871: BgraToYuv420pV2<Base::Trect871>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride); break;
            default:
                assert(0);
            }
        }

        //-------------------------------------------------------------------------------------------------

        template <class T, bool mask> SIMD_INLINE void LoadAndConvertYA16(const uint8_t* bgra, __m512i& b16_r16, __m512i& g16_1, __m512i& y16, __m512i& a16, const __mmask64* tails)
        {
            __m512i _b16_r16[2], _g16_1[2], a32[2];
            LoadPreparedBgra16<false, mask>(bgra + 0, _b16_r16[0], _g16_1[0], a32[0], tails + 0);
            LoadPreparedBgra16<false, mask>(bgra + A, _b16_r16[1], _g16_1[1], a32[1], tails + 1);
            b16_r16 = Hadd32(_b16_r16[0], _b16_r16[1]);
            g16_1 = Hadd32(_g16_1[0], _g16_1[1]);
            static const __m512i Y_LO = SIMD_MM512_SET1_EPI16(T::Y_LO);
            y16 = Saturate16iTo8u(_mm512_add_epi16(Y_LO, _mm512_packs_epi32(BgrToY32<T>(_b16_r16[0], _g16_1[0]), BgrToY32<T>(_b16_r16[1], _g16_1[1]))));
            a16 = _mm512_packs_epi32(a32[0], a32[1]);
        }

        template <class T, bool mask> SIMD_INLINE void LoadAndStoreYA(const uint8_t* bgra, __m512i b16_r16[2], __m512i g16_1[2], uint8_t* y, uint8_t* a, const __mmask64* tails)
        {
            __m512i y16[2], a16[2];
            LoadAndConvertYA16<T, mask>(bgra + 0 * A, b16_r16[0], g16_1[0], y16[0], a16[0], tails + 0);
            LoadAndConvertYA16<T, mask>(bgra + 2 * A, b16_r16[1], g16_1[1], y16[1], a16[1], tails + 2);
            Store<false, mask>(y, Permuted2Pack16iTo8u(y16[0], y16[1]), tails[4]);
            Store<false, mask>(a, Permuted2Pack16iTo8u(a16[0], a16[1]), tails[4]);
        }

        template <class T, bool mask> SIMD_INLINE void BgraToYuva420pV2(const uint8_t* bgra0, size_t bgraStride, uint8_t* y0, size_t yStride, uint8_t* u, uint8_t* v, uint8_t* a0, size_t aStride, const __mmask64* tails)
        {
            const uint8_t* bgra1 = bgra0 + bgraStride;
            uint8_t* y1 = y0 + yStride;
            uint8_t* a1 = a0 + aStride;

            __m512i _b16_r16[2][2][2], _g16_1[2][2][2];
            LoadAndStoreYA<T, mask>(bgra0 + 0 * A, _b16_r16[0][0], _g16_1[0][0], y0 + 0, a0 + 0, tails + 0);
            LoadAndStoreYA<T, mask>(bgra0 + 4 * A, _b16_r16[0][1], _g16_1[0][1], y0 + A, a0 + A, tails + 5);
            LoadAndStoreYA<T, mask>(bgra1 + 0 * A, _b16_r16[1][0], _g16_1[1][0], y1 + 0, a1 + 0, tails + 0);
            LoadAndStoreYA<T, mask>(bgra1 + 4 * A, _b16_r16[1][1], _g16_1[1][1], y1 + A, a1 + A, tails + 5);

            Average16(_b16_r16[0][0][0], _b16_r16[1][0][0]);
            Average16(_b16_r16[0][0][1], _b16_r16[1][0][1]);
            Average16(_b16_r16[0][1][0], _b16_r16[1][1][0]);
            Average16(_b16_r16[0][1][1], _b16_r16[1][1][1]);

            Average16(_g16_1[0][0][0], _g16_1[1][0][0]);
            Average16(_g16_1[0][0][1], _g16_1[1][0][1]);
            Average16(_g16_1[0][1][0], _g16_1[1][1][0]);
            Average16(_g16_1[0][1][1], _g16_1[1][1][1]);

            Store<false, mask>(u, Permuted2Pack16iTo8u(BgrToU16<T>(_b16_r16[0][0], _g16_1[0][0]), BgrToU16<T>(_b16_r16[0][1], _g16_1[0][1])), tails[10]);
            Store<false, mask>(v, Permuted2Pack16iTo8u(BgrToV16<T>(_b16_r16[0][0], _g16_1[0][0]), BgrToV16<T>(_b16_r16[0][1], _g16_1[0][1])), tails[10]);
        }

        template <class T>  void BgraToYuva420pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, uint8_t* a, size_t aStride)
        {
            assert((width % 2 == 0) && (height % 2 == 0) && (width >= DA));

            width /= 2;
            size_t widthA = AlignLo(width, A);
            size_t tail = width - widthA;
            __mmask64 tails[11];
            for (size_t i = 0; i < 4; ++i)
            {
                tails[i + 0] = TailMask64(tail * 8 - A * (i + 0));
                tails[i + 5] = TailMask64(tail * 8 - A * (i + 4));
            }
            tails[4] = TailMask64(tail * 2 - A * 0);
            tails[9] = TailMask64(tail * 2 - A * 1);
            tails[10] = TailMask64(tail);
            for (size_t row = 0; row < height; row += 2)
            {
                size_t col = 0;
                for (; col < widthA; col += A)
                    BgraToYuva420pV2<T, false>(bgra + col * 8, bgraStride, y + col * 2, yStride, u + col, v + col, a + col * 2, aStride, tails);
                if (col < width)
                    BgraToYuva420pV2<T, true>(bgra + col * 8, bgraStride, y + col * 2, yStride, u + col, v + col, a + col * 2, aStride, tails);
                bgra += 2 * bgraStride;
                y += 2 * yStride;
                u += uStride;
                v += vStride;
                a += 2 * aStride;
            }
        }

        void BgraToYuva420pV2(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height, uint8_t* y, size_t yStride,
            uint8_t* u, size_t uStride, uint8_t* v, size_t vStride, uint8_t* a, size_t aStride, SimdYuvType yuvType)
        {
            switch (yuvType)
            {
            case SimdYuvBt601: BgraToYuva420pV2<Base::Bt601>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride); break;
            case SimdYuvBt709: BgraToYuva420pV2<Base::Bt709>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride); break;
            case SimdYuvBt2020: BgraToYuva420pV2<Base::Bt2020>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride); break;
            case SimdYuvTrect871: BgraToYuva420pV2<Base::Trect871>(bgra, bgraStride, width, height, y, yStride, u, uStride, v, vStride, a, aStride); break;
            default:
                assert(0);
            }
        }
    }
#endif
}
