// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <utility>

#include "wpi/hal/AddressableLEDTypes.h"
#include "simd/simd.h"

namespace hal::detail {

constexpr size_t kPixelSize = 3;
static_assert(sizeof(HAL_AddressableLEDData) == kPixelSize);
static_assert(offsetof(HAL_AddressableLEDData, r) == 0);
static_assert(offsetof(HAL_AddressableLEDData, g) == 1);
static_assert(offsetof(HAL_AddressableLEDData, b) == 2);

using namespace Simd::Neon;

template <typename T>
using ConvertFunc = void (*)(T);

/*
 * Conversion funtions perform in-place conversion by swapping elements.
 * The names of the functions indicate the desired wire output order.
 */

template <typename T>
void ToRBG(T val) {
  std::swap(val[1], val[2]);  // swap G and B
}

template <typename T>
void ToBGR(T val) {
  std::swap(val[0], val[2]);  // swap B and R
}

template <typename T>
void ToBRG(T val) {
  std::swap(val[0], val[2]);  // swap R and B
  std::swap(val[1], val[2]);  // swap R and G
}

template <typename T>
void ToGBR(T val) {
  std::swap(val[0], val[2]);  // swap R and B
  std::swap(val[0], val[1]);  // swap B and G
}

template <typename T>
void ToGRB(T val) {
  std::swap(val[0], val[1]);  // swap R and G
}

/*
 * We don't use the alignment argument on the Load/Store functions (set it to
 * false) because aarch64 doesn't have alignment assertions.
 */

/**
 * Copies 16 pixels from src to dst using NEON instructions, converting using
 * the provided conversion function.
 * @tparam the conversion function
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 48 bytes (16 pixels)
 */
template <ConvertFunc<uint8x16_t*> Convert>
inline void ConvertNEON_16(const uint8_t* src, uint8_t* dst) {
  uint8x16x3_t pixels = Load3<false>(src);
  Convert(pixels.val);
  Store3<false>(dst, pixels);
}

/**
 * Copies 8 pixels from src to dst using NEON instructions, converting using
 * the provided conversion function.
 * @tparam the conversion function
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 24 bytes (8 pixels)
 */
template <ConvertFunc<uint8x8_t*> Convert>
inline void ConvertNEON_8(const uint8_t* src, uint8_t* dst) {
  uint8x8x3_t pixels = LoadHalf3<false>(src);
  Convert(pixels.val);
  Store3<false>(dst, pixels);
}

/**
 * Copies 16 pixels from src to dst, converting from RGB to the specified order.
 * @tparam order the color order to convert to
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 48 bytes (16 pixels)
 */
template <HAL_AddressableLEDColorOrder order>
void Convert16Pixels(const uint8_t* src, uint8_t* dst) {
  switch (order) {
    case HAL_ALED_RBG:
      ConvertNEON_16<ToRBG>(src, dst);
      break;
    case HAL_ALED_BGR:
      ConvertNEON_16<ToBGR>(src, dst);
      break;
    case HAL_ALED_BRG:
      ConvertNEON_16<ToBRG>(src, dst);
      break;
    case HAL_ALED_GBR:
      ConvertNEON_16<ToGBR>(src, dst);
      break;
    case HAL_ALED_GRB:
      ConvertNEON_16<ToGRB>(src, dst);
      break;
  }
}

/**
 * Copies 8 pixels from src to dst, converting from RGB to the specified order.
 * @tparam order the color order to convert to
 * @param[in] src The source array
 * @param[out] dst the destination array
 * @pre src and dst must contain at least 24 bytes (8 pixels)
 */
template <HAL_AddressableLEDColorOrder order>
void Convert8Pixels(const uint8_t* src, uint8_t* dst) {
  switch (order) {
    case HAL_ALED_RBG:
      ConvertNEON_8<ToRBG>(src, dst);
      break;
    case HAL_ALED_BGR:
      ConvertNEON_8<ToBGR>(src, dst);
      break;
    case HAL_ALED_BRG:
      ConvertNEON_8<ToBRG>(src, dst);
      break;
    case HAL_ALED_GBR:
      ConvertNEON_8<ToGBR>(src, dst);
      break;
    case HAL_ALED_GRB:
      ConvertNEON_8<ToGRB>(src, dst);
      break;
  }
}

/**
 * Copies 1 pixel from src to dst, converting from RGB to the specified order.
 * @param[in] order the color order to convert to
 * @param[in] in the source array
 * @param[out] the destination array
 * @pre in and out must contain at least 1 pixel (3 bytes).
 */
inline void Convert1Pixel(HAL_AddressableLEDColorOrder order,
                          const uint8_t* src, uint8_t* dst) {
  uint8_t tmp[kPixelSize];
  std::memcpy(tmp, src, kPixelSize);  // Load 3 bytes
  // convert based on order
  switch (order) {
    case HAL_ALED_RGB:
      break;  // this shouldn't ever get hit but compiler
              // wants this to be exhaustive
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
      ToGRB(tmp);
      break;
  }
  std::memcpy(dst, tmp, kPixelSize);  // Store 3 bytes
}

/**
 * Copies len pixels from src to dst, converting from RGB to the
 * specified order.
 * @tparam order the color order to convert to
 * @param src the source array
 * @param dst the destination array
 * @param pixelCount the number of pixels to convert and copy
 */
template <HAL_AddressableLEDColorOrder order>
void ConvertPixels(const uint8_t* src, uint8_t* dst, size_t len) {
  if (len >= 16) {
    // Stride of 1 16-pixel conversion operation. (3 NEON Q registers)
    constexpr size_t stride = A * kPixelSize;
    // size of whole copy in bytes
    const size_t size = len * kPixelSize;
    // number of bytes we can copy with whole 16-pixel strides
    const size_t aligned = Simd::AlignLo(size, stride);
    for (size_t i = 0; i < aligned; i += stride) {
      Convert16Pixels<order>(src + i, dst + i);
    }
    if (aligned < size) {
      const size_t recopyOffset = size - stride;
      Convert16Pixels<order>(
          src + recopyOffset,
          dst + recopyOffset);  // copy last 16 pixels, possibly recopying.
    }
  } else if (len >= 8) {
    // If len between 8 and 16, we can do 1 or 2 8-pixel copies
    Convert8Pixels<order>(src, dst);
    if (len > 8) {
      const size_t recopyOffset = (len - HA) * kPixelSize;
      Convert8Pixels<order>(
          src + recopyOffset,
          dst + recopyOffset);  // copy last 8 pixels, possibly recopying
    }
  } else {
    // Just copy pixel-by-pixel for <8
    for (size_t i = 0; i < len; i += kPixelSize) {
      Convert1Pixel(order, src + i, dst + i);
    }
  }
}
}  // namespace hal::detail
