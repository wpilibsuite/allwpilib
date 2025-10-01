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
#ifndef __SimdImageSave_h__
#define __SimdImageSave_h__

#include "Simd/SimdMemoryStream.h"
#include "Simd/SimdArray.h"
#include "Simd/SimdPerformance.h"

namespace Simd
{
    typedef uint8_t* (*ImageSaveToMemoryPtr)(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);

    SimdBool ImageSaveToFile(const ImageSaveToMemoryPtr saver, const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, const char* path);

    //-------------------------------------------------------------------------------------------------

    struct ImageSaverParam
    {
        size_t width, height;
        SimdPixelFormatType format;
        SimdImageFileType file;
        int quality;
        SimdYuvType yuvType;

        SIMD_INLINE ImageSaverParam(size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality)
        {
            this->width = width;
            this->height = height;
            this->format = format;
            this->file = file;
            this->quality = quality;
            this->yuvType = SimdYuvUnknown;
        }

        SIMD_INLINE ImageSaverParam(size_t width, size_t height, int quality, SimdYuvType yuvType)
        {
            this->width = width;
            this->height = height;
            this->format = SimdPixelFormatNone;
            this->file = SimdImageFileJpeg;
            this->quality = quality;
            this->yuvType = yuvType;
        }

        SIMD_INLINE bool Validate()
        {
            if (file == SimdImageFileUndefined)
            {
                if (format == SimdPixelFormatGray8)
                    file = SimdImageFilePgmBin;
                else
                    file = SimdImageFilePpmBin;
            } 
            if (width == 0 || height == 0)
                return false;
            if (yuvType == SimdYuvUnknown)
            {
                if (format < SimdPixelFormatGray8 || format > SimdPixelFormatRgba32)
                    return false;
            }
            else
            {
                if (yuvType != SimdYuvTrect871)
                    return false;
                if (width % 2 != 0 || height % 2 != 0)
                    return false;
            }
            if (file <= SimdImageFileUndefined || file > SimdImageFileBmp)
                return false;
            return true;
        }
    };

    //-------------------------------------------------------------------------------------------------

    class ImageSaver
    {
    protected:
        ImageSaverParam _param;
        OutputMemoryStream _stream;
    public:
        ImageSaver(const ImageSaverParam& param)
            : _param(param)
        {
        }

        virtual ~ImageSaver()
        {
        }

        virtual bool ToStream(const uint8_t* src, size_t stride) = 0;

        SIMD_INLINE uint8_t* Release(size_t* size)
        {
            return _stream.Release(size);
        }
    };

    //-------------------------------------------------------------------------------------------------
       
    namespace Base
    {

        class ImageJpegSaver : public ImageSaver
        {
        public:
            ImageJpegSaver(const ImageSaverParam& param);

            virtual bool ToStream(const uint8_t* src, size_t stride);

            virtual bool ToStream(const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride);

            virtual bool ToStream(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride);
        protected:
            typedef void (*DeintBgrPtr)(const uint8_t* bgr, size_t bgrStride, size_t width, size_t height,
                uint8_t* b, size_t bStride, uint8_t* g, size_t gStride, uint8_t* r, size_t rStride);
            typedef void (*DeintBgraPtr)(const uint8_t* bgra, size_t bgraStride, size_t width, size_t height,
                uint8_t* b, size_t bStride, uint8_t* g, size_t gStride, uint8_t* r, size_t rStride, uint8_t* a, size_t aStride);
            typedef void (*WriteBlockPtr)(OutputMemoryStream& stream, int width, int height, const uint8_t* red,
                const uint8_t* green, const uint8_t* blue, int stride, const float* fY, const float* fUv, int dc[3]);
            typedef void (*WriteNv12BlockPtr)(OutputMemoryStream& stream, int width, int height, const uint8_t* y, 
                int yStride, const uint8_t* uv, int uvStride, const float* fY, const float* fUv, int dc[3]);
            typedef void (*WriteYuv420pBlockPtr)(OutputMemoryStream& stream, int width, int height, const uint8_t* y, int yStride, 
                const uint8_t* u, int uStride, const uint8_t* v, int vStride, const float* fY, const float* fUv, int dc[3]);

            Array8u _buffer;
            DeintBgrPtr _deintBgr;
            DeintBgraPtr _deintBgra;
            WriteBlockPtr _writeBlock;
            WriteNv12BlockPtr _writeNv12Block;
            WriteYuv420pBlockPtr _writeYuv420pBlock;
            bool _subSample;
            int _quality, _block, _width;
            float _fY[64], _fUv[64];
            uint8_t _uY[64], _uUv[64];

            virtual void Init();

            void InitParams(bool trans);
            void WriteHeader();
        };

        //-------------------------------------------------------------------------------------------------

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);

        uint8_t* Nv12SaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);

        uint8_t* Yuv420pSaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);
    }

#ifdef SIMD_SSE41_ENABLE    
    namespace Sse41
    {

        class ImageJpegSaver : public Base::ImageJpegSaver
        {
        public:
            ImageJpegSaver(const ImageSaverParam& param);

        protected:
            virtual void Init();
        };

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);

        uint8_t* Nv12SaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);

        uint8_t* Yuv420pSaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);
    }
#endif

#ifdef SIMD_AVX2_ENABLE    
    namespace Avx2
    {

        class ImageJpegSaver : public Sse41::ImageJpegSaver
        {
        public:
            ImageJpegSaver(const ImageSaverParam& param);

        protected:
            virtual void Init();
        };

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);

        uint8_t* Nv12SaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);

        uint8_t* Yuv420pSaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);
    }
#endif

#ifdef SIMD_AVX512BW_ENABLE    
    namespace Avx512bw
    {

        class ImageJpegSaver : public Avx2::ImageJpegSaver
        {
        public:
            ImageJpegSaver(const ImageSaverParam& param);

        protected:
            virtual void Init();
        };

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);

        uint8_t* Nv12SaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);

        uint8_t* Yuv420pSaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);
    }
#endif

#ifdef SIMD_NEON_ENABLE    
    namespace Neon
    {

        class ImageJpegSaver : public Base::ImageJpegSaver
        {
        public:
            ImageJpegSaver(const ImageSaverParam& param);

        protected:
            virtual void Init();
        };

        //-------------------------------------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);

        uint8_t* Nv12SaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* uv, size_t uvStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);

        uint8_t* Yuv420pSaveAsJpegToMemory(const uint8_t* y, size_t yStride, const uint8_t* u, size_t uStride, const uint8_t* v, size_t vStride, size_t width, size_t height, SimdYuvType yuvType, int quality, size_t* size);
    }
#endif
}

#endif
