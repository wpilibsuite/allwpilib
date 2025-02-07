// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <utility>

#include "hal/AddressableLEDTypes.h"
#include "simd/simd.h"

// Timing info
// https://developer.arm.com/documentation/ddi0409/i/instruction-timing/instruction-specific-scheduling/advanced-simd-load-store-instructions?lang=en

namespace hal::detail {

using namespace Simd::Neon;

template <typename T>
using ConvertFunc = void (*)(T);

/*
 * Conversion funtions perform in-place conversion by swapping elements.
 * The names of the functions indicate the wire output (default GRB),
 * but the FPGA takes sequences of BGR_.
 */

template <typename T>
void ToRGB(T val) {
  std::swap(val[1], val[2]);  // swap G and R
}

template <typename T>
void ToRBG(T val) {
  std::swap(val[1], val[2]);  // swap G and R
  std::swap(val[0], val[2]);  // swap B and G
}

template <typename T>
void ToBGR(T val) {
  std::swap(val[0], val[1]);  // swap B and G
  std::swap(val[0], val[2]);  // swap G and R
}

template <typename T>
void ToBRG(T val) {
  std::swap(val[0], val[1]);  // swap B and G
}

template <typename T>
void ToGBR(T val) {
  std::swap(val[0], val[2]);  // swap B and R
}

/**
 * Copies 16 pixels from src to dst using NEON instructions, converting using
 * the provided conversion function. Optimizes based on alignment of input and
 * output arrays specified by srcAlign and dstAlign
 * @tparam srcAlign whether src is aligned to the size of a NEON register (16
 * bytes)
 * @tparam dstAlign whether dst is aligned to the size of a NEON register (16
 * bytes)
 * @tparam the conversion function
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 64 bytes (16 pixels)
 * @pre if srcAlign is true, src must be 16 byte aligned
 * @pre if dstAlign is true, src muts be 16 byte aligned
 */
template <bool srcAlign, bool dstAlign, ConvertFunc<uint8x16_t*> Convert>
void ConvertNEON_16(const uint8_t* src, uint8_t* dst) {
  uint8x16x4_t pixels = Load4<srcAlign>(src);
  Convert(pixels.val);
  Store4<dstAlign>(dst, pixels);
}

/**
 * Copies 8 pixels from src to dst using NEON instructions, converting using
 * the provided conversion function. Optimizes based on alignment of input and
 * output arrays specified by srcAlign and dstAlign
 * @tparam srcAlign whether src is aligned to the size of a NEON register (16
 * bytes)
 * @tparam dstAlign whether dst is aligned to the size of a NEON register (16
 * bytes)
 * @tparam the conversion function
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 32 bytes (8 pixels)
 * @pre if srcAlign is true, src must be 16 byte aligned
 * @pre if dstAlign is true, src muts be 16 byte aligned
 */
template <bool srcAlign, bool dstAlign, ConvertFunc<uint8x8_t*> Convert>
void ConvertNEON_8(const uint8_t* src, uint8_t* dst) {
  uint8x8x4_t pixels = LoadHalf4<srcAlign>(src);
  Convert(pixels.val);
  Store4<dstAlign>(dst, pixels);
}

/**
 * Copies 16 pixels from src to dst, converting from GRB (wire order) to order.
 * Optimizes based on alignment of input and output arrays specified by srcAlign
 * and dstAlign
 * @tparam order the color order to convert to
 * @tparam srcAlign whether src is aligned to the size of a NEON register (16
 * bytes)
 * @tparam dstAlign whether dst is aligned to the size of a NEON register (16
 * bytes)
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 64 bytes (16 pixels)
 * @pre if srcAlign is true, src must be 16 byte aligned
 * @pre if dstAlign is true, src muts be 16 byte aligned
 */
template <HAL_AddressableLEDColorOrder order, bool srcAlign, bool dstAlign>
void Convert16Pixels(const uint8_t* src, uint8_t* dst) {
  switch (order) {
    case HAL_ALED_RGB:
      ConvertNEON_16<srcAlign, dstAlign, ToRGB>(src, dst);
      break;
    case HAL_ALED_RBG:
      ConvertNEON_16<srcAlign, dstAlign, ToRBG>(src, dst);
      break;
    case HAL_ALED_BGR:
      ConvertNEON_16<srcAlign, dstAlign, ToBGR>(src, dst);
      break;
    case HAL_ALED_BRG:
      ConvertNEON_16<srcAlign, dstAlign, ToBRG>(src, dst);
      break;
    case HAL_ALED_GBR:
      ConvertNEON_16<srcAlign, dstAlign, ToGBR>(src, dst);
      break;
  }
}

/**
 * Copies 8 pixels from src to dst, converting from GRB (wire order) to order.
 * Optimizes based on alignment of input and output arrays specified by srcAlign
 * and dstAlign
 * @tparam order the color order to convert to
 * @tparam srcAlign whether src is aligned to the size of a NEON register (16
 * bytes)
 * @tparam dstAlign whether dst is aligned to the size of a NEON register (16
 * bytes)
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 32 bytes (8 pixels)
 * @pre if srcAlign is true, src must be 16 byte aligned
 * @pre if dstAlign is true, src muts be 16 byte aligned
 */
template <HAL_AddressableLEDColorOrder order, bool srcAlign, bool dstAlign>
void Convert8Pixels(const uint8_t* src, uint8_t* dst) {
  switch (order) {
    case HAL_ALED_RGB:
      ConvertNEON_8<srcAlign, dstAlign, ToRGB>(src, dst);
      break;
    case HAL_ALED_RBG:
      ConvertNEON_8<srcAlign, dstAlign, ToRBG>(src, dst);
      break;
    case HAL_ALED_BGR:
      ConvertNEON_8<srcAlign, dstAlign, ToBGR>(src, dst);
      break;
    case HAL_ALED_BRG:
      ConvertNEON_8<srcAlign, dstAlign, ToBRG>(src, dst);
      break;
    case HAL_ALED_GBR:
      ConvertNEON_8<srcAlign, dstAlign, ToGBR>(src, dst);
      break;
  }
}

/**
 * Copies 1 pixel from src to dst, converting from RGB to the specified order.
 * @param[in] order the color order to convert to
 * @param[in] in the source array
 * @param[out] the destination array
 * @pre in and out must contain at least 1 pixel (4 bytes).
 */
void Convert1Pixel(HAL_AddressableLEDColorOrder order, const uint8_t* src,
                   uint8_t* dst) {
  uint8_t tmp[4];
  std::memcpy(tmp, src, 4);  // Load 4 bytes
  // convert based on order
  switch (order) {
    case HAL_ALED_RGB:
      ToRGB(tmp);
      break;
    case HAL_ALED_RBG:
      ToRBG(tmp);
      break;
    case HAL_ALED_BGR:
      ToBGR(tmp);
      break;
    case HAL_ALED_BRG:
      ToBRG(tmp);
      break;
    case HAL_ALED_GBR:
      ToGBR(tmp);
      break;
    case HAL_ALED_GRB:
      break;  // this shouldn't ever get hit but compiler
              // wants this to be exhaustive
  }
  std::memcpy(dst, tmp, 4);  // Store 4 bytes
}
/**
 * Copies len pixels from src to dst, converting from GRB (wire order) to order.
 * Optimizes based on alignment of input and output arrays specified by srcAlign
 * and dstAlign
 * @tparam order the color order to convert to
 * @tparam srcAlign whether src is aligned to the size of a NEON register (16
 * bytes)
 * @tparam dstAlign whether dst is aligned to the size of a NEON register (16
 * bytes)
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @param[in] len the size (in pixels, len = (size in bytes) / 4)
 * @pre src and dst must have at least len*4 capacity in bytes
 * @pre if srcAlign is true, src must be 16 byte aligned
 * @pre if dstAlign is true, src muts be 16 byte aligned
 */
template <HAL_AddressableLEDColorOrder order, bool srcAlign, bool dstAlign>
void ConvertPixels(const uint8_t* src, uint8_t* dst, size_t len) {
  if (len >= 16) {
    constexpr size_t A4 =
        A * 4;  // Stride of 1 16-pixel conversion operation. (4 NEON registers)
    size_t size = len * 4;
    size_t aligned = Simd::AlignLo(
        size, A4);  // number of bytes we can copy with whole 16-pixel strides
    for (size_t i = 0; i < aligned; i += A4) {
      Convert16Pixels<order, srcAlign, dstAlign>(src + i, dst + i);
    }
    if (aligned < size) {
      Convert16Pixels<order, false, false>(
          src + size - A4,
          dst + size - A4);  // copy last 16 pixels, possibly recopying.
    }
  } else if (len >= 8) {
    // If len between 8 and 16, we can do 1 or 2 8-pixel copies
    Convert8Pixels<order, srcAlign, dstAlign>(src, dst);
    if (len > 8) {
      size_t recopyOffset = (len * 4) - (HA * 4);
      Convert8Pixels<order, false, false>(
          src + recopyOffset,
          dst + recopyOffset);  // copy last 8 pixels, possibly recopying
    }
  } else {
    // Just copy pixel-by-pixel for <8
    for (size_t i = 0; i < len; i += 4) {
      Convert1Pixel(order, src + i, dst + i);
    }
  }
}

/**
 * Copies pixelCount pixels from src to dst, converting from RGB to the
 * specified order
 * @tparam order the color order to convert to
 * @param src the source array
 * @param dst the destination array
 * @param pixelCount the number of pixels to convert and copy
 */
template <HAL_AddressableLEDColorOrder order>
void ConvertPixels(const uint8_t* src, uint8_t* dst, size_t pixelCount) {
  if (Aligned(src) && Aligned(dst)) {
    ConvertPixels<order, true, true>(src, dst, pixelCount);
  } else if (Aligned(src)) {
    ConvertPixels<order, true, false>(src, dst, pixelCount);
  } else if (Aligned(dst)) {
    ConvertPixels<order, false, true>(src, dst, pixelCount);
  } else {
    ConvertPixels<order, false, false>(src, dst, pixelCount);
  }
}
}  // namespace hal::detail
