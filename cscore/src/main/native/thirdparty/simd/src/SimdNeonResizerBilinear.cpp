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
#include "Simd/SimdResizer.h"
#include "Simd/SimdResizerCommon.h"
#include "Simd/SimdUpdate.h"

namespace Simd
{
#ifdef SIMD_NEON_ENABLE    
    namespace Neon
    {
        ResizerByteBilinear::ResizerByteBilinear(const ResParam & param)
            : Base::ResizerByteBilinear(param)
            , _blocks(0)
        {
        }

        size_t ResizerByteBilinear::BlockCountMax(size_t align)
        {
            return (size_t)Simd::Max(::ceil(float(_param.srcW) / (align - 1)), ::ceil(float(_param.dstW) * 2.0f / align));
        }

        void ResizerByteBilinear::EstimateParams()
        {
            if (_ax.data)
                return;
            if (_param.channels == 1 && _param.srcW < 4 * _param.dstW)
                _blocks = BlockCountMax(A);
            float scale = (float)_param.srcW / _param.dstW;
            _ax.Resize(_param.dstW * _param.channels * 2, false, _param.align);
            uint8_t * alphas = _ax.data;
            if (_blocks)
            {
                _ixg.Resize(_blocks);
                int block = 0;
                _ixg[0].src = 0;
                _ixg[0].dst = 0;
                for (int dstIndex = 0; dstIndex < _param.dstW; ++dstIndex)
                {
                    float alpha = (float)((dstIndex + 0.5)*scale - 0.5);
                    int srcIndex = (int)::floor(alpha);
                    alpha -= srcIndex;

                    if (srcIndex < 0)
                    {
                        srcIndex = 0;
                        alpha = 0;
                    }

                    if (srcIndex > _param.srcW - 2)
                    {
                        srcIndex = (int)_param.srcW - 2;
                        alpha = 1;
                    }

                    int dst = 2 * dstIndex - _ixg[block].dst;
                    int src = srcIndex - _ixg[block].src;
                    if (src >= A - 1 || dst >= A)
                    {
                        block++;
                        _ixg[block].src = Simd::Min(srcIndex, int(_param.srcW - A));
                        _ixg[block].dst = 2 * dstIndex;
                        dst = 0;
                        src = srcIndex - _ixg[block].src;
                    }
                    _ixg[block].shuffle[dst] = src;
                    _ixg[block].shuffle[dst + 1] = src + 1;

                    alphas[1] = (uint8_t)(alpha * Base::FRACTION_RANGE + 0.5);
                    alphas[0] = (uint8_t)(Base::FRACTION_RANGE - alphas[1]);
                    alphas += 2;
                }
                _blocks = block + 1;
            }
            else
            {
                _ix.Resize(_param.dstW);
                for (size_t i = 0; i < _param.dstW; ++i)
                {
                    float alpha = (float)((i + 0.5)*scale - 0.5);
                    ptrdiff_t index = (ptrdiff_t)::floor(alpha);
                    alpha -= index;

                    if (index < 0)
                    {
                        index = 0;
                        alpha = 0;
                    }

                    if (index >(ptrdiff_t)_param.srcW - 2)
                    {
                        index = _param.srcW - 2;
                        alpha = 1;
                    }

                    _ix[i] = (int)index;
                    alphas[1] = (uint8_t)(alpha * Base::FRACTION_RANGE + 0.5);
                    alphas[0] = (uint8_t)(Base::FRACTION_RANGE - alphas[1]);
                    for (size_t channel = 1; channel < _param.channels; channel++)
                        ((uint16_t*)alphas)[channel] = *(uint16_t*)alphas;
                    alphas += 2 * _param.channels;
                }
            }
            size_t size = AlignHi(_param.dstW, _param.align)*_param.channels * 2;
            _bx[0].Resize(size, false, _param.align);
            _bx[1].Resize(size, false, _param.align);
}

        template <size_t N> void ResizerByteBilinearInterpolateX(const uint8_t * alpha, uint8_t * buffer);

        template <> SIMD_INLINE void ResizerByteBilinearInterpolateX<1>(const uint8_t * alpha, uint8_t * buffer)
        {
            uint8x8x2_t a = vld2_u8(alpha);
            uint8x8x2_t b = vld2_u8(buffer);
            Store<true>(buffer, (uint8x16_t)vaddq_u16(vmull_u8(a.val[0], b.val[0]), vmull_u8(a.val[1], b.val[1])));
        }

        SIMD_INLINE void ResizerByteBilinearInterpolateX2(const uint8_t * alpha, uint8_t * buffer)
        {
            uint8x8x2_t a = vld2_u8(alpha);
            uint16x4x2_t b = vld2_u16((uint16_t*)buffer);
            Store<true>(buffer, (uint8x16_t)vaddq_u16(vmull_u8(a.val[0], (uint8x8_t)b.val[0]), vmull_u8(a.val[1], (uint8x8_t)b.val[1])));
        }

        template <> SIMD_INLINE void ResizerByteBilinearInterpolateX<2>(const uint8_t * alpha, uint8_t * buffer)
        {
            ResizerByteBilinearInterpolateX2(alpha + 0, buffer + 0);
            ResizerByteBilinearInterpolateX2(alpha + A, buffer + A);
        }

        SIMD_INLINE void ResizerByteBilinearInterpolateX3(const uint8_t * alpha, const uint8_t * src, uint8_t * dst)
        {
            uint8x8x2_t a = vld2_u8(alpha);
            uint8x8x2_t b = vld2_u8(src);
            Store<true>(dst, (uint8x16_t)vaddq_u16(vmull_u8(a.val[0], b.val[0]), vmull_u8(a.val[1], b.val[1])));
        }

        template <> SIMD_INLINE void ResizerByteBilinearInterpolateX<3>(const uint8_t * alpha, uint8_t * buffer)
        {
            uint8_t b[3 * A];
            uint8x16x3_t _b = vld3q_u8(buffer);
            vst3q_u16((uint16_t*)b, *(uint16x8x3_t*)&_b);
            ResizerByteBilinearInterpolateX3(alpha + 0 * A, b + 0 * A, buffer + 0 * A);
            ResizerByteBilinearInterpolateX3(alpha + 1 * A, b + 1 * A, buffer + 1 * A);
            ResizerByteBilinearInterpolateX3(alpha + 2 * A, b + 2 * A, buffer + 2 * A);
        }

        SIMD_INLINE void ResizerByteBilinearInterpolateX4(const uint8_t * alpha, uint8_t * buffer)
        {
            uint8x8x2_t a = vld2_u8(alpha);
            uint32x2x2_t b = vld2_u32((uint32_t*)buffer);
            Store<true>(buffer, (uint8x16_t)vaddq_u16(vmull_u8(a.val[0], (uint8x8_t)b.val[0]), vmull_u8(a.val[1], (uint8x8_t)b.val[1])));
        }

        template <> SIMD_INLINE void ResizerByteBilinearInterpolateX<4>(const uint8_t * alpha, uint8_t * buffer)
        {
            ResizerByteBilinearInterpolateX4(alpha + 0 * A, buffer + 0 * A);
            ResizerByteBilinearInterpolateX4(alpha + 1 * A, buffer + 1 * A);
            ResizerByteBilinearInterpolateX4(alpha + 2 * A, buffer + 2 * A);
            ResizerByteBilinearInterpolateX4(alpha + 3 * A, buffer + 3 * A);
        }

        const uint16x8_t K16_FRACTION_ROUND_TERM = SIMD_VEC_SET1_EPI16(Base::BILINEAR_ROUND_TERM);

        template<bool align> SIMD_INLINE uint16x8_t ResizerByteBilinearInterpolateY(const uint16_t * pbx0, const uint16_t * pbx1, uint16x8_t alpha[2])
        {
            uint16x8_t sum = vaddq_u16(vmulq_u16(Load<align>(pbx0), alpha[0]), vmulq_u16(Load<align>(pbx1), alpha[1]));
            return vshrq_n_u16(vaddq_u16(sum, K16_FRACTION_ROUND_TERM), Base::BILINEAR_SHIFT);
        }

        template<bool align> SIMD_INLINE void ResizerByteBilinearInterpolateY(const uint8_t * bx0, const uint8_t * bx1, uint16x8_t alpha[2], uint8_t * dst)
        {
            uint16x8_t lo = ResizerByteBilinearInterpolateY<align>((uint16_t*)(bx0 + 0), (uint16_t*)(bx1 + 0), alpha);
            uint16x8_t hi = ResizerByteBilinearInterpolateY<align>((uint16_t*)(bx0 + A), (uint16_t*)(bx1 + A), alpha);
            Store<false>(dst, PackU16(lo, hi));
        }

        template<size_t N> void ResizerByteBilinear::Run(const uint8_t * src, size_t srcStride, uint8_t * dst, size_t dstStride)
        {
            struct One { uint8_t val[N * 1]; };
            struct Two { uint8_t val[N * 2]; };

            size_t size = 2 * _param.dstW*N;
            size_t aligned = AlignHi(size, DA) - DA;
            const size_t step = A * N;
            ptrdiff_t previous = -2;
            uint16x8_t a[2];
            uint8_t * bx[2] = { _bx[0].data, _bx[1].data };
            const uint8_t * ax = _ax.data;
            const int32_t * ix = _ix.data;
            size_t dstW = _param.dstW;

            for (size_t yDst = 0; yDst < _param.dstH; yDst++, dst += dstStride)
            {
                a[0] = vdupq_n_u16(int16_t(Base::FRACTION_RANGE - _ay[yDst]));
                a[1] = vdupq_n_u16(int16_t(_ay[yDst]));

                ptrdiff_t sy = _iy[yDst];
                int k = 0;

                if (sy == previous)
                    k = 2;
                else if (sy == previous + 1)
                {
                    Swap(bx[0], bx[1]);
                    k = 1;
                }

                previous = sy;

                for (; k < 2; k++)
                {
                    Two * pb = (Two *)bx[k];
                    const One * psrc = (const One *)(src + (sy + k)*srcStride);
                    for (size_t x = 0; x < dstW; x++)
                        pb[x] = *(Two *)(psrc + ix[x]);

                    uint8_t * pbx = bx[k];
                    for (size_t i = 0; i < size; i += step)
                        ResizerByteBilinearInterpolateX<N>(ax + i, pbx + i);
                }

                for (size_t ib = 0, id = 0; ib < aligned; ib += DA, id += A)
                    ResizerByteBilinearInterpolateY<true>(bx[0] + ib, bx[1] + ib, a, dst + id);
                size_t i = size - DA;
                ResizerByteBilinearInterpolateY<false>(bx[0] + i, bx[1] + i, a, dst + i / 2);
            }
        }

        union ResizerByteBilinearLoadGrayInterpolatedHelper
        {
            uint8x16_t full;
            uint8x8x2_t half;
        }; 

        template <class Idx> SIMD_INLINE void ResizerByteBilinearLoadGrayInterpolated(const uint8_t * src, const Idx & index, const uint8_t * alpha, uint8_t * dst)
        {
            ResizerByteBilinearLoadGrayInterpolatedHelper _src, _shuffle, _alpha, unpacked;
            _src.full = vld1q_u8(src + index.src);
            _shuffle.full = vld1q_u8(index.shuffle);
            unpacked.half.val[0] = vtbl2_u8(_src.half, _shuffle.half.val[0]);
            unpacked.half.val[1] = vtbl2_u8(_src.half, _shuffle.half.val[1]);
            _alpha.full = vld1q_u8(alpha + index.dst);
            uint16x8_t lo = vmull_u8(unpacked.half.val[0], _alpha.half.val[0]);
            uint16x8_t hi = vmull_u8(unpacked.half.val[1], _alpha.half.val[1]);
            vst1q_u8(dst + index.dst, vreinterpretq_u8_u16(Hadd16u(lo, hi)));
        }

        template <class Idx> SIMD_INLINE void ResizerByteBilinearLoadGray(const uint8_t * src, const Idx & index, uint8_t * dst)
        {
            ResizerByteBilinearLoadGrayInterpolatedHelper _src, _shuffle, _alpha, unpacked;
            _src.full = vld1q_u8(src + index.src);
            _shuffle.full = vld1q_u8(index.shuffle);
            unpacked.half.val[0] = vtbl2_u8(_src.half, _shuffle.half.val[0]);
            unpacked.half.val[1] = vtbl2_u8(_src.half, _shuffle.half.val[1]);
            vst1q_u8(dst + index.dst, unpacked.full);
        }

//#define MERGE_LOADING_AND_INTERPOLATION

        void ResizerByteBilinear::RunG(const uint8_t * src, size_t srcStride, uint8_t * dst, size_t dstStride)
        {
            size_t bufW = AlignHi(_param.dstW, A) * 2;
            size_t size = 2 * _param.dstW;
            size_t aligned = AlignHi(size, DA) - DA;
            size_t blocks = _blocks;
            ptrdiff_t previous = -2;
            uint16x8_t a[2];
            uint8_t * bx[2] = { _bx[0].data, _bx[1].data };
            const uint8_t * ax = _ax.data;
            const Idx * ixg = _ixg.data;

            for (size_t yDst = 0; yDst < _param.dstH; yDst++, dst += dstStride)
            {
                a[0] = vdupq_n_u16(int16_t(Base::FRACTION_RANGE - _ay[yDst]));
                a[1] = vdupq_n_u16(int16_t(_ay[yDst]));

                ptrdiff_t sy = _iy[yDst];
                int k = 0;

                if (sy == previous)
                    k = 2;
                else if (sy == previous + 1)
                {
                    Swap(bx[0], bx[1]);
                    k = 1;
                }

                previous = sy;

                for (; k < 2; k++)
                {
#ifdef MERGE_LOADING_AND_INTERPOLATION
                    const uint8_t * psrc = src + (sy + k)*srcStride;
                    uint8_t * pdst = bx[k];
                    for (size_t i = 0; i < blocks; ++i)
                        ResizerByteBilinearLoadGrayInterpolated(psrc, ixg[i], ax, pdst);
#else
                    const uint8_t * psrc = src + (sy + k)*srcStride;
                    uint8_t * pdst = bx[k];
                    for (size_t i = 0; i < blocks; ++i)
                        ResizerByteBilinearLoadGray(psrc, ixg[i], pdst);

                    uint8_t * pbx = bx[k];
                    for (size_t i = 0; i < size; i += A)
                        ResizerByteBilinearInterpolateX<1>(ax + i, pbx + i);
#endif
                }

                for (size_t ib = 0, id = 0; ib < aligned; ib += DA, id += A)
                    ResizerByteBilinearInterpolateY<true>(bx[0] + ib, bx[1] + ib, a, dst + id);
                size_t i = size - DA;
                ResizerByteBilinearInterpolateY<false>(bx[0] + i, bx[1] + i, a, dst + i / 2);
            }
        }

        void ResizerByteBilinear::Run(const uint8_t * src, size_t srcStride, uint8_t * dst, size_t dstStride)
        {
            assert(_param.dstW >= A);

            EstimateParams();
            switch (_param.channels)
            {
            case 1:
                if (_blocks)
                    RunG(src, srcStride, dst, dstStride);
                else
                    Run<1>(src, srcStride, dst, dstStride);
                break;
            case 2: Run<2>(src, srcStride, dst, dstStride); break;
            case 3: Run<3>(src, srcStride, dst, dstStride); break;
            case 4: Run<4>(src, srcStride, dst, dstStride); break;
            default:
                assert(0);
            }
        }

        //-------------------------------------------------------------------------------------------------

    }
#endif// SIMD_NEON_ENABLE
}
