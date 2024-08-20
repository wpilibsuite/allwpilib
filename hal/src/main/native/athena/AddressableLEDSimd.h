// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <utility>

#include "hal/AddressableLEDTypes.h"
#include "simd/simd.h"
//https://developer.arm.com/documentation/ddi0409/g/Instruction-Timing/Instruction-specific-scheduling/Advanced-SIMD-load-store-instructions?lang=en
//https://developer.arm.com/documentation/ddi0406/c/Application-Level-Architecture/Instruction-Details/Alphabetical-list-of-instructions/VLD4--single-4-element-structure-to-one-lane-
namespace {
using namespace Simd::Neon;
template <bool align>
void RGBToRBG_16(const uint8_t* rgb, uint8_t* bgr) {
  uint8x16x4_t _rgb = Load4<align>(rgb);
  std::swap(_rgb.val[1], _rgb.val[2]);  // swap G and B
  Store4<align>(bgr, _rgb);
}

template <bool align>
void RGBToBGR_16(const uint8_t* rgb, uint8_t* bgr) {
  uint8x16x4_t _rgb = Load4<align>(rgb);
  std::swap(_rgb.val[0], _rgb.val[2]);  // swap R and B
  Store4<align>(bgr, _rgb);
}

template <bool align>
void RGBToBRG_16(const uint8_t* rgb, uint8_t* bgr) {
  uint8x16x4_t _rgb = Load4<align>(rgb);
  std::swap(_rgb.val[0], _rgb.val[2]);  // swap R and B
  std::swap(_rgb.val[1], _rgb.val[2]);  // swap G and R
  Store4<align>(bgr, _rgb);
}

template <bool align>
void RGBToGRB_16(const uint8_t* rgb, uint8_t* bgr) {
  uint8x16x4_t _rgb = Load4<align>(rgb);
  std::swap(_rgb.val[0], _rgb.val[1]);  // swap G and R
  Store4<align>(bgr, _rgb);
}

template <bool align>
void RGBToGBR_16(const uint8_t* rgb, uint8_t* bgr) {
  uint8x16x4_t _rgb = Load4<align>(rgb);
  std::swap(_rgb.val[0], _rgb.val[2]);  // swap R and B
  std::swap(_rgb.val[0], _rgb.val[1]);  // swap B and G
  Store4<align>(bgr, _rgb);
}

template <HAL_AddressableLEDColorOrder order, bool align>
void RGBConvert_16(const uint8_t* src, uint8_t* dst) {
  switch (order) {
    case HAL_ALED_RBG:
      RGBToRBG_16<align>(src, dst);
      break;
    case HAL_ALED_BGR:
      RGBToBGR_16<align>(src, dst);
      break;
    case HAL_ALED_BRG:
      RGBToBRG_16<align>(src, dst);
      break;
    case HAL_ALED_GRB:
      RGBToGRB_16<align>(src, dst);
      break;
    case HAL_ALED_GBR:
      RGBToGBR_16<align>(src, dst);
      break;
  }
}

template <HAL_AddressableLEDColorOrder order, bool align>
void RGBConvert(const uint8_t* src, uint8_t* dst, size_t len) {
  if(len > 16) {
    const size_t A4 = A * 4;
    size_t size = len * 4;
    size_t aligned = Simd::AlignLo(size, A4);
    for (size_t i = 0; i < aligned; i += A4) {
      RGBConvert_16<order, align>(src + i, dst + i);
    }
    if (aligned < size) {
      RGBConvert_16<order, false>(
          src + size - A4,
          dst + size - A4);  // copy last 16 pixels, possibly recopying.
    }
  } else {
    // https://developer.arm.com/documentation/ddi0406/c/Application-Level-Architecture/Instruction-Details/Alphabetical-list-of-instructions/VLD4--single-4-element-structure-to-all-lanes-
    // https://developer.arm.com/documentation/102474/0100/Fundamentals-of-Armv8-Neon-technology/Registers--vectors--lanes-and-elements
    // vld4_lane_u8
  }

}

template <HAL_AddressableLEDColorOrder order>
void RGBConvert(const uint8_t* src, uint8_t* dst, size_t pixelCount) {
  if (Aligned(src) && Aligned(dst)) {
    RGBConvert<order, true>(src, dst, pixelCount);
  } else {
    RGBConvert<order, false>(src, dst, pixelCount);
  }
}
}  // namespace
