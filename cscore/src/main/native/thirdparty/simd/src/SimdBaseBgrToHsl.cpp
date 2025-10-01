/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2017 Yermalayeu Ihar.
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
#include "Simd/SimdConversion.h"

namespace Simd
{
    namespace Base
    {
        void BgrToHsl(const uint8_t * bgr, size_t width, size_t height, size_t bgrStride, uint8_t * hsl, size_t hslStride)
        {
            for (size_t row = 0; row < height; ++row)
            {
                const uint8_t * pBgr = bgr + row*bgrStride;
                uint8_t * pHsl = hsl + row*hslStride;
                for (const uint8_t * pBgrEnd = pBgr + width * 3; pBgr < pBgrEnd; pBgr += 3, pHsl += 3)
                {
                    BgrToHsl(pBgr[0], pBgr[1], pBgr[2], pHsl);
                }
            }
        }
    }
}
