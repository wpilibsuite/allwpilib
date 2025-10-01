/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2025 Yermalayeu Ihar.
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
#ifndef __SimdYuvToBgr_h__
#define __SimdYuvToBgr_h__

#include "Simd/SimdInit.h"
#include "Simd/SimdSet.h"
#include "Simd/SimdMath.h"
#include "Simd/SimdUnpack.h"
#include "Simd/SimdLog.h"
#include "Simd/SimdLoad.h"

#if defined(_MSC_VER) && _MSC_VER >= 1900    
#define SIMD_YUV_TO_BGR_INLINE inline
#else
#define SIMD_YUV_TO_BGR_INLINE SIMD_INLINE
#endif

namespace Simd
{
    namespace Base
    {
        /* Corresponds to BT.601 standard. Uses Kr=0.299, Kb=0.114. Restricts Y to range [16..235], U and V to [16..240]. */
        struct Bt601
        {
            static const int Y_LO = 16;
            static const int Y_HI = 235;
            static const int UV_LO = 16;
            static const int UV_HI = 240;
            static const int UV_Z = 128;

            static const int F_SHIFT = 13;
            static const int F_RANGE = 1 << F_SHIFT;
            static const int F_ROUND = 1 << (F_SHIFT - 1);

            static const int Y_2_A = int(1.0f * 255 / (Y_HI - Y_LO) * F_RANGE + 0.5f);
            static const int U_2_B = int(2.0f * (1.0f - 0.114f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int U_2_G = -int(2.0f * 0.114f * (1.0f - 0.114f) / (1.0f - 0.299f - 0.114f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int V_2_G = -int(2.0f * 0.299f * (1.0f - 0.299f) / (1.0f - 0.299f - 0.114f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int V_2_R = int(2.0f * (1.0f - 0.299f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);

            //-------------------------------------------------------------------------------------

            static const int B_SHIFT = 14;
            static const int B_RANGE = 1 << B_SHIFT;
            static const int B_ROUND = 1 << (B_SHIFT - 1);

            static const int B_2_Y = int(0.114f * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_Y = int((1.0f - 0.299f - 0.114f) * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_Y = int(0.299f * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int B_2_U = int(0.5f * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_U = -int(0.5f * (1.0f - 0.299f - 0.114f) / (1 - 0.114f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_U = -int(0.5f * 0.299f / (1 - 0.114f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int B_2_V = -int(0.5f * 0.114f / (1 - 0.299f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_V = -int(0.5f * (1.0f - 0.299f - 0.114f) / (1 - 0.299f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_V = int(0.5f * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
        };

        /* Corresponds to BT.709 standard. Uses Kr=0.2126, Kb=0.0722. Restricts Y to range [16..235], U and V to [16..240]. */
        struct Bt709
        {
            static const int Y_LO = 16;
            static const int Y_HI = 235;
            static const int UV_LO = 16;
            static const int UV_HI = 240;
            static const int UV_Z = 128;

            static const int F_SHIFT = 13;
            static const int F_RANGE = 1 << F_SHIFT;
            static const int F_ROUND = 1 << (F_SHIFT - 1);

            static const int Y_2_A = int(1.0f * 255 / (Y_HI - Y_LO) * F_RANGE + 0.5f);
            static const int U_2_B = int(2.0f * (1.0f - 0.0722f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int U_2_G = -int(2.0f * 0.0722f * (1.0f - 0.0722f) / (1.0f - 0.2126f - 0.0722f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int V_2_G = -int(2.0f * 0.2126f * (1.0f - 0.2126f) / (1.0f - 0.2126f - 0.0722f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int V_2_R = int(2.0f * (1.0f - 0.2126f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);

            //-------------------------------------------------------------------------------------

            static const int B_SHIFT = 14;
            static const int B_RANGE = 1 << B_SHIFT;
            static const int B_ROUND = 1 << (B_SHIFT - 1);

            static const int B_2_Y = int(0.0722f * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_Y = int((1.0f - 0.2126f - 0.0722f) * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_Y = int(0.2126f * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int B_2_U = int(0.5f * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_U = -int(0.5f * (1.0f - 0.2126f - 0.0722f) / (1 - 0.0722f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_U = -int(0.5f * 0.2126f / (1 - 0.0722f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int B_2_V = -int(0.5f * 0.0722f / (1 - 0.2126f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_V = -int(0.5f * (1.0f - 0.2126f - 0.0722f) / (1 - 0.2126f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_V = int(0.5f * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
        };

        /* Corresponds to BT.2020 standard. Uses Kr=0.2627, Kb=0.0593. Restricts Y to range [16..235], U and V to [16..240]. */
        struct Bt2020
        {
            static const int Y_LO = 16;
            static const int Y_HI = 235;
            static const int UV_LO = 16;
            static const int UV_HI = 240;
            static const int UV_Z = 128;

            static const int F_SHIFT = 13;
            static const int F_RANGE = 1 << F_SHIFT;
            static const int F_ROUND = 1 << (F_SHIFT - 1);

            static const int Y_2_A = int(1.0f * 255 / (Y_HI - Y_LO) * F_RANGE + 0.5f);
            static const int U_2_B = int(2.0f * (1.0f - 0.0593f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int U_2_G = -int(2.0f * 0.0593f * (1.0f - 0.0593f) / (1.0f - 0.2627f - 0.0593f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int V_2_G = -int(2.0f * 0.2627f * (1.0f - 0.2126f) / (1.0f - 0.2627f - 0.0593f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);
            static const int V_2_R = int(2.0f * (1.0f - 0.2627f) * 255 / (UV_HI - UV_LO) * F_RANGE + 0.5f);

            //-------------------------------------------------------------------------------------

            static const int B_SHIFT = 14;
            static const int B_RANGE = 1 << B_SHIFT;
            static const int B_ROUND = 1 << (B_SHIFT - 1);

            static const int B_2_Y = int(0.0593f * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_Y = int((1.0f - 0.2627f - 0.0593f) * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_Y = int(0.2627f * (Y_HI - Y_LO) / 255 * B_RANGE + 0.5f);
            static const int B_2_U = int(0.5f * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_U = -int(0.5f * (1.0f - 0.2627f - 0.0593f) / (1 - 0.0593f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_U = -int(0.5f * 0.2627f / (1 - 0.0593f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int B_2_V = -int(0.5f * 0.0593f / (1 - 0.2627f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int G_2_V = -int(0.5f * (1.0f - 0.2627f - 0.0593f) / (1 - 0.2627f) * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
            static const int R_2_V = int(0.5f * (UV_HI - UV_LO) / 255 * B_RANGE + 0.5f);
        };

        /* Corresponds to T-REC-T.871 standard. Uses Kr=0.299, Kb=0.114. Y, U and V use full range [0..255]. */
        struct Trect871
        {
            static const int Y_LO = 0;
            static const int Y_HI = 255;
            static const int UV_LO = 0;
            static const int UV_HI = 255;
            static const int UV_Z = 128;

            static const int F_SHIFT = 14;
            static const int F_RANGE = 1 << F_SHIFT;
            static const int F_ROUND = 1 << (F_SHIFT - 1);

            static const int Y_2_A = int(1.0f * F_RANGE + 0.5f);
            static const int U_2_B = int(2.0f * (1.0f - 0.114f) * F_RANGE + 0.5f);
            static const int U_2_G = -int(2.0f * 0.114f * (1.0f - 0.114f) / (1.0f - 0.299f - 0.114f) * F_RANGE + 0.5f);
            static const int V_2_G = -int(2.0f * 0.299f * (1.0f - 0.299f) / (1.0f - 0.299f - 0.114f) * F_RANGE + 0.5f);
            static const int V_2_R = int(2.0f * (1.0f - 0.299f) * F_RANGE + 0.5f);

            //-------------------------------------------------------------------------------------

            static const int B_SHIFT = 14;
            static const int B_RANGE = 1 << B_SHIFT;
            static const int B_ROUND = 1 << (B_SHIFT - 1);

            static const int B_2_Y = int(0.114f * B_RANGE + 0.5f);
            static const int G_2_Y = int((1.0f - 0.299f - 0.114f) * B_RANGE + 0.5f);
            static const int R_2_Y = int(0.299f * B_RANGE + 0.5f);
            static const int B_2_U = int(0.5f * B_RANGE + 0.5f);
            static const int G_2_U = -int(0.5f * (1.0f - 0.299f - 0.114f) / (1 - 0.114f) * B_RANGE + 0.5f);
            static const int R_2_U = -int(0.5f * 0.299f / (1 - 0.114f) * B_RANGE + 0.5f);
            static const int B_2_V = -int(0.5f * 0.114f / (1 - 0.299f) * B_RANGE + 0.5f);
            static const int G_2_V = -int(0.5f * (1.0f - 0.299f - 0.114f) / (1 - 0.299f) * B_RANGE + 0.5f);
            static const int R_2_V = int(0.5f * B_RANGE + 0.5f);
        };

        //-----------------------------------------------------------------------------------------

        template<class T> SIMD_INLINE int YuvToBlue(int y, int u)
        {
            return RestrictRange((T::Y_2_A * (y - T::Y_LO) + T::U_2_B * (u - T::UV_Z) + T::F_ROUND) >> T::F_SHIFT);
        }

        template<class T> SIMD_INLINE int YuvToGreen(int y, int u, int v)
        {
            return RestrictRange((T::Y_2_A * (y - T::Y_LO) + T::U_2_G * (u - T::UV_Z) + T::V_2_G * (v - T::UV_Z) + T::F_ROUND) >> T::F_SHIFT);
        }

        template<class T> SIMD_INLINE int YuvToRed(int y, int v)
        {
            return RestrictRange((T::Y_2_A * (y - T::Y_LO) + T::V_2_R * (v - T::UV_Z) + T::F_ROUND) >> T::F_SHIFT);
        }

        template<class T> SIMD_INLINE void YuvToBgr(int y, int u, int v, uint8_t* bgr)
        {
            bgr[0] = YuvToBlue<T>(y, u);
            bgr[1] = YuvToGreen<T>(y, u, v);
            bgr[2] = YuvToRed<T>(y, v);
        }

        template<class T> SIMD_INLINE void YuvToBgra(int y, int u, int v, int alpha, uint8_t* bgra)
        {
            bgra[0] = YuvToBlue<T>(y, u);
            bgra[1] = YuvToGreen<T>(y, u, v);
            bgra[2] = YuvToRed<T>(y, v);
            bgra[3] = alpha;
        }

        template<class T> SIMD_INLINE void YuvToRgb(int y, int u, int v, uint8_t* rgb)
        {
            rgb[0] = YuvToRed<T>(y, v);
            rgb[1] = YuvToGreen<T>(y, u, v);
            rgb[2] = YuvToBlue<T>(y, u);
        }

        template<class T> SIMD_INLINE void YuvToRgba(int y, int u, int v, int alpha, uint8_t* rgba)
        {
            rgba[0] = YuvToRed<T>(y, v);
            rgba[1] = YuvToGreen<T>(y, u, v);
            rgba[2] = YuvToBlue<T>(y, u);
            rgba[3] = alpha;
        }

        //-------------------------------------------------------------------------------------------------

        template<class T> SIMD_INLINE int BgrToY(int blue, int green, int red)
        {
            return RestrictRange(((T::B_2_Y * blue + T::G_2_Y * green + T::R_2_Y * red + T::B_ROUND) >> T::B_SHIFT) + T::Y_LO);
        }

        template<class T> SIMD_INLINE int BgrToU(int blue, int green, int red)
        {
            return RestrictRange(((T::B_2_U * blue + T::G_2_U * green + T::R_2_U * red + T::B_ROUND) >> T::B_SHIFT) + T::UV_Z);
        }

        template<class T> SIMD_INLINE int BgrToV(int blue, int green, int red)
        {
            return RestrictRange(((T::B_2_V * blue + T::G_2_V * green + T::R_2_V * red + T::B_ROUND) >> T::B_SHIFT) + T::UV_Z);
        }
    }

#ifdef SIMD_SSE41_ENABLE    
    namespace Sse41
    {
        template<class T> SIMD_INLINE __m128i YuvToRed32(__m128i y16_1, __m128i v16_0)
        {
            static const __m128i YA_RT = SIMD_MM_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m128i VR_0 = SIMD_MM_SET2_EPI16(T::V_2_R, 0);
            return _mm_srai_epi32(_mm_add_epi32(_mm_madd_epi16(y16_1, YA_RT), _mm_madd_epi16(v16_0, VR_0)), T::F_SHIFT);
        }

        template<class T> SIMD_INLINE __m128i YuvToGreen32(__m128i y16_1, __m128i u16_v16)
        {
            static const __m128i YA_RT = SIMD_MM_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m128i UG_VG = SIMD_MM_SET2_EPI16(T::U_2_G, T::V_2_G);
            return _mm_srai_epi32(_mm_add_epi32(_mm_madd_epi16(y16_1, YA_RT), _mm_madd_epi16(u16_v16, UG_VG)), T::F_SHIFT);
        }

        template<class T> SIMD_INLINE __m128i YuvToBlue32(__m128i y16_1, __m128i u16_0)
        {
            static const __m128i YA_RT = SIMD_MM_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m128i UB_0 = SIMD_MM_SET2_EPI16(T::U_2_B, 0);
            return _mm_srai_epi32(_mm_add_epi32(_mm_madd_epi16(y16_1, YA_RT), _mm_madd_epi16(u16_0, UB_0)), T::F_SHIFT);
        }

        template <class T, int part> SIMD_INLINE __m128i UnpackY(__m128i y)
        {
            static const __m128i Y_LO = SIMD_MM_SET1_EPI16(T::Y_LO);
            return _mm_subs_epi16(UnpackU8<part>(y, K_ZERO), Y_LO);
        }

        template <class T, int part> SIMD_INLINE __m128i UnpackUV(__m128i uv)
        {
            static const __m128i UV_Z = SIMD_MM_SET1_EPI16(T::UV_Z);
            return _mm_subs_epi16(UnpackU8<part>(uv, K_ZERO), UV_Z);
        }

        template <class T> SIMD_INLINE __m128i YuvToRed16(__m128i y16, __m128i v16)
        {
            __m128i lo = YuvToRed32<T>(_mm_unpacklo_epi16(y16, K16_0001), _mm_unpacklo_epi16(v16, K_ZERO));
            __m128i hi = YuvToRed32<T>(_mm_unpackhi_epi16(y16, K16_0001), _mm_unpackhi_epi16(v16, K_ZERO));
            return SaturateI16ToU8(_mm_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m128i YuvToGreen16(__m128i y16, __m128i u16, __m128i v16)
        {
            __m128i lo = YuvToGreen32<T>(_mm_unpacklo_epi16(y16, K16_0001), _mm_unpacklo_epi16(u16, v16));
            __m128i hi = YuvToGreen32<T>(_mm_unpackhi_epi16(y16, K16_0001), _mm_unpackhi_epi16(u16, v16));
            return SaturateI16ToU8(_mm_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m128i YuvToBlue16(__m128i y16, __m128i u16)
        {
            __m128i lo = YuvToBlue32<T>(_mm_unpacklo_epi16(y16, K16_0001), _mm_unpacklo_epi16(u16, K_ZERO));
            __m128i hi = YuvToBlue32<T>(_mm_unpackhi_epi16(y16, K16_0001), _mm_unpackhi_epi16(u16, K_ZERO));
            return SaturateI16ToU8(_mm_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m128i YuvToRed(__m128i y, __m128i v)
        {
            __m128i lo = YuvToRed16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(v));
            __m128i hi = YuvToRed16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(v));
            return _mm_packus_epi16(lo, hi);
        }

        template <class T> SIMD_INLINE __m128i YuvToGreen(__m128i y, __m128i u, __m128i v)
        {
            __m128i lo = YuvToGreen16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(u), UnpackUV<T, 0>(v));
            __m128i hi = YuvToGreen16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(u), UnpackUV<T, 1>(v));
            return _mm_packus_epi16(lo, hi);
        }

        template <class T> SIMD_INLINE __m128i YuvToBlue(__m128i y, __m128i u)
        {
            __m128i lo = YuvToBlue16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(u));
            __m128i hi = YuvToBlue16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(u));
            return _mm_packus_epi16(lo, hi);
        }

        //-------------------------------------------------------------------------------------------------

        template<class T> SIMD_INLINE __m128i BgrToY32(__m128i b16_r16, __m128i g16_1)
        {
            static const __m128i BY_RY = SIMD_MM_SET2_EPI16(T::B_2_Y, T::R_2_Y);
            static const __m128i GY_RT = SIMD_MM_SET2_EPI16(T::G_2_Y, T::B_ROUND);
            return _mm_srai_epi32(_mm_add_epi32(_mm_madd_epi16(b16_r16, BY_RY), _mm_madd_epi16(g16_1, GY_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m128i BgrToY16(__m128i b16, __m128i g16, __m128i r16)
        {
            static const __m128i Y_LO = SIMD_MM_SET1_EPI16(T::Y_LO);
            return SaturateI16ToU8(_mm_add_epi16(Y_LO, _mm_packs_epi32(
                BgrToY32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToY32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m128i BgrToY8(__m128i b8, __m128i g8, __m128i r8)
        {
            return _mm_packus_epi16(
                BgrToY16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToY16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        template<class T> SIMD_INLINE __m128i BgrToU32(__m128i b16_r16, __m128i g16_1)
        {
            static const __m128i BU_RU = SIMD_MM_SET2_EPI16(T::B_2_U, T::R_2_U);
            static const __m128i GU_RT = SIMD_MM_SET2_EPI16(T::G_2_U, T::B_ROUND);
            return _mm_srai_epi32(_mm_add_epi32(_mm_madd_epi16(b16_r16, BU_RU), _mm_madd_epi16(g16_1, GU_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m128i BgrToU16(__m128i b16, __m128i g16, __m128i r16)
        {
            static const __m128i UV_Z = SIMD_MM_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm_add_epi16(UV_Z, _mm_packs_epi32(
                BgrToU32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToU32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m128i BgrToU8(__m128i b8, __m128i g8, __m128i r8)
        {
            return _mm_packus_epi16(
                BgrToU16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToU16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        template<class T> SIMD_INLINE __m128i BgrToV32(__m128i b16_r16, __m128i g16_1)
        {
            static const __m128i BV_RV = SIMD_MM_SET2_EPI16(T::B_2_V, T::R_2_V);
            static const __m128i GV_RT = SIMD_MM_SET2_EPI16(T::G_2_V, T::B_ROUND);
            return _mm_srai_epi32(_mm_add_epi32(_mm_madd_epi16(b16_r16, BV_RV), _mm_madd_epi16(g16_1, GV_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m128i BgrToV16(__m128i b16, __m128i g16, __m128i r16)
        {
            static const __m128i UV_Z = SIMD_MM_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm_add_epi16(UV_Z, _mm_packs_epi32(
                BgrToV32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToV32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m128i BgrToV8(__m128i b8, __m128i g8, __m128i r8)
        {
            return _mm_packus_epi16(
                BgrToV16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToV16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        //-------------------------------------------------------------------------------------------------

        template <bool align> SIMD_INLINE void LoadPreparedBgra16(const __m128i* bgra, __m128i& b16_r16, __m128i& g16_1)
        {
            static const __m128i BGRA_TO_B0R0 = SIMD_MM_SETR_EPI8(0x0, -1, 0x2, -1, 0x4, -1, 0x6, -1, 0x8, -1, 0xA, -1, 0xC, -1, 0xE, -1);
            static const __m128i BGRA_TO_G000 = SIMD_MM_SETR_EPI8(0x1, -1, -1, -1, 0x5, -1, -1, -1, 0x9, -1, -1, -1, 0xD, -1, -1, -1);
            __m128i _bgra = Load<align>(bgra);
            b16_r16 = _mm_shuffle_epi8(_bgra, BGRA_TO_B0R0);
            g16_1 = _mm_or_si128(_mm_shuffle_epi8(_bgra, BGRA_TO_G000), K32_00010000);
        }

        template <bool align> SIMD_INLINE void LoadPreparedBgra16(const __m128i* bgra, __m128i& b16_r16, __m128i& g16_1, __m128i& a32)
        {
            static const __m128i BGRA_TO_B0R0 = SIMD_MM_SETR_EPI8(0x0, -1, 0x2, -1, 0x4, -1, 0x6, -1, 0x8, -1, 0xA, -1, 0xC, -1, 0xE, -1);
            static const __m128i BGRA_TO_G000 = SIMD_MM_SETR_EPI8(0x1, -1, -1, -1, 0x5, -1, -1, -1, 0x9, -1, -1, -1, 0xD, -1, -1, -1);
            __m128i _bgra = Load<align>(bgra);
            b16_r16 = _mm_shuffle_epi8(_bgra, BGRA_TO_B0R0);
            g16_1 = _mm_or_si128(_mm_shuffle_epi8(_bgra, BGRA_TO_G000), K32_00010000);
            a32 = _mm_and_si128(_mm_srli_si128(_bgra, 3), K32_000000FF);
        }
    }
#endif

#ifdef SIMD_AVX2_ENABLE    
    namespace Avx2
    {
        template<class T> SIMD_INLINE __m256i YuvToRed32(__m256i y16_1, __m256i v16_0)
        {
            static const __m256i YA_RT = SIMD_MM256_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m256i VR_0 = SIMD_MM256_SET2_EPI16(T::V_2_R, 0);
            return _mm256_srai_epi32(_mm256_add_epi32(_mm256_madd_epi16(y16_1, YA_RT), _mm256_madd_epi16(v16_0, VR_0)), T::F_SHIFT);
        }

        template<class T> SIMD_INLINE __m256i YuvToGreen32(__m256i y16_1, __m256i u16_v16)
        {
            static const __m256i YA_RT = SIMD_MM256_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m256i UG_VG = SIMD_MM256_SET2_EPI16(T::U_2_G, T::V_2_G);
            return _mm256_srai_epi32(_mm256_add_epi32(_mm256_madd_epi16(y16_1, YA_RT), _mm256_madd_epi16(u16_v16, UG_VG)), T::F_SHIFT);
        }

        template<class T> SIMD_INLINE __m256i YuvToBlue32(__m256i y16_1, __m256i u16_0)
        {
            static const __m256i YA_RT = SIMD_MM256_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m256i UB_0 = SIMD_MM256_SET2_EPI16(T::U_2_B, 0);
            return _mm256_srai_epi32(_mm256_add_epi32(_mm256_madd_epi16(y16_1, YA_RT), _mm256_madd_epi16(u16_0, UB_0)), T::F_SHIFT);
        }

        template <class T, int part> SIMD_INLINE __m256i UnpackY(__m256i y)
        {
            static const __m256i Y_LO = SIMD_MM256_SET1_EPI16(T::Y_LO);
            return _mm256_subs_epi16(UnpackU8<part>(y, K_ZERO), Y_LO);
        }

        template <class T, int part> SIMD_INLINE __m256i UnpackUV(__m256i uv)
        {
            static const __m256i UV_Z = SIMD_MM256_SET1_EPI16(T::UV_Z);
            return _mm256_subs_epi16(UnpackU8<part>(uv, K_ZERO), UV_Z);
        }

        template <class T> SIMD_INLINE __m256i YuvToRed16(__m256i y16, __m256i v16)
        {
            __m256i lo = YuvToRed32<T>(_mm256_unpacklo_epi16(y16, K16_0001), _mm256_unpacklo_epi16(v16, K_ZERO));
            __m256i hi = YuvToRed32<T>(_mm256_unpackhi_epi16(y16, K16_0001), _mm256_unpackhi_epi16(v16, K_ZERO));
            return SaturateI16ToU8(_mm256_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m256i YuvToGreen16(__m256i y16, __m256i u16, __m256i v16)
        {
            __m256i lo = YuvToGreen32<T>(_mm256_unpacklo_epi16(y16, K16_0001), _mm256_unpacklo_epi16(u16, v16));
            __m256i hi = YuvToGreen32<T>(_mm256_unpackhi_epi16(y16, K16_0001), _mm256_unpackhi_epi16(u16, v16));
            return SaturateI16ToU8(_mm256_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m256i YuvToBlue16(__m256i y16, __m256i u16)
        {
            __m256i lo = YuvToBlue32<T>(_mm256_unpacklo_epi16(y16, K16_0001), _mm256_unpacklo_epi16(u16, K_ZERO));
            __m256i hi = YuvToBlue32<T>(_mm256_unpackhi_epi16(y16, K16_0001), _mm256_unpackhi_epi16(u16, K_ZERO));
            return SaturateI16ToU8(_mm256_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m256i YuvToRed(__m256i y, __m256i v)
        {
            __m256i lo = YuvToRed16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(v));
            __m256i hi = YuvToRed16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(v));
            return _mm256_packus_epi16(lo, hi);
        }

        template <class T> SIMD_INLINE __m256i YuvToGreen(__m256i y, __m256i u, __m256i v)
        {
            __m256i lo = YuvToGreen16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(u), UnpackUV<T, 0>(v));
            __m256i hi = YuvToGreen16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(u), UnpackUV<T, 1>(v));
            return _mm256_packus_epi16(lo, hi);
        }

        template <class T> SIMD_INLINE __m256i YuvToBlue(__m256i y, __m256i u)
        {
            __m256i lo = YuvToBlue16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(u));
            __m256i hi = YuvToBlue16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(u));
            return _mm256_packus_epi16(lo, hi);
        }

        //-------------------------------------------------------------------------------------------------

        template<class T> SIMD_INLINE __m256i BgrToY32(__m256i b16_r16, __m256i g16_1)
        {
            static const __m256i BY_RY = SIMD_MM256_SET2_EPI16(T::B_2_Y, T::R_2_Y);
            static const __m256i GY_RT = SIMD_MM256_SET2_EPI16(T::G_2_Y, T::B_ROUND);
            return _mm256_srai_epi32(_mm256_add_epi32(_mm256_madd_epi16(b16_r16, BY_RY), _mm256_madd_epi16(g16_1, GY_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m256i BgrToY16(__m256i b16, __m256i g16, __m256i r16)
        {
            static const __m256i Y_LO = SIMD_MM256_SET1_EPI16(T::Y_LO);
            return SaturateI16ToU8(_mm256_add_epi16(Y_LO, _mm256_packs_epi32(
                BgrToY32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToY32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m256i BgrToY8(__m256i b8, __m256i g8, __m256i r8)
        {
            return _mm256_packus_epi16(
                BgrToY16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToY16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        template<class T> SIMD_INLINE __m256i BgrToU32(__m256i b16_r16, __m256i g16_1)
        {
            static const __m256i BU_RU = SIMD_MM256_SET2_EPI16(T::B_2_U, T::R_2_U);
            static const __m256i GU_RT = SIMD_MM256_SET2_EPI16(T::G_2_U, T::B_ROUND);
            return _mm256_srai_epi32(_mm256_add_epi32(_mm256_madd_epi16(b16_r16, BU_RU), _mm256_madd_epi16(g16_1, GU_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m256i BgrToU16(__m256i b16, __m256i g16, __m256i r16)
        {
            static const __m256i UV_Z = SIMD_MM256_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm256_add_epi16(UV_Z, _mm256_packs_epi32(
                BgrToU32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToU32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m256i BgrToU8(__m256i b8, __m256i g8, __m256i r8)
        {
            return _mm256_packus_epi16(
                BgrToU16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToU16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        template<class T> SIMD_INLINE __m256i BgrToV32(__m256i b16_r16, __m256i g16_1)
        {
            static const __m256i BV_RV = SIMD_MM256_SET2_EPI16(T::B_2_V, T::R_2_V);
            static const __m256i GV_RT = SIMD_MM256_SET2_EPI16(T::G_2_V, T::B_ROUND);
            return _mm256_srai_epi32(_mm256_add_epi32(_mm256_madd_epi16(b16_r16, BV_RV), _mm256_madd_epi16(g16_1, GV_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m256i BgrToV16(__m256i b16, __m256i g16, __m256i r16)
        {
            static const __m256i UV_Z = SIMD_MM256_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm256_add_epi16(UV_Z, _mm256_packs_epi32(
                BgrToV32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToV32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m256i BgrToV8(__m256i b8, __m256i g8, __m256i r8)
        {
            return _mm256_packus_epi16(
                BgrToV16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToV16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        //-------------------------------------------------------------------------------------------------

        template <bool align> SIMD_INLINE void LoadPreparedBgra16(const __m256i* bgra, __m256i& b16_r16, __m256i& g16_1)
        {
            __m256i _bgra = Load<align>(bgra);
            b16_r16 = _mm256_and_si256(_bgra, K16_00FF);
            g16_1 = _mm256_or_si256(_mm256_and_si256(_mm256_srli_si256(_bgra, 1), K32_000000FF), K32_00010000);
        }

        template <bool align> SIMD_INLINE void LoadPreparedBgra16(const __m256i* bgra, __m256i& b16_r16, __m256i& g16_1, __m256i& a32)
        {
            __m256i _bgra = Load<align>(bgra);
            b16_r16 = _mm256_and_si256(_bgra, K16_00FF);
            g16_1 = _mm256_or_si256(_mm256_and_si256(_mm256_srli_si256(_bgra, 1), K32_000000FF), K32_00010000);
            a32 = _mm256_and_si256(_mm256_srli_si256(_bgra, 3), K32_000000FF);
        }
    }
#endif

#ifdef SIMD_AVX512BW_ENABLE    
    namespace Avx512bw
    {
        template<class T> SIMD_INLINE __m512i YuvToRed32(__m512i y16_1, __m512i v16_0)
        {
            static const __m512i YA_RT = SIMD_MM512_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m512i VR_0 = SIMD_MM512_SET2_EPI16(T::V_2_R, 0);
            return _mm512_srai_epi32(_mm512_add_epi32(_mm512_madd_epi16(y16_1, YA_RT), _mm512_madd_epi16(v16_0, VR_0)), T::F_SHIFT);
        }

        template<class T> SIMD_INLINE __m512i YuvToGreen32(__m512i y16_1, __m512i u16_v16)
        {
            static const __m512i YA_RT = SIMD_MM512_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m512i UG_VG = SIMD_MM512_SET2_EPI16(T::U_2_G, T::V_2_G);
            return _mm512_srai_epi32(_mm512_add_epi32(_mm512_madd_epi16(y16_1, YA_RT), _mm512_madd_epi16(u16_v16, UG_VG)), T::F_SHIFT);
        }

        template<class T> SIMD_INLINE __m512i YuvToBlue32(__m512i y16_1, __m512i u16_0)
        {
            static const __m512i YA_RT = SIMD_MM512_SET2_EPI16(T::Y_2_A, T::F_ROUND);
            static const __m512i UB_0 = SIMD_MM512_SET2_EPI16(T::U_2_B, 0);
            return _mm512_srai_epi32(_mm512_add_epi32(_mm512_madd_epi16(y16_1, YA_RT), _mm512_madd_epi16(u16_0, UB_0)), T::F_SHIFT);
        }

        template <class T, int part> SIMD_INLINE __m512i UnpackY(__m512i y)
        {
            static const __m512i Y_LO = SIMD_MM512_SET1_EPI16(T::Y_LO);
            return _mm512_subs_epi16(UnpackU8<part>(y, K_ZERO), Y_LO);
        }

        template <class T, int part> SIMD_INLINE __m512i UnpackUV(__m512i uv)
        {
            static const __m512i UV_Z = SIMD_MM512_SET1_EPI16(T::UV_Z);
            return _mm512_subs_epi16(UnpackU8<part>(uv, K_ZERO), UV_Z);
        }

        template <class T> SIMD_INLINE __m512i YuvToRed16(__m512i y16, __m512i v16)
        {
            __m512i lo = YuvToRed32<T>(_mm512_unpacklo_epi16(y16, K16_0001), _mm512_unpacklo_epi16(v16, K_ZERO));
            __m512i hi = YuvToRed32<T>(_mm512_unpackhi_epi16(y16, K16_0001), _mm512_unpackhi_epi16(v16, K_ZERO));
            return SaturateI16ToU8(_mm512_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m512i YuvToGreen16(__m512i y16, __m512i u16, __m512i v16)
        {
            __m512i lo = YuvToGreen32<T>(_mm512_unpacklo_epi16(y16, K16_0001), _mm512_unpacklo_epi16(u16, v16));
            __m512i hi = YuvToGreen32<T>(_mm512_unpackhi_epi16(y16, K16_0001), _mm512_unpackhi_epi16(u16, v16));
            return SaturateI16ToU8(_mm512_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m512i YuvToBlue16(__m512i y16, __m512i u16)
        {
            __m512i lo = YuvToBlue32<T>(_mm512_unpacklo_epi16(y16, K16_0001), _mm512_unpacklo_epi16(u16, K_ZERO));
            __m512i hi = YuvToBlue32<T>(_mm512_unpackhi_epi16(y16, K16_0001), _mm512_unpackhi_epi16(u16, K_ZERO));
            return SaturateI16ToU8(_mm512_packs_epi32(lo, hi));
        }

        template <class T> SIMD_INLINE __m512i YuvToRed(__m512i y, __m512i v)
        {
            __m512i lo = YuvToRed16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(v));
            __m512i hi = YuvToRed16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(v));
            return _mm512_packus_epi16(lo, hi);
        }

        template <class T> SIMD_INLINE __m512i YuvToGreen(__m512i y, __m512i u, __m512i v)
        {
            __m512i lo = YuvToGreen16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(u), UnpackUV<T, 0>(v));
            __m512i hi = YuvToGreen16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(u), UnpackUV<T, 1>(v));
            return _mm512_packus_epi16(lo, hi);
        }

        template <class T> SIMD_INLINE __m512i YuvToBlue(__m512i y, __m512i u)
        {
            __m512i lo = YuvToBlue16<T>(UnpackY<T, 0>(y), UnpackUV<T, 0>(u));
            __m512i hi = YuvToBlue16<T>(UnpackY<T, 1>(y), UnpackUV<T, 1>(u));
            return _mm512_packus_epi16(lo, hi);
        }

        template <bool align, bool mask, class T> SIMD_INLINE void YuvToBgra(const __m512i& y, const __m512i& u,
            const __m512i& v, const __m512i& a, uint8_t* bgra, const __mmask64* tails)
        {
            __m512i b = _mm512_permutexvar_epi32(K32_PERMUTE_FOR_TWO_UNPACK, YuvToBlue<T>(y, u));
            __m512i g = _mm512_permutexvar_epi32(K32_PERMUTE_FOR_TWO_UNPACK, YuvToGreen<T>(y, u, v));
            __m512i r = _mm512_permutexvar_epi32(K32_PERMUTE_FOR_TWO_UNPACK, YuvToRed<T>(y, v));
            __m512i bg0 = UnpackU8<0>(b, g);
            __m512i bg1 = UnpackU8<1>(b, g);
            __m512i ra0 = UnpackU8<0>(r, a);
            __m512i ra1 = UnpackU8<1>(r, a);
            Store<align, mask>(bgra + 0 * A, UnpackU16<0>(bg0, ra0), tails[0]);
            Store<align, mask>(bgra + 1 * A, UnpackU16<1>(bg0, ra0), tails[1]);
            Store<align, mask>(bgra + 2 * A, UnpackU16<0>(bg1, ra1), tails[2]);
            Store<align, mask>(bgra + 3 * A, UnpackU16<1>(bg1, ra1), tails[3]);
        }

        //-------------------------------------------------------------------------------------------------

        template<class T> SIMD_INLINE __m512i BgrToY32(__m512i b16_r16, __m512i g16_1)
        {
            static const __m512i BY_RY = SIMD_MM512_SET2_EPI16(T::B_2_Y, T::R_2_Y);
            static const __m512i GY_RT = SIMD_MM512_SET2_EPI16(T::G_2_Y, T::B_ROUND);
            return _mm512_srai_epi32(_mm512_add_epi32(_mm512_madd_epi16(b16_r16, BY_RY), _mm512_madd_epi16(g16_1, GY_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m512i BgrToY16(__m512i b16, __m512i g16, __m512i r16)
        {
            static const __m512i Y_LO = SIMD_MM512_SET1_EPI16(T::Y_LO);
            return SaturateI16ToU8(_mm512_add_epi16(Y_LO, _mm512_packs_epi32(
                BgrToY32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToY32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m512i BgrToY16(__m512i b16_r16[2], __m512i g16_1[2])
        {
            static const __m512i Y_LO = SIMD_MM512_SET1_EPI16(T::Y_LO);
            return SaturateI16ToU8(_mm512_add_epi16(Y_LO, _mm512_packs_epi32(
                BgrToY32<T>(b16_r16[0], g16_1[0]), BgrToY32<T>(b16_r16[1], g16_1[1]))));
        }

        template<class T> SIMD_INLINE __m512i BgrToY8(__m512i b8, __m512i g8, __m512i r8)
        {
            return _mm512_packus_epi16(
                BgrToY16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToY16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        template<class T> SIMD_INLINE __m512i BgrToU32(__m512i b16_r16, __m512i g16_1)
        {
            static const __m512i BU_RU = SIMD_MM512_SET2_EPI16(T::B_2_U, T::R_2_U);
            static const __m512i GU_RT = SIMD_MM512_SET2_EPI16(T::G_2_U, T::B_ROUND);
            return _mm512_srai_epi32(_mm512_add_epi32(_mm512_madd_epi16(b16_r16, BU_RU), _mm512_madd_epi16(g16_1, GU_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m512i BgrToU16(__m512i b16, __m512i g16, __m512i r16)
        {
            static const __m512i UV_Z = SIMD_MM512_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm512_add_epi16(UV_Z, _mm512_packs_epi32(
                BgrToU32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToU32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m512i BgrToU16(__m512i b16_r16[2], __m512i g16_1[2])
        {
            static const __m512i UV_Z = SIMD_MM512_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm512_add_epi16(UV_Z, _mm512_packs_epi32(
                BgrToU32<T>(b16_r16[0], g16_1[0]), BgrToU32<T>(b16_r16[1], g16_1[1]))));
        }

        template<class T> SIMD_INLINE __m512i BgrToU8(__m512i b8, __m512i g8, __m512i r8)
        {
            return _mm512_packus_epi16(
                BgrToU16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToU16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        template<class T> SIMD_INLINE __m512i BgrToV32(__m512i b16_r16, __m512i g16_1)
        {
            static const __m512i BV_RV = SIMD_MM512_SET2_EPI16(T::B_2_V, T::R_2_V);
            static const __m512i GV_RT = SIMD_MM512_SET2_EPI16(T::G_2_V, T::B_ROUND);
            return _mm512_srai_epi32(_mm512_add_epi32(_mm512_madd_epi16(b16_r16, BV_RV), _mm512_madd_epi16(g16_1, GV_RT)), T::B_SHIFT);
        }

        template<class T> SIMD_INLINE __m512i BgrToV16(__m512i b16, __m512i g16, __m512i r16)
        {
            static const __m512i UV_Z = SIMD_MM512_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm512_add_epi16(UV_Z, _mm512_packs_epi32(
                BgrToV32<T>(UnpackU16<0>(b16, r16), UnpackU16<0>(g16, K16_0001)),
                BgrToV32<T>(UnpackU16<1>(b16, r16), UnpackU16<1>(g16, K16_0001)))));
        }

        template<class T> SIMD_INLINE __m512i BgrToV16(__m512i b16_r16[2], __m512i g16_1[2])
        {
            static const __m512i UV_Z = SIMD_MM512_SET1_EPI16(T::UV_Z);
            return SaturateI16ToU8(_mm512_add_epi16(UV_Z, _mm512_packs_epi32(
                BgrToV32<T>(b16_r16[0], g16_1[0]), BgrToV32<T>(b16_r16[1], g16_1[1]))));
        }

        template<class T> SIMD_INLINE __m512i BgrToV8(__m512i b8, __m512i g8, __m512i r8)
        {
            return _mm512_packus_epi16(
                BgrToV16<T>(UnpackU8<0>(b8), UnpackU8<0>(g8), UnpackU8<0>(r8)),
                BgrToV16<T>(UnpackU8<1>(b8), UnpackU8<1>(g8), UnpackU8<1>(r8)));
        }

        //-------------------------------------------------------------------------------------------------

        template <bool align, bool mask> SIMD_INLINE void LoadPreparedBgra16(const uint8_t* bgra, __m512i& b16_r16, __m512i& g16_1, const __mmask64* tails)
        {
            __m512i _bgra = Load<align, mask>(bgra, tails[0]);
            b16_r16 = _mm512_and_si512(_bgra, K16_00FF);
            g16_1 = _mm512_or_si512(_mm512_shuffle_epi8(_bgra, K8_SUFFLE_BGRA_TO_G000), K32_00010000);
        }

        template <bool align, bool mask> SIMD_INLINE void LoadPreparedBgra16(const uint8_t* bgra, __m512i& b16_r16, __m512i& g16_1, __m512i& a32, const __mmask64* tails)
        {
            __m512i _bgra = Load<align, mask>(bgra, tails[0]);
            b16_r16 = _mm512_and_si512(_bgra, K16_00FF);
            g16_1 = _mm512_or_si512(_mm512_shuffle_epi8(_bgra, K8_SUFFLE_BGRA_TO_G000), K32_00010000);
            a32 = _mm512_shuffle_epi8(_bgra, K8_SUFFLE_BGRA_TO_A000);
        }
    }
#endif

#ifdef SIMD_NEON_ENABLE
    namespace Neon
    {
        template <class T, int part> SIMD_INLINE int32x4_t YuvToRed(int16x8_t y, int16x8_t v)
        {
            static const int16x4_t YA = SIMD_VEC_SET1_PI16(T::Y_2_A);
            static const int16x4_t VR = SIMD_VEC_SET1_PI16(T::V_2_R);
            static const int32x4_t RT = SIMD_VEC_SET1_PI16(T::F_ROUND);
            return vshrq_n_s32(vmlal_s16(vmlal_s16(RT, Half<part>(y), YA), Half<part>(v), VR), T::F_SHIFT);
        }

        template <class T> SIMD_INLINE int16x8_t YuvToRed(int16x8_t y, int16x8_t v)
        {
            return PackI32(YuvToRed<T, 0>(y, v), YuvToRed<T, 1>(y, v));
        }

        template <class T, int part> SIMD_INLINE int32x4_t YuvToGreen(int16x8_t y, int16x8_t u, int16x8_t v)
        {
            static const int16x4_t YA = SIMD_VEC_SET1_PI16(T::Y_2_A);
            static const int16x4_t UG = SIMD_VEC_SET1_PI16(T::U_2_G);
            static const int16x4_t VG = SIMD_VEC_SET1_PI16(T::V_2_G);
            static const int32x4_t RT = SIMD_VEC_SET1_PI16(T::F_ROUND);
            return vshrq_n_s32(vmlal_s16(vmlal_s16(vmlal_s16(RT, Half<part>(y), YA), Half<part>(u), UG), Half<part>(v), VG), T::F_SHIFT);
        }

        template <class T> SIMD_INLINE int16x8_t YuvToGreen(int16x8_t y, int16x8_t u, int16x8_t v)
        {
            return PackI32(YuvToGreen<T, 0>(y, u, v), YuvToGreen<T, 1>(y, u, v));
        }

        template <class T, int part> SIMD_INLINE int32x4_t YuvToBlue(int16x8_t y, int16x8_t u)
        {
            static const int16x4_t YA = SIMD_VEC_SET1_PI16(T::Y_2_A);
            static const int16x4_t UB = SIMD_VEC_SET1_PI16(T::U_2_B);
            static const int32x4_t RT = SIMD_VEC_SET1_PI16(T::F_ROUND);
            return vshrq_n_s32(vmlal_s16(vmlal_s16(RT, Half<part>(y), YA), Half<part>(u), UB), T::F_SHIFT);
        }

        template <class T> SIMD_INLINE int16x8_t YuvToBlue(int16x8_t y, int16x8_t u)
        {
            return PackI32(YuvToBlue<T, 0>(y, u), YuvToBlue<T, 1>(y, u));
        }

        template <class T> SIMD_INLINE int16x8_t AdjustY(uint8x8_t y)
        {
            static const int16x8_t Y_LO = SIMD_VEC_SET1_EPI16(T::Y_LO);
            return vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(y)), Y_LO);
        }

        template <class T> SIMD_INLINE int16x8_t AdjustUV(uint8x8_t uv)
        {
            static const int16x8_t UV_Z = SIMD_VEC_SET1_EPI16(T::UV_Z);
            return vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(uv)), UV_Z);
        }

        template <class T, int part> SIMD_INLINE int16x8_t UnpackY(uint8x16_t y)
        {
            return AdjustY<T>(Half<part>(y));
        }

        template <class T, int part> SIMD_INLINE int16x8_t UnpackUV(uint8x16_t uv)
        {
            return AdjustUV<T>(Half<part>(uv));
        }

        template <class T> SIMD_INLINE void YuvToBgr(uint8x16_t y, uint8x16_t u, uint8x16_t v, uint8x16x3_t& bgr)
        {
            int16x8_t yLo = UnpackY<T, 0>(y), uLo = UnpackUV<T, 0>(u), vLo = UnpackUV<T, 0>(v);
            int16x8_t yHi = UnpackY<T, 1>(y), uHi = UnpackUV<T, 1>(u), vHi = UnpackUV<T, 1>(v);
            bgr.val[0] = PackSaturatedI16(YuvToBlue<T>(yLo, uLo), YuvToBlue<T>(yHi, uHi));
            bgr.val[1] = PackSaturatedI16(YuvToGreen<T>(yLo, uLo, vLo), YuvToGreen<T>(yHi, uHi, vHi));
            bgr.val[2] = PackSaturatedI16(YuvToRed<T>(yLo, vLo), YuvToRed<T>(yHi, vHi));
        }

        template <class T> SIMD_INLINE void YuvToBgra(uint8x16_t y, uint8x16_t u, uint8x16_t v, uint8x16_t a, uint8x16x4_t& bgra)
        {
            int16x8_t yLo = UnpackY<T, 0>(y), uLo = UnpackUV<T, 0>(u), vLo = UnpackUV<T, 0>(v);
            int16x8_t yHi = UnpackY<T, 1>(y), uHi = UnpackUV<T, 1>(u), vHi = UnpackUV<T, 1>(v);
            bgra.val[0] = PackSaturatedI16(YuvToBlue<T>(yLo, uLo), YuvToBlue<T>(yHi, uHi));
            bgra.val[1] = PackSaturatedI16(YuvToGreen<T>(yLo, uLo, vLo), YuvToGreen<T>(yHi, uHi, vHi));
            bgra.val[2] = PackSaturatedI16(YuvToRed<T>(yLo, vLo), YuvToRed<T>(yHi, vHi));
            bgra.val[3] = a;
        }

        template <class T> SIMD_INLINE void YuvToRgb(uint8x16_t y, uint8x16_t u, uint8x16_t v, uint8x16x3_t& rgb)
        {
            int16x8_t yLo = UnpackY<T, 0>(y), uLo = UnpackUV<T, 0>(u), vLo = UnpackUV<T, 0>(v);
            int16x8_t yHi = UnpackY<T, 1>(y), uHi = UnpackUV<T, 1>(u), vHi = UnpackUV<T, 1>(v);
            rgb.val[0] = PackSaturatedI16(YuvToRed<T>(yLo, vLo), YuvToRed<T>(yHi, vHi));
            rgb.val[1] = PackSaturatedI16(YuvToGreen<T>(yLo, uLo, vLo), YuvToGreen<T>(yHi, uHi, vHi));
            rgb.val[2] = PackSaturatedI16(YuvToBlue<T>(yLo, uLo), YuvToBlue<T>(yHi, uHi));
        }

        //-------------------------------------------------------------------------------------------------

        template <class T, int part> SIMD_INLINE int32x4_t BgrToY32(uint16x8_t blue, uint16x8_t green, uint16x8_t red)
        {
            static const int16x4_t BY = SIMD_VEC_SET1_PI16(T::B_2_Y);
            static const int16x4_t GY = SIMD_VEC_SET1_PI16(T::G_2_Y);
            static const int16x4_t RY = SIMD_VEC_SET1_PI16(T::R_2_Y);
            static const int32x4_t RT = SIMD_VEC_SET1_EPI32(T::B_ROUND);
            return vshrq_n_s32(vmlal_s16(vmlal_s16(vmlal_s16(RT, vreinterpret_s16_u16(Half<part>(blue)), BY),
                vreinterpret_s16_u16(Half<part>(green)), GY), vreinterpret_s16_u16(Half<part>(red)), RY), T::B_SHIFT);
        }

        template <class T> SIMD_INLINE int16x8_t BgrToY16(uint16x8_t blue, uint16x8_t green, uint16x8_t red)
        {
            static const int16x8_t Y_LO = SIMD_VEC_SET1_EPI16(T::Y_LO);
            return vaddq_s16(Y_LO, PackI32(BgrToY32<T, 0>(blue, green, red), BgrToY32<T, 1>(blue, green, red)));
        }

        template <class T> SIMD_INLINE uint8x16_t BgrToY8(uint8x16_t blue, uint8x16_t green, uint8x16_t red)
        {
            return PackSaturatedI16(
                BgrToY16<T>(UnpackU8<0>(blue), UnpackU8<0>(green), UnpackU8<0>(red)),
                BgrToY16<T>(UnpackU8<1>(blue), UnpackU8<1>(green), UnpackU8<1>(red)));
        }

        template <class T, int part> SIMD_INLINE int32x4_t BgrToU32(uint16x8_t blue, uint16x8_t green, uint16x8_t red)
        {
            static const int16x4_t BU = SIMD_VEC_SET1_PI16(T::B_2_U);
            static const int16x4_t GU = SIMD_VEC_SET1_PI16(T::G_2_U);
            static const int16x4_t RU = SIMD_VEC_SET1_PI16(T::R_2_U);
            static const int32x4_t RT = SIMD_VEC_SET1_EPI32(T::B_ROUND);
            return vshrq_n_s32(vmlal_s16(vmlal_s16(vmlal_s16(RT, vreinterpret_s16_u16(Half<part>(blue)), BU),
                vreinterpret_s16_u16(Half<part>(green)), GU), vreinterpret_s16_u16(Half<part>(red)), RU), T::B_SHIFT);
        }

        template <class T> SIMD_INLINE int16x8_t BgrToU16(uint16x8_t blue, uint16x8_t green, uint16x8_t red)
        {
            static const int16x8_t UV_Z = SIMD_VEC_SET1_EPI16(T::UV_Z);
            return vaddq_s16(UV_Z, PackI32(BgrToU32<T, 0>(blue, green, red), BgrToU32<T, 1>(blue, green, red)));
        }

        template <class T> SIMD_INLINE uint8x16_t BgrToU8(uint8x16_t blue, uint8x16_t green, uint8x16_t red)
        {
            return PackSaturatedI16(
                BgrToU16<T>(UnpackU8<0>(blue), UnpackU8<0>(green), UnpackU8<0>(red)),
                BgrToU16<T>(UnpackU8<1>(blue), UnpackU8<1>(green), UnpackU8<1>(red)));
        }

        template <class T, int part> SIMD_INLINE int32x4_t BgrToV32(uint16x8_t blue, uint16x8_t green, uint16x8_t red)
        {
            static const int16x4_t BV = SIMD_VEC_SET1_PI16(T::B_2_V);
            static const int16x4_t GV = SIMD_VEC_SET1_PI16(T::G_2_V);
            static const int16x4_t RV = SIMD_VEC_SET1_PI16(T::R_2_V);
            static const int32x4_t RT = SIMD_VEC_SET1_EPI32(T::B_ROUND);
            return vshrq_n_s32(vmlal_s16(vmlal_s16(vmlal_s16(RT, vreinterpret_s16_u16(Half<part>(blue)), BV),
                vreinterpret_s16_u16(Half<part>(green)), GV), vreinterpret_s16_u16(Half<part>(red)), RV), T::B_SHIFT);
        }

        template <class T> SIMD_INLINE int16x8_t BgrToV16(uint16x8_t blue, uint16x8_t green, uint16x8_t red)
        {
            static const int16x8_t UV_Z = SIMD_VEC_SET1_EPI16(T::UV_Z);
            return vaddq_s16(UV_Z, PackI32(BgrToV32<T, 0>(blue, green, red), BgrToV32<T, 1>(blue, green, red)));
        }

        template <class T> SIMD_INLINE uint8x16_t BgrToV8(uint8x16_t blue, uint8x16_t green, uint8x16_t red)
        {
            return PackSaturatedI16(
                BgrToV16<T>(UnpackU8<0>(blue), UnpackU8<0>(green), UnpackU8<0>(red)),
                BgrToV16<T>(UnpackU8<1>(blue), UnpackU8<1>(green), UnpackU8<1>(red)));
        }
    }
#endif
}

#endif//__SimdYuvToBgr_h__
