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
#include "Simd/SimdMemory.h"
#include "Simd/SimdResizer.h"
#include "Simd/SimdCopy.h"

namespace Simd
{
    namespace Base
    {
        ResizerByteBilinear::ResizerByteBilinear(const ResParam & param)
            : Resizer(param)
        {
            _ay.Resize(_param.dstH);
            _iy.Resize(_param.dstH);
            EstimateIndexAlpha(_param.srcH, _param.dstH, 1, _iy.data, _ay.data);
        }        
        
        void ResizerByteBilinear::EstimateIndexAlpha(size_t srcSize, size_t dstSize, size_t channels, int32_t * indices, int32_t * alphas)
        {
            float scale = (float)srcSize / dstSize;

            for (size_t i = 0; i < dstSize; ++i)
            {
                float alpha = (float)((i + 0.5f)*scale - 0.5f);
                ptrdiff_t index = (ptrdiff_t)::floor(alpha);
                alpha -= index;

                if (index < 0)
                {
                    index = 0;
                    alpha = 0;
                }

                if (index >(ptrdiff_t)srcSize - 2)
                {
                    index = srcSize - 2;
                    alpha = 1;
                }

                for (size_t c = 0; c < channels; c++)
                {
                    size_t offset = i * channels + c;
                    indices[offset] = (int32_t)(channels*index + c);
                    alphas[offset] = (int32_t)(alpha * FRACTION_RANGE + 0.5f);
                }
            }
        }        

        void ResizerByteBilinear::Run(const uint8_t * src, size_t srcStride, uint8_t * dst, size_t dstStride)
        {
            size_t cn =  _param.channels;
            size_t rs = _param.dstW * cn;
            if (_ax.data == 0)
            {
                _ax.Resize(rs);
                _ix.Resize(rs);
                EstimateIndexAlpha(_param.srcW, _param.dstW, cn, _ix.data, _ax.data);
                _bx[0].Resize(rs);
                _bx[1].Resize(rs);
            }
            int32_t * pbx[2] = { _bx[0].data, _bx[1].data };
            int32_t prev = -2;
            for (size_t dy = 0; dy < _param.dstH; dy++, dst += dstStride)
            {
                int32_t fy = _ay[dy];
                int32_t sy = _iy[dy];
                int32_t k = 0;

                if (sy == prev)
                    k = 2;
                else if (sy == prev + 1)
                {
                    Swap(pbx[0], pbx[1]);
                    k = 1;
                }

                prev = sy;

                for (; k < 2; k++)
                {
                    int32_t * pb = pbx[k];
                    const uint8_t * ps = src + (sy + k)*srcStride;
                    for (size_t dx = 0; dx < rs; dx++)
                    {
                        int32_t sx = _ix[dx];
                        int32_t fx = _ax[dx];
                        int32_t t = ps[sx];
                        pb[dx] = (t << LINEAR_SHIFT) + (ps[sx + cn] - t)*fx;
                    }
                }

                if (fy == 0)
                    for (size_t dx = 0; dx < rs; dx++)
                        dst[dx] = ((pbx[0][dx] << LINEAR_SHIFT) + BILINEAR_ROUND_TERM) >> BILINEAR_SHIFT;
                else if (fy == FRACTION_RANGE)
                    for (size_t dx = 0; dx < rs; dx++)
                        dst[dx] = ((pbx[1][dx] << LINEAR_SHIFT) + BILINEAR_ROUND_TERM) >> BILINEAR_SHIFT;
                else
                {
                    for (size_t dx = 0; dx < rs; dx++)
                    {
                        int32_t t = pbx[0][dx];
                        dst[dx] = ((t << LINEAR_SHIFT) + (pbx[1][dx] - t)*fy + BILINEAR_ROUND_TERM) >> BILINEAR_SHIFT;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------------------------------

    }
}

