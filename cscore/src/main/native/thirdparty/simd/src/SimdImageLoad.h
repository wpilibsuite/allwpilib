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
#ifndef __SimdImageLoad_h__
#define __SimdImageLoad_h__

#include "Simd/SimdMemoryStream.h"
#include "Simd/SimdArray.h"
#include "Simd/SimdAlignment.h"

#include "Simd/SimdView.hpp"

#include <vector>

namespace Simd
{
    typedef uint8_t* (*ImageLoadFromMemoryPtr)(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format);

    uint8_t* ImageLoadFromFile(const ImageLoadFromMemoryPtr loader, const char* path, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format);

    //-------------------------------------------------------------------------

    struct ImageLoaderParam
    {
        const uint8_t* data;
        size_t size;
        SimdImageFileType file;
        SimdPixelFormatType format;

        ImageLoaderParam(const uint8_t* d, size_t s, SimdPixelFormatType f);

        bool Validate();
    };

    //-------------------------------------------------------------------------------------------------

    class ImageLoader
    {
    protected:
        typedef Simd::View<Simd::Allocator> Image;

        ImageLoaderParam _param;
        InputMemoryStream _stream;
        Image _image;
        
    public:
        ImageLoader(const ImageLoaderParam& param)
            : _param(param)
            , _stream(_param.data, _param.size)
        {
        }

        virtual ~ImageLoader()
        {
        }

        virtual bool FromStream() = 0;

        SIMD_INLINE uint8_t* Release(size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format)
        {
            *stride = _image.stride;
            *width = _image.width;
            *height = _image.height;
            *format = (SimdPixelFormatType)_image.format;
            return _image.Release();
        }
    };

    //-------------------------------------------------------------------------------------------------

    namespace Base
    {

        //-------------------------------------------------------------------------------------------------

        class ImageJpegLoader : public ImageLoader
        {
        public:
            ImageJpegLoader(const ImageLoaderParam& param);

            virtual ~ImageJpegLoader();

            virtual bool FromStream();

        protected:
            struct JpegContext* _context;
        };

        //-------------------------------------------------------------------------------------------------

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageLoadFromMemory(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format);
    }

#ifdef SIMD_SSE41_ENABLE    
    namespace Sse41
    {

        class ImageJpegLoader : public Base::ImageJpegLoader
        {
        public:
            ImageJpegLoader(const ImageLoaderParam& param);
        };

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageLoadFromMemory(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format);
    }
#endif

#ifdef SIMD_AVX2_ENABLE    
    namespace Avx2
    {

        class ImageJpegLoader : public Sse41::ImageJpegLoader
        {
        public:
            ImageJpegLoader(const ImageLoaderParam& param);

            virtual bool FromStream();
        };

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageLoadFromMemory(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format);
    }
#endif

#ifdef SIMD_AVX512BW_ENABLE    
    namespace Avx512bw
    {

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageLoadFromMemory(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format);
    }
#endif

#ifdef SIMD_NEON_ENABLE    
    namespace Neon
    {

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageLoadFromMemory(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format);
    }
#endif
}

#endif
