// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <utility>

#include "hal/AddressableLEDTypes.h"
#include "simd/simd.h"
//https://developer.arm.com/documentation/ddi0409/i/instruction-timing/instruction-specific-scheduling/advanced-simd-load-store-instructions?lang=en
//https://developer.arm.com/documentation/ddi0406/c/Application-Level-Architecture/Instruction-Details/Alphabetical-list-of-instructions/VLD4--single-4-element-structure-to-one-lane-

namespace {
using namespace Simd::Neon;

template<typename T>
using ConvertFunc = void(*)(T);

template <typename T>
void RGBToRBG(T val) {
  std::swap(val[0], val[1]); // swap G and B
}

template <typename T>
void RGBToBGR(T val) {
  std::swap(val[0], val[2]); // swap R and B
}

template <typename T>
void RGBToBRG(T val) {
  std::swap(val[0], val[2]); // swap R and B
  std::swap(val[0], val[1]); // swap G and R
}

template <typename T>
void RGBToGRB(T val) {
  std::swap(val[1], val[2]); // swap R and G
}

template <typename T>
void RGBToGBR(T val) {
  std::swap(val[0], val[2]); // swap R and B
  std::swap(val[1], val[2]); // swap B and G
}

template <bool inAlign, bool outAlign, ConvertFunc<uint8x16_t*> Convert>
void ConvertNEON_16(const uint8_t* in, uint8_t* out) {
  uint8x16x4_t rgb = Load4<inAlign>(in);
  Convert(rgb.val);
  Store4<outAlign>(out, rgb);
}

template <bool inAlign, bool outAlign, ConvertFunc<uint8x8_t*> Convert>
void ConvertNEON_8(const uint8_t* in, uint8_t* out) {
  uint8x8x4_t rgb = LoadHalf4<inAlign>(in);
  Convert(rgb.val);
  StoreHalf4<outAlign>(out, rgb);
}

template <HAL_AddressableLEDColorOrder order, bool inAlign, bool outAlign>
void RGBConvert_16(const uint8_t* src, uint8_t* dst) {
  switch (order) {
    case HAL_ALED_RBG:
      ConvertNEON_16<inAlign, outAlign, RGBToRBG>(src, dst);
      break;
    case HAL_ALED_BGR:
      ConvertNEON_16<inAlign, outAlign, RGBToBGR>(src, dst);
      break;
    case HAL_ALED_BRG:
      ConvertNEON_16<inAlign, outAlign, RGBToBRG>(src, dst);
      break;
    case HAL_ALED_GRB:
      ConvertNEON_16<inAlign, outAlign, RGBToGRB>(src, dst);
      break;
    case HAL_ALED_GBR:
      ConvertNEON_16<inAlign, outAlign, RGBToGBR>(src, dst);
      break;
  }
}

template <HAL_AddressableLEDColorOrder order, bool inAlign, bool outAlign>
void RGBConvert(const uint8_t* src, uint8_t* dst, size_t len) {
  if(len >= 16) {
    const size_t A4 = A * 4;
    size_t size = len * 4;
    size_t aligned = Simd::AlignLo(size, A4);
    for (size_t i = 0; i < aligned; i += A4) {
      RGBConvert_16<order, inAlign, outAlign>(src + i, dst + i);
    }
    if (aligned < size) {
      RGBConvert_16<order, false, false>(
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
    RGBConvert<order, true, true>(src, dst, pixelCount);
  } else if(Aligned(src)) {
    RGBConvert<order, true, false>(src, dst, pixelCount);
  } else if(Aligned(dst)) {
    RGBConvert<order, false, true>(src, dst, pixelCount);
  } else {
    RGBConvert<order, false, false>(src, dst, pixelCount);
  }
}
}  // namespace
