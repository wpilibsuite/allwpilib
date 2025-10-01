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
#include "Simd/SimdResizer.h"
#include "Simd/SimdResizerCommon.h"
#include "Simd/SimdCopy.h"

namespace Simd
{
#ifdef SIMD_SSE41_ENABLE
    namespace Sse41
    {
        ResizerNearest::ResizerNearest(const ResParam& param)
            : Base::ResizerNearest(param)
            , _blocks(0)
        {
        }

        size_t ResizerNearest::BlockCountMax(size_t align)
        {
            return (size_t)::ceil(float(Simd::Max(_param.srcW, _param.dstW) * _param.PixelSize()) / (align - _param.PixelSize()));
        }

        void ResizerNearest::EstimateParams()
        {
            if (_blocks)
                return;
            Base::ResizerNearest::EstimateParams();
            const size_t pixelSize = _param.PixelSize();
            if (pixelSize *_param.dstW < A || pixelSize * _param.srcW < A)
                return;
            if (pixelSize < 4 && _param.srcW < 4 * _param.dstW)
                _blocks = BlockCountMax(A);
            float scale = (float)_param.srcW / _param.dstW;
            if (_blocks)
            {
                _tails = 0;
                _ix16x1.Resize(_blocks);
                _tail16x1.Resize((size_t)::ceil(A * scale / pixelSize));
                size_t dstRowSize = _param.dstW * pixelSize;
                int block = 0;
                _ix16x1[0].src = 0;
                _ix16x1[0].dst = 0;
                for (int dstIndex = 0; dstIndex < (int)_param.dstW; ++dstIndex)
                {
                    int srcIndex = _ix[dstIndex] / (int)pixelSize;
                    int dst = dstIndex * (int)pixelSize - _ix16x1[block].dst;
                    int src = srcIndex * (int)pixelSize - _ix16x1[block].src;
                    if (src >= int(A - pixelSize) || dst >= int(A - pixelSize))
                    {
                        block++;
                        _ix16x1[block].src = srcIndex * (int)pixelSize;
                        _ix16x1[block].dst = dstIndex * (int)pixelSize;
                        if (_ix16x1[block].dst > int(dstRowSize - A))
                        {
                            _tail16x1[_tails] = LeftNotZero8i(dstRowSize - _ix16x1[block].dst);
                            _tails++;
                        }
                        dst = 0;
                        src = srcIndex * (int)pixelSize - _ix16x1[block].src;
                    }
                    for(size_t i = 0; i < pixelSize; ++i)
                        _ix16x1[block].shuffle[dst + i] = uint8_t(src + i);
                }
                _blocks = block + 1;
            }
        }

        void ResizerNearest::Shuffle16x1(const uint8_t* src, size_t srcStride, uint8_t* dst, size_t dstStride)
        {
            size_t body = _blocks - _tails;
            for (size_t dy = 0; dy < _param.dstH; dy++)
            {
                const uint8_t* srcRow = src + _iy[dy] * srcStride;
                size_t i = 0, t = 0;
                for (; i < body; ++i)
                {
                   const IndexShuffle16x1& index = _ix16x1[i];
                    __m128i _src = _mm_loadu_si128((__m128i*)(srcRow + index.src));
                    __m128i _shuffle = _mm_loadu_si128((__m128i*) & index.shuffle);
                    _mm_storeu_si128((__m128i*)(dst + index.dst), _mm_shuffle_epi8(_src, _shuffle));
                }
                for (; i < _blocks; ++i, t++)
                {
                    const IndexShuffle16x1& index = _ix16x1[i];
                    __m128i _src = _mm_loadu_si128((__m128i*)(srcRow + index.src));
                    __m128i _shuffle = _mm_loadu_si128((__m128i*) & index.shuffle);
                    StoreMasked<false>((__m128i*)(dst + index.dst), _mm_shuffle_epi8(_src, _shuffle), _tail16x1[t]);
                }
                dst += dstStride;
            }
        }

        SIMD_INLINE void CopyPixel12(const uint8_t* src, uint8_t* dst)
        {
            __m128i val = _mm_loadu_si128((__m128i*)src);
            _mm_storeu_si128((__m128i*)dst, val);
        }
        
        void ResizerNearest::Resize12(const uint8_t* src, size_t srcStride, uint8_t* dst, size_t dstStride)
        {
            size_t body = _param.dstW - 1;
            for (size_t dy = 0; dy < _param.dstH; dy++)
            {
                const uint8_t* srcRow = src + _iy[dy] * srcStride;
                size_t dx = 0, offset = 0;
                for (; dx < body; dx++, offset += 12)
                    CopyPixel12(srcRow + _ix[dx], dst + offset);
                Base::CopyPixel<12>(srcRow + _ix[dx], dst + offset);
                dst += dstStride;
            }
        }

        void ResizerNearest::Run(const uint8_t* src, size_t srcStride, uint8_t* dst, size_t dstStride)
        {
            EstimateParams();
            if (_blocks)
                Shuffle16x1(src, srcStride, dst, dstStride);
            else if (_pixelSize == 12)
                Resize12(src, srcStride, dst, dstStride);
            else
                Base::ResizerNearest::Run(src, srcStride, dst, dstStride);
        }
    }
#endif
}

