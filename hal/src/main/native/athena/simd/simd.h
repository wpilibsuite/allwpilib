// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// This file contains modified snippets from the Simd Library by Ihar Yermalayeu
// (http://ermig1979.github.io/Simd). The original source file names are listed
// above each section.
/*
 * Simd Library (http://ermig1979.github.io/Simd).
 *
 * Copyright (c) 2011-2024 Yermalayeu Ihar.
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

#pragma once

#include <arm_neon.h>

#include <cstddef>
#include <cstring>

// SimdLib.h
#define SIMD_INLINE inline __attribute__((always_inline))

// SimdMemory.h
namespace Simd {
SIMD_INLINE size_t AlignLo(size_t size, size_t align) {
  return size & ~(align - 1);
}

SIMD_INLINE void* AlignLo(const void* ptr, size_t align) {
  return reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr) & ~(align - 1));
}

SIMD_INLINE bool Aligned(size_t size, size_t align) {
  return size == AlignLo(size, align);
}

SIMD_INLINE bool Aligned(const void* ptr, size_t align) {
  return ptr == AlignLo(ptr, align);
}
}  // namespace Simd
namespace Simd::Neon {
SIMD_INLINE bool Aligned(size_t size, size_t align = sizeof(uint8x16_t)) {
  return Simd::Aligned(size, align);
}

SIMD_INLINE bool Aligned(const void* ptr, size_t align = sizeof(uint8x16_t)) {
  return Simd::Aligned(ptr, align);
}
}  // namespace Simd::Neon

// SimdConst.h
namespace Simd::Neon {
const size_t A = sizeof(uint8x16_t);
const size_t DA = 2 * A;
const size_t QA = 4 * A;
const size_t OA = 8 * A;
const size_t HA = A / 2;
}  // namespace Simd::Neon

// SimdLoad.h
namespace Simd::Neon {
template <bool align>
SIMD_INLINE uint8x8x4_t LoadHalf4(const uint8_t* p);

template <>
SIMD_INLINE uint8x8x4_t LoadHalf4<false>(const uint8_t* p) {
#if defined(__GNUC__) && SIMD_NEON_PREFECH_SIZE
  __builtin_prefetch(p + SIMD_NEON_PREFECH_SIZE);
#endif
  return vld4_u8(p);
}

template <>
SIMD_INLINE uint8x8x4_t LoadHalf4<true>(const uint8_t* p) {
#if defined(__GNUC__)
#if SIMD_NEON_PREFECH_SIZE
  __builtin_prefetch(p + SIMD_NEON_PREFECH_SIZE);
#endif
  uint8_t* _p = static_cast<uint8_t*>(__builtin_assume_aligned(p, 8));
  return vld4_u8(_p);
#elif defined(_MSC_VER)
  return vld4_u8_ex(p, 64);
#else
  return vld4_u8(p);
#endif
}

template <bool align>
SIMD_INLINE uint8x16x4_t Load4(const uint8_t* p);

template <>
SIMD_INLINE uint8x16x4_t Load4<false>(const uint8_t* p) {
#if defined(__GNUC__) && SIMD_NEON_PREFECH_SIZE
  __builtin_prefetch(p + SIMD_NEON_PREFECH_SIZE);
#endif
  return vld4q_u8(p);
}

template <>
SIMD_INLINE uint8x16x4_t Load4<true>(const uint8_t* p) {
#if defined(__GNUC__)
#if SIMD_NEON_PREFECH_SIZE
  __builtin_prefetch(p + SIMD_NEON_PREFECH_SIZE);
#endif
  uint8_t* _p = static_cast<uint8_t*>(__builtin_assume_aligned(p, 16));
  return vld4q_u8(_p);
#elif defined(_MSC_VER)
  return vld4q_u8_ex(p, 128);
#else
  return vld4q_u8(p);
#endif
}

// SimdStore.h
template <bool align>
SIMD_INLINE void Store4(uint8_t* p, uint8x16x4_t a);

template <>
SIMD_INLINE void Store4<false>(uint8_t* p, uint8x16x4_t a) {
  vst4q_u8(p, a);
}

template <>
SIMD_INLINE void Store4<true>(uint8_t* p, uint8x16x4_t a) {
#if defined(__GNUC__)
  uint8_t* _p = static_cast<uint8_t*>(__builtin_assume_aligned(p, 16));
  vst4q_u8(_p, a);
#elif defined(_MSC_VER)
  vst4q_u8_ex(p, a, 128);
#else
  vst4q_u8(p, a);
#endif
}

template <bool align>
SIMD_INLINE void Store4(uint8_t* p, uint8x8x4_t a);

template <>
SIMD_INLINE void Store4<false>(uint8_t* p, uint8x8x4_t a) {
  vst4_u8(p, a);
}

template <>
SIMD_INLINE void Store4<true>(uint8_t* p, uint8x8x4_t a) {
#if defined(__GNUC__)
  uint8_t* _p = static_cast<uint8_t*>(__builtin_assume_aligned(p, 8));
  vst4_u8(_p, a);
#elif defined(_MSC_VER)
  vst4_u8_ex(p, a, 64);
#else
  vst4_u8(p, a);
#endif
}

}  // namespace Simd::Neon
