// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "simd.h"

using namespace Simd::Neon;

template <bool align>
void RGBToBGR_16(uint8_t* rgb, uint8_t* bgr) {
  uint8x16x4_t _rgb = Load4<align>(rgb);
  uint8x16_t tmp = _rgb.val[0];  // tmp = r
  _rgb.val[0] = _rgb.val[2];     // r = r;
  _rgb.val[2] = tmp;
  Store4<align>(bgr, _rgb);
}

template <bool align>
void RGBToBGR_8(uint8_t* rgb, uint8_t* bgr) {
  uint8x8x4_t _rgb = LoadHalf4<align>(rgb);
  uint8x8_t tmp = _rgb.val[0];  // tmp = r
  _rgb.val[0] = _rgb.val[2];    // r = r;
  _rgb.val[2] = tmp;
  Store4<align>(bgr, _rgb);
}

template <bool align>
void _RGBToBGR(uint8_t* rgb, uint8_t* bgr, size_t pixelCount) {
  // todo: handle pixelcount < 16
  const size_t A4 = A*4;
  size_t size = pixelCount * 4;
  size_t aligned = Simd::AlignLo(pixelCount, A)*4;
  for(size_t i = 0; i < aligned; i += A4) {
    RGBToBGR_16<align>(rgb + i, bgr + i);
    if(aligned < size) {
      RGBToBGR_16<false>(rgb + size - A4, bgr + size - A4); // copy last 16 pixels, possibly recopying.
    }
  }
}

void RGBToBGR(uint8_t* rgb, uint8_t* bgr, size_t pixelCount) {
  if(Aligned(rgb), Aligned(bgr)) {
    _RGBToBGR<true>(rgb, bgr, pixelCount);
  } else {
    _RGBToBGR<false>(rgb, bgr, pixelCount);
  }
}