/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdarg.h>

#include "Vision/BaeUtilities.h"
#include "Vision/FrcError.h"
#include "Vision/VisionAPI.h"

int VisionAPI_debugFlag = 1;
#define DPRINTF \
  if (VisionAPI_debugFlag) dprintf

/** @file
 *    Image Management functions
 */

/**
* @brief Create an image object
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL, IMAQ_IMAGE_COMPLEX,
* IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL, IMAQ_IMAGE_RGB_U64
* The border size is defaulted to 3 so that convolutional algorithms work at the
* edges.
* When you are finished with the created image, dispose of it by calling
* frcDispose().
* To get extended error information, call GetLastError().
*
* @param type Type of image to create
* @return Image* On success, this function returns the created image. On
* failure, it returns nullptr.
*/
Image* frcCreateImage(ImageType type) {
  return imaqCreateImage(type, DEFAULT_BORDER_SIZE);
}

/**
* @brief Dispose of one object. Supports any object created on the heap.
*
* @param object object to dispose of
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcDispose(void* object) { return imaqDispose(object); }
/**
* @brief Dispose of a list of objects. Supports any object created on the heap.
*
* @param functionName The name of the function
* @param ... A list of pointers to structures that need to be disposed of.
* The last pointer in the list should always be set to nullptr.
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcDispose(const char* functionName, ...) /* Variable argument list */
{
  va_list disposalPtrList; /* Input argument list */
  void* disposalPtr;       /* For iteration */
  int success, returnValue = 1;

  va_start(disposalPtrList, functionName); /* start of variable list */
  disposalPtr = va_arg(disposalPtrList, void*);
  while (disposalPtr != nullptr) {
    success = imaqDispose(disposalPtr);
    if (!success) {
      returnValue = 0;
    }
    disposalPtr = va_arg(disposalPtrList, void*);
  }
  va_end(disposalPtrList);
  return returnValue;
}

/**
* @brief Copy an image object.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL, IMAQ_IMAGE_RGB,
* IMAQ_IMAGE_HSL.
*
* @param dest Copy of image. On failure, dest is nullptr. Must have already been
* created using frcCreateImage().
* When you are finished with the created image, dispose of it by calling
* frcDispose().
* @param source Image to copy
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcCopyImage(Image* dest, const Image* source) {
  return imaqDuplicate(dest, source);
}

/**
* @brief Crop image without changing the scale.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL, IMAQ_IMAGE_RGB,
* IMAQ_IMAGE_HSL.
*
* @param dest Modified image
* @param source Image to crop
* @param rect region to process, or IMAQ_NO_RECT
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcCrop(Image* dest, const Image* source, Rect rect) {
  return imaqScale(dest, source, 1, 1, IMAQ_SCALE_LARGER, rect);
}

/**
* @brief Scales the entire image larger or smaller.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL, IMAQ_IMAGE_RGB,
* IMAQ_IMAGE_HSL.
*
* @param dest Modified image
* @param source Image to scale
* @param xScale the horizontal reduction ratio
* @param yScale the vertical reduction ratio
* @param scaleMode IMAQ_SCALE_LARGER or IMAQ_SCALE_SMALLER
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcScale(Image* dest, const Image* source, int xScale, int yScale,
             ScalingMode scaleMode) {
  Rect rect = IMAQ_NO_RECT;
  return imaqScale(dest, source, xScale, yScale, scaleMode, rect);
}

/**
 * @brief Creates image object from the information in a file. The file can be
 * in one of the following formats:
 * PNG, JPEG, JPEG2000, TIFF, AIPD, or BMP.
 * Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL, IMAQ_IMAGE_COMPLEX,
 * IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL, IMAQ_IMAGE_RGB_U64.
 *
 * @param image Image read in
 * @param fileName File to read. Cannot be nullptr
 *
 * @return On success: 1. On failure: 0. To get extended error information, call
 * GetLastError().
 */
int frcReadImage(Image* image, const char* fileName) {
  return imaqReadFile(image, fileName, nullptr, nullptr);
}

/**
* @brief Write image to a file.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL, IMAQ_IMAGE_COMPLEX,
* IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL, IMAQ_IMAGE_RGB_U64.
*
* The file type is determined by the extension, as follows:
*
* 		Extension 					File Type
* 		aipd or .apd 				AIPD
* 		.bmp 						BMP
* 		.jpg or .jpeg 				JPEG
* 		.jp2 						JPEG2000
* 		.png 						PNG
* 		.tif or .tiff 				TIFF
*
*
* The following are the supported image types for each file type:
*
* 		File Types 					Image Types
* 		AIPD 						all image types
* 		BMP, JPEG 					8-bit, RGB
* 		PNG, TIFF, JPEG2000 		8-bit, 16-bit, RGB, RGBU64
*
* @param image Image to write
* @param fileName File to read. Cannot be nullptr. The extension determines the
* file format that is written.
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcWriteImage(const Image* image, const char* fileName) {
  RGBValue* colorTable = nullptr;
  return imaqWriteFile(image, fileName, colorTable);
}

/*  Measure Intensity functions */

/**
* @brief Measures the pixel intensities in a rectangle of an image.
* Outputs intensity based statistics about an image such as Max, Min, Mean and
* Std Dev of pixel value.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL.
*
*  Parameter Discussion	:
* 		Relevant parameters of the HistogramReport include:
* 			min, max, mean and stdDev
*		min/max —Setting both min and max to 0 causes the function to set
* min to 0
* 			and the max to 255 for 8-bit images and to the actual
* minimum value and
* 			maximum value of the image for all other image types.
*		max—Setting both min and max to 0 causes the function to set max
* to 255
* 			for 8-bit images and to the actual maximum value of the
* image for
* 			all other image types.
*
* @param image Image whose histogram the function calculates.
* @param numClasses The number of classes into which the function separates the
* pixels.
* Determines the number of elements in the histogram array returned
* @param min The minimum pixel value to consider for the histogram.
* The function does not count pixels with values less than min.
* @param max The maximum pixel value to consider for the histogram.
* The function does not count pixels with values greater than max.
* @param rect Region of interest in the image. If not included, the entire image
* is used.
* @return On success, this function returns a report describing the pixel value
* classification.
* When you are finished with the report, dispose of it by calling frcDispose().
* On failure, this function returns nullptr. To get extended error information,
* call GetLastError().
*
*/
HistogramReport* frcHistogram(const Image* image, int numClasses, float min,
                              float max) {
  Rect rect = IMAQ_NO_RECT;
  return frcHistogram(image, numClasses, min, max, rect);
}
HistogramReport* frcHistogram(const Image* image, int numClasses, float min,
                              float max, Rect rect) {
  int success;
  int fillValue = 1;

  /* create the region of interest */
  ROI* pRoi = imaqCreateROI();
  success = imaqAddRectContour(pRoi, rect);
  if (!success) {
    GetLastVisionError();
    return nullptr;
  }

  /* make a mask from the ROI */
  Image* pMask = frcCreateImage(IMAQ_IMAGE_U8);
  success = imaqROIToMask(pMask, pRoi, fillValue, nullptr, nullptr);
  if (!success) {
    GetLastVisionError();
    frcDispose(__FUNCTION__, pRoi, nullptr);
    return nullptr;
  }

  /* get a histogram report */
  HistogramReport* pHr = nullptr;
  pHr = imaqHistogram(image, numClasses, min, max, pMask);

  /* clean up */
  frcDispose(__FUNCTION__, pRoi, pMask, nullptr);

  return pHr;
}

/**
* @brief Calculates the histogram, or pixel distribution, of a color image.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL.
*
* @param image Image whose histogram the function calculates.
* @param numClasses The number of classes into which the function separates the
* pixels.
* Determines the number of elements in the histogram array returned
* @param mode The color space in which to perform the histogram. Possible values
* include IMAQ_RGB and IMAQ_HSL.
* @param mask An optional mask image. This image must be an IMAQ_IMAGE_U8 image.
* The function calculates the histogram using only those pixels in the image
* whose
* corresponding pixels in the mask are non-zero. Set this parameter to nullptr to
* calculate
* the histogram of the entire image, or use the simplified call.
*
* @return On success, this function returns a report describing the
* classification
* of each plane in a HistogramReport.
* When you are finished with the report, dispose of it by calling frcDispose().
* On failure, this function returns nullptr.
* To get extended error information, call imaqGetLastError().
*/
ColorHistogramReport* frcColorHistogram(const Image* image, int numClasses,
                                        ColorMode mode) {
  return frcColorHistogram(image, numClasses, mode, nullptr);
}

ColorHistogramReport* frcColorHistogram(const Image* image, int numClasses,
                                        ColorMode mode, Image* mask) {
  return imaqColorHistogram2((Image*)image, numClasses, mode, nullptr, mask);
}

/**
* @brief Measures the pixel intensities in a rectangle of an image.
* Outputs intensity based statistics about an image such as Max, Min, Mean and
* Std Dev of pixel value.
* Supports IMAQ_IMAGE_U8 (grayscale) IMAQ_IMAGE_RGB (color) IMAQ_IMAGE_HSL
* (color-HSL).
*
* @param image The image whose pixel value the function queries
* @param pixel The coordinates of the pixel that the function queries
* @param value On return, the value of the specified image pixel. This parameter
* cannot be nullptr.
* This data structure contains either grayscale, RGB, HSL, Complex or
* RGBU64Value depending on the type of image.
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcGetPixelValue(const Image* image, Point pixel, PixelValue* value) {
  return imaqGetPixel(image, pixel, value);
}

/*   Particle Analysis functions */

/**
* @brief Filters particles out of an image based on their measurements.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL.
*
* @param dest The destination image. If dest is used, it must be the same size
* as the Source image. It will contain only the filtered particles.
* @param source The image containing the particles to filter.
* @param criteria An array of criteria to apply to the particles in the source
* image. This array cannot be nullptr.
* See the NIVisionCVI.chm help file for definitions of criteria.
* @param criteriaCount The number of elements in the criteria array.
* @param options Binary filter options, including rejectMatches, rejectBorder,
* and connectivity8.
* @param rect Area of image to filter. If omitted, the default is entire image.
* @param numParticles On return, the number of particles left in the image
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcParticleFilter(Image* dest, Image* source,
                      const ParticleFilterCriteria2* criteria,
                      int criteriaCount, const ParticleFilterOptions* options,
                      int* numParticles) {
  Rect rect = IMAQ_NO_RECT;
  return frcParticleFilter(dest, source, criteria, criteriaCount, options, rect,
                           numParticles);
}

int frcParticleFilter(Image* dest, Image* source,
                      const ParticleFilterCriteria2* criteria,
                      int criteriaCount, const ParticleFilterOptions* options,
                      Rect rect, int* numParticles) {
  ROI* roi = imaqCreateROI();
  imaqAddRectContour(roi, rect);
  return imaqParticleFilter3(dest, source, criteria, criteriaCount, options,
                             roi, numParticles);
}

/**
* @brief Performs morphological transformations on binary images.
* Supports IMAQ_IMAGE_U8.
*
* @param dest The destination image. The border size of the destination image is
* not important.
* @param source The image on which the function performs the morphological
* operations. The calculation
* modifies the border of the source image. The border must be at least half as
* large as the larger
* dimension  of the structuring element.  The connected source image for a
* morphological transformation
* must have been created with a border capable of supporting the size of the
* structuring element.
* A 3 by 3 structuring element requires a minimal border of 1, a 5 by 5
* structuring element requires a minimal border of 2, and so on.
* @param method The morphological transform to apply.
* @param structuringElement The structuring element used in the operation. Omit
* this parameter if you do not want a custom structuring element.
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcMorphology(Image* dest, Image* source, MorphologyMethod method) {
  return imaqMorphology(dest, source, method, nullptr);
}

int frcMorphology(Image* dest, Image* source, MorphologyMethod method,
                  const StructuringElement* structuringElement) {
  return imaqMorphology(dest, source, method, structuringElement);
}

/**
* @brief Eliminates particles that touch the border of the image.
* Supports IMAQ_IMAGE_U8.
*
* @param dest The destination image.
* @param source The source image. If the image has a border, the function sets
* all border pixel values to 0.
* @param connectivity8 specifies the type of connectivity used by the algorithm
* for particle detection.
* The connectivity mode directly determines whether an adjacent pixel belongs to
* the same particle or a
* different particle. Set to TRUE to use connectivity-8 to determine whether
* particles are touching
* Set to FALSE to use connectivity-4 to determine whether particles are
* touching.
* The default setting for the simplified call is TRUE
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcRejectBorder(Image* dest, Image* source) {
  return imaqRejectBorder(dest, source, TRUE);
}

int frcRejectBorder(Image* dest, Image* source, int connectivity8) {
  return imaqRejectBorder(dest, source, connectivity8);
}

/**
* @brief Counts the number of particles in a binary image.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL.
* @param image binary (thresholded) image
* @param numParticles On return, the number of particles.
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcCountParticles(Image* image, int* numParticles) {
  return imaqCountParticles(image, 1, numParticles);
}

/**
* @brief Conduct measurements for a single particle in an images.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16, IMAQ_IMAGE_SGL.
*
* @param image image with the particle to analyze. This function modifies the
* source image.
* If you need the original image, create a copy of the image using frcCopy()
* before using this function.
* @param particleNumber The number of the particle to get information on
* @param par on return, a particle analysis report containing information about
* the particle. This structure must be created by the caller.
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcParticleAnalysis(Image* image, int particleNumber,
                        ParticleAnalysisReport* par) {
  int success = 0;

  /* image information */
  int height, width;
  if (!imaqGetImageSize(image, &width, &height)) {
    return success;
  }
  par->imageWidth = width;
  par->imageHeight = height;
  par->particleIndex = particleNumber;

  /* center of mass point of the largest particle	*/
  double returnDouble;
  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_CENTER_OF_MASS_X, &returnDouble);
  if (!success) {
    return success;
  }
  par->center_mass_x = (int)returnDouble;  // pixel

  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_CENTER_OF_MASS_Y, &returnDouble);
  if (!success) {
    return success;
  }
  par->center_mass_y = (int)returnDouble;  // pixel

  /* particle size statistics */
  success = imaqMeasureParticle(image, particleNumber, 0, IMAQ_MT_AREA,
                                &returnDouble);
  if (!success) {
    return success;
  }
  par->particleArea = returnDouble;

  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_BOUNDING_RECT_TOP, &returnDouble);
  if (!success) {
    return success;
  }
  par->boundingRect.top = (int)returnDouble;

  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_BOUNDING_RECT_LEFT, &returnDouble);
  if (!success) {
    return success;
  }
  par->boundingRect.left = (int)returnDouble;

  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_BOUNDING_RECT_HEIGHT, &returnDouble);
  if (!success) {
    return success;
  }
  par->boundingRect.height = (int)returnDouble;

  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_BOUNDING_RECT_WIDTH, &returnDouble);
  if (!success) {
    return success;
  }
  par->boundingRect.width = (int)returnDouble;

  /* particle quality statistics */
  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_AREA_BY_IMAGE_AREA, &returnDouble);
  if (!success) {
    return success;
  }
  par->particleToImagePercent = returnDouble;

  success = imaqMeasureParticle(image, particleNumber, 0,
                                IMAQ_MT_AREA_BY_PARTICLE_AND_HOLES_AREA,
                                &returnDouble);
  if (!success) {
    return success;
  }
  par->particleQuality = returnDouble;

  /* normalized position (-1 to 1) */
  par->center_mass_x_normalized = RangeToNormalized(par->center_mass_x, width);
  par->center_mass_y_normalized = RangeToNormalized(par->center_mass_y, height);

  return success;
}

/*   Image Enhancement functions */

/**
* @brief Improves contrast on a grayscale image.
* Supports IMAQ_IMAGE_U8, IMAQ_IMAGE_I16.
* @param dest The destination image.
* @param source The image to equalize
* @param min the smallest value used for processing. After processing, all pixel
* values that are less than or equal to the Minimum in the original image are set
* to 0 for an 8-bit image. In 16-bit and floating-point images, these pixel
* values are set to the smallest pixel value found in the original image.
* @param max the largest value used for processing. After processing, all pixel
* values that are greater than or equal to the Maximum in the original image are
* set to 255 for an 8-bit image. In 16-bit and floating-point images, these pixel
* values are set to the largest pixel value found in the original image.
* @param mask an 8-bit image that specifies the region of the small image that
* will be copied. Only those pixels in the Image Src (Small) image that
* correspond to an equivalent non-zero pixel in the mask image are copied. All
* other pixels keep their original values. The entire image is processed if Image
* Mask is nullptr or this parameter is omitted.
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*
*  option defaults:
*       searchRect = IMAQ_NO_RECT
* 		minMatchScore = DEFAULT_MINMAX_SCORE (800)
*/
int frcEqualize(Image* dest, const Image* source, float min, float max) {
  return frcEqualize(dest, source, min, max, nullptr);
}

int frcEqualize(Image* dest, const Image* source, float min, float max,
                const Image* mask) {
  return imaqEqualize(dest, source, min, max, mask);
}

/**
* @brief Improves contrast on a color image.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL
*
* option defaults: colorEqualization = TRUE to equalize all three planes of the
* image
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
* @param dest The destination image.
* @param source The image to equalize
* @param colorEqualization Set this parameter to TRUE to equalize all three
* planes of the image (the default). Set this parameter to FALSE to equalize only
* the luminance plane.
*/
int frcColorEqualize(Image* dest, const Image* source) {
  return imaqColorEqualize(dest, source, TRUE);
}

int frcColorEqualize(Image* dest, const Image* source, int colorEqualization) {
  return imaqColorEqualize(dest, source, TRUE);
}

/*   Image Conversion functions */

/**
* @brief Automatically thresholds a grayscale image into a binary image for
* Particle Analysis based on a smart threshold.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_I16
* @param dest The destination image.
* @param source The image to threshold
* @param windowWidth The width of the rectangular window around the pixel on
* which the function
*  performs the local threshold. This number must be at least 3 and cannot be
* larger than the width of source
* @param windowHeight The height of the rectangular window around the pixel on
* which the function
* performs the local threshold. This number must be at least 3 and cannot be
* larger than the height of source
* @param method Specifies the local thresholding method the function uses. Value
* can be IMAQ_NIBLACK
* (which computes thresholds for each pixel based on its local statistics using
* the Niblack local thresholding
* algorithm.), or IMAQ_BACKGROUND_CORRECTION (which does background correction
* first to eliminate non-uniform
* lighting effects, then performs thresholding using the Otsu thresholding
* algorithm)
* @param deviationWeight Specifies the k constant used in the Niblack local
* thresholding algorithm, which
* determines the weight applied to the variance calculation. Valid k constants
* range from 0 to 1. Setting
* this value to 0 will increase the performance of the function because the
* function will not calculate the
* variance for any of the pixels. The function ignores this value if method is
* not set to IMAQ_NIBLACK
* @param type Specifies the type of objects for which you want to look. Values
* can be IMAQ_BRIGHT_OBJECTS
* or IMAQ_DARK_OBJECTS.
* @param replaceValue Specifies the replacement value the function uses for the
* pixels of the kept objects
* in the destination image.
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcSmartThreshold(Image* dest, const Image* source,
                      unsigned int windowWidth, unsigned int windowHeight,
                      LocalThresholdMethod method, double deviationWeight,
                      ObjectType type) {
  float replaceValue = 1.0;
  return imaqLocalThreshold(dest, source, windowWidth, windowHeight, method,
                            deviationWeight, type, replaceValue);
}

int frcSmartThreshold(Image* dest, const Image* source,
                      unsigned int windowWidth, unsigned int windowHeight,
                      LocalThresholdMethod method, double deviationWeight,
                      ObjectType type, float replaceValue) {
  return imaqLocalThreshold(dest, source, windowWidth, windowHeight, method,
                            deviationWeight, type, replaceValue);
}

/**
* @brief Converts a grayscale image to a binary image for Particle Analysis
* based on a fixed threshold.
* The function sets pixels values outside of the given range to 0. The function
* sets pixel values
* within the range to a given value or leaves the values unchanged.
* Use the simplified call to leave pixel values unchanged.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_I16.
*
* @param dest The destination image.
* @param source The image to threshold
* @param rangeMin The lower boundary of the range of pixel values to keep
* @param rangeMax The upper boundary of the range of pixel values to keep.
*
* @return int - error code: 0 = error. To get extended error information, call
* GetLastError().
*/
int frcSimpleThreshold(Image* dest, const Image* source, float rangeMin,
                       float rangeMax) {
  int newValue = 255;
  return frcSimpleThreshold(dest, source, rangeMin, rangeMax, newValue);
}

/**
* @brief Converts a grayscale image to a binary image for Particle Analysis
* based on a fixed threshold.
* The function sets pixels values outside of the given range to 0. The function
* sets
* pixel values within the range to the given value.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_I16.
*
* @param dest The destination image.
* @param source The image to threshold
* @param rangeMin The lower boundary of the range of pixel values to keep
* @param rangeMax The upper boundary of the range of pixel values to keep.
* @param newValue The replacement value for pixels within the range. Use the
* simplified call to leave the pixel values unchanged
*
* @return int - error code: 0 = error. To get extended error information, call
* GetLastError().
*/
int frcSimpleThreshold(Image* dest, const Image* source, float rangeMin,
                       float rangeMax, float newValue) {
  int useNewValue = TRUE;
  return imaqThreshold(dest, source, rangeMin, rangeMax, useNewValue, newValue);
}

/**
* @brief Applies a threshold to the Red, Green, and Blue values of a RGB image
* or the Hue,
* Saturation, Luminance values for a HSL image.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL.
* This simpler version filters based on a hue range in the HSL mode.
*
* @param dest The destination image. This must be a IMAQ_IMAGE_U8 image.
* @param source The image to threshold
* @param mode The color space to perform the threshold in. valid values are:
* IMAQ_RGB, IMAQ_HSL.
* @param plane1Range The selection range for the first plane of the image. Set
* this parameter to nullptr to use a selection range from 0 to 255.
* @param plane2Range The selection range for the second plane of the image. Set
* this parameter to nullptr to use a selection range from 0 to 255.
* @param plane3Range The selection range for the third plane of the image. Set
* this parameter to nullptr to use a selection range from 0 to 255.
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
* */
int frcColorThreshold(Image* dest, const Image* source, ColorMode mode,
                      const Range* plane1Range, const Range* plane2Range,
                      const Range* plane3Range) {
  int replaceValue = 1;
  return imaqColorThreshold(dest, source, replaceValue, mode, plane1Range,
                            plane2Range, plane3Range);
}

/**
* @brief Applies a threshold to the Red, Green, and Blue values of a RGB image
* or the Hue,
* Saturation, Luminance values for a HSL image.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL.
* The simpler version filters based on a hue range in the HSL mode.
*
* @param dest The destination image. This must be a IMAQ_IMAGE_U8 image.
* @param source The image to threshold
* @param replaceValue Value to assign to selected pixels. Defaults to 1 if
* simplified call is used.
* @param mode The color space to perform the threshold in. valid values are:
* IMAQ_RGB, IMAQ_HSL.
* @param plane1Range The selection range for the first plane of the image. Set
* this parameter to nullptr to use a selection range from 0 to 255.
* @param plane2Range The selection range for the second plane of the image. Set
* this parameter to nullptr to use a selection range from 0 to 255.
* @param plane3Range The selection range for the third plane of the image. Set
* this parameter to nullptr to use a selection range from 0 to 255.
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcColorThreshold(Image* dest, const Image* source, int replaceValue,
                      ColorMode mode, const Range* plane1Range,
                      const Range* plane2Range, const Range* plane3Range) {
  return imaqColorThreshold(dest, source, replaceValue, mode, plane1Range,
                            plane2Range, plane3Range);
}

/**
* @brief A simpler version of ColorThreshold that thresholds hue range in the
* HSL mode. Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL.
* @param dest The destination image.
* @param source The image to threshold
* @param hueRange The selection range for the hue (color).
* @param minSaturation The minimum saturation value (1-255).  If not used,
* DEFAULT_SATURATION_THRESHOLD is the default.
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcHueThreshold(Image* dest, const Image* source, const Range* hueRange) {
  return frcHueThreshold(dest, source, hueRange, DEFAULT_SATURATION_THRESHOLD);
}

int frcHueThreshold(Image* dest, const Image* source, const Range* hueRange,
                    int minSaturation) {
  // assume HSL mode
  ColorMode mode = IMAQ_HSL;
  // Set saturation 100 - 255
  Range satRange;
  satRange.minValue = minSaturation;
  satRange.maxValue = 255;
  // Set luminance 100 - 255
  Range lumRange;
  lumRange.minValue = 100;
  lumRange.maxValue = 255;
  // Replace pixels with 1 if pass threshold filter
  int replaceValue = 1;
  return imaqColorThreshold(dest, source, replaceValue, mode, hueRange,
                            &satRange, &lumRange);
}

/**
* @brief Extracts the Red, Green, Blue, or Hue, Saturation or Luminance
* information from a color image.
* Supports IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL, IMAQ_IMAGE_RGB_U64.
*
* @param image The source image that the function extracts the planes from.
* @param mode The color space that the function extracts the planes from. Valid
* values are IMAQ_RGB, IMAQ_HSL, IMAQ_HSV, IMAQ_HSI.
* @param plane1 On return, the first extracted plane. Set this parameter to nullptr
* if you do not need this information. RGB-Red, HSL/HSV/HSI-Hue.
* @param plane2 On return, the second extracted plane. Set this parameter to
* nullptr if you do not need this information. RGB-Green, HSL/HSV/HSI-Saturation.
* @param plane3 On return, the third extracted plane. Set this parameter to nullptr
* if you do not need this information. RGB-Blue, HSL-Luminance, HSV-Value,
* HSI-Intensity.
*
* @return error code: 0 = error. To get extended error information, call
* GetLastError().
*/
int frcExtractColorPlanes(const Image* image, ColorMode mode, Image* plane1,
                          Image* plane2, Image* plane3) {
  return imaqExtractColorPlanes(image, mode, plane1, plane2, plane3);
}

/**
* @brief Extracts the Hue information from a color image. Supports
* IMAQ_IMAGE_RGB, IMAQ_IMAGE_HSL, IMAQ_IMAGE_RGB_U64
*
* @param image The source image that the function extracts the plane from.
* @param huePlane On return, the extracted hue plane.
* @param minSaturation the minimum saturation level required 0-255 (try 50)
*
* @return On success: 1. On failure: 0. To get extended error information, call
* GetLastError().
*/
int frcExtractHuePlane(const Image* image, Image* huePlane) {
  return frcExtractHuePlane(image, huePlane, DEFAULT_SATURATION_THRESHOLD);
}

int frcExtractHuePlane(const Image* image, Image* huePlane, int minSaturation) {
  return frcExtractColorPlanes(image, IMAQ_HSL, huePlane, nullptr, nullptr);
}
