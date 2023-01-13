//===-- ConvertUTFWrapper.cpp - Wrap ConvertUTF.h with clang data types -----===
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <span>
#include "wpi/ConvertUTF.h"
#include "wpi/SmallVector.h"
#include "wpi/ErrorHandling.h"
#include "wpi/SwapByteOrder.h"
#include <string>
#include <string_view>
#include <vector>

namespace wpi {

bool ConvertUTF8toWide(unsigned WideCharWidth, std::string_view Source,
                       char *&ResultPtr, const UTF8 *&ErrorPtr) {
  assert(WideCharWidth == 1 || WideCharWidth == 2 || WideCharWidth == 4);
  ConversionResult result = conversionOK;
  // Copy the character std::span over.
  if (WideCharWidth == 1) {
    const UTF8 *Pos = reinterpret_cast<const UTF8*>(Source.data());
    if (!isLegalUTF8String(&Pos, reinterpret_cast<const UTF8*>(Source.data() + Source.size()))) {
      result = sourceIllegal;
      ErrorPtr = Pos;
    } else {
      memcpy(ResultPtr, Source.data(), Source.size());
      ResultPtr += Source.size();
    }
  } else if (WideCharWidth == 2) {
    const UTF8 *sourceStart = (const UTF8*)Source.data();
    // FIXME: Make the type of the result buffer correct instead of
    // using reinterpret_cast.
    UTF16 *targetStart = reinterpret_cast<UTF16*>(ResultPtr);
    ConversionFlags flags = strictConversion;
    result = ConvertUTF8toUTF16(
        &sourceStart, sourceStart + Source.size(),
        &targetStart, targetStart + Source.size(), flags);
    if (result == conversionOK)
      ResultPtr = reinterpret_cast<char*>(targetStart);
    else
      ErrorPtr = sourceStart;
  } else if (WideCharWidth == 4) {
    const UTF8 *sourceStart = (const UTF8*)Source.data();
    // FIXME: Make the type of the result buffer correct instead of
    // using reinterpret_cast.
    UTF32 *targetStart = reinterpret_cast<UTF32*>(ResultPtr);
    ConversionFlags flags = strictConversion;
    result = ConvertUTF8toUTF32(
        &sourceStart, sourceStart + Source.size(),
        &targetStart, targetStart + Source.size(), flags);
    if (result == conversionOK)
      ResultPtr = reinterpret_cast<char*>(targetStart);
    else
      ErrorPtr = sourceStart;
  }
  assert((result != targetExhausted)
         && "ConvertUTF8toUTFXX exhausted target buffer");
  return result == conversionOK;
}

bool ConvertCodePointToUTF8(unsigned Source, char *&ResultPtr) {
  const UTF32 *SourceStart = &Source;
  const UTF32 *SourceEnd = SourceStart + 1;
  UTF8 *TargetStart = reinterpret_cast<UTF8 *>(ResultPtr);
  UTF8 *TargetEnd = TargetStart + 4;
  ConversionResult CR = ConvertUTF32toUTF8(&SourceStart, SourceEnd,
                                           &TargetStart, TargetEnd,
                                           strictConversion);
  if (CR != conversionOK)
    return false;

  ResultPtr = reinterpret_cast<char*>(TargetStart);
  return true;
}

bool hasUTF16ByteOrderMark(std::span<const char> S) {
  return (S.size() >= 2 &&
          ((S[0] == '\xff' && S[1] == '\xfe') ||
           (S[0] == '\xfe' && S[1] == '\xff')));
}

bool convertUTF16ToUTF8String(std::span<const char> SrcBytes, SmallVectorImpl<char> &Out) {
  assert(Out.empty());

  // Error out on an uneven byte count.
  if (SrcBytes.size() % 2)
    return false;

  // Avoid OOB by returning early on empty input.
  if (SrcBytes.empty())
    return true;

  const UTF16 *Src = reinterpret_cast<const UTF16 *>(&*SrcBytes.begin());
  const UTF16 *SrcEnd = reinterpret_cast<const UTF16 *>(&*SrcBytes.begin() + SrcBytes.size());

  assert((uintptr_t)Src % sizeof(UTF16) == 0);

  // Byteswap if necessary.
  std::vector<UTF16> ByteSwapped;
  if (Src[0] == UNI_UTF16_BYTE_ORDER_MARK_SWAPPED) {
    ByteSwapped.insert(ByteSwapped.end(), Src, SrcEnd);
    for (UTF16 &I : ByteSwapped)
      I = wpi::ByteSwap_16(I);
    Src = &ByteSwapped[0];
    SrcEnd = &ByteSwapped[ByteSwapped.size() - 1] + 1;
  }

  // Skip the BOM for conversion.
  if (Src[0] == UNI_UTF16_BYTE_ORDER_MARK_NATIVE)
    Src++;

  // Just allocate enough space up front.  We'll shrink it later.  Allocate
  // enough that we can fit a null terminator without reallocating.
  Out.resize(SrcBytes.size() * UNI_MAX_UTF8_BYTES_PER_CODE_POINT + 1);
  UTF8 *Dst = reinterpret_cast<UTF8 *>(&Out[0]);
  UTF8 *DstEnd = Dst + Out.size();

  ConversionResult CR =
      ConvertUTF16toUTF8(&Src, SrcEnd, &Dst, DstEnd, strictConversion);
  assert(CR != targetExhausted);

  if (CR != conversionOK) {
    Out.clear();
    return false;
  }

  Out.resize(reinterpret_cast<char *>(Dst) - &Out[0]);
  Out.push_back(0);
  Out.pop_back();
  return true;
}

bool convertUTF16ToUTF8String(std::span<const UTF16> Src, SmallVectorImpl<char> &Out)
{
  return convertUTF16ToUTF8String(
      std::span<const char>(reinterpret_cast<const char *>(Src.data()),
      Src.size() * sizeof(UTF16)), Out);
}

bool convertUTF8ToUTF16String(std::string_view SrcUTF8,
                              SmallVectorImpl<UTF16> &DstUTF16) {
  assert(DstUTF16.empty());

  // Avoid OOB by returning early on empty input.
  if (SrcUTF8.empty()) {
    DstUTF16.push_back(0);
    DstUTF16.pop_back();
    return true;
  }

  const UTF8 *Src = reinterpret_cast<const UTF8 *>(SrcUTF8.data());
  const UTF8 *SrcEnd = reinterpret_cast<const UTF8 *>(SrcUTF8.data() + SrcUTF8.size());

  // Allocate the same number of UTF-16 code units as UTF-8 code units. Encoding
  // as UTF-16 should always require the same amount or less code units than the
  // UTF-8 encoding.  Allocate one extra byte for the null terminator though,
  // so that someone calling DstUTF16.data() gets a null terminated string.
  // We resize down later so we don't have to worry that this over allocates.
  DstUTF16.resize(SrcUTF8.size()+1);
  UTF16 *Dst = &DstUTF16[0];
  UTF16 *DstEnd = Dst + DstUTF16.size();

  ConversionResult CR =
      ConvertUTF8toUTF16(&Src, SrcEnd, &Dst, DstEnd, strictConversion);
  assert(CR != targetExhausted);

  if (CR != conversionOK) {
    DstUTF16.clear();
    return false;
  }

  DstUTF16.resize(Dst - &DstUTF16[0]);
  DstUTF16.push_back(0);
  DstUTF16.pop_back();
  return true;
}

static_assert(sizeof(wchar_t) == 1 || sizeof(wchar_t) == 2 ||
                  sizeof(wchar_t) == 4,
              "Expected wchar_t to be 1, 2, or 4 bytes");

template <typename TResult>
static inline bool ConvertUTF8toWideInternal(std::string_view Source,
                                             TResult &Result) {
  // Even in the case of UTF-16, the number of bytes in a UTF-8 string is
  // at least as large as the number of elements in the resulting wide
  // string, because surrogate pairs take at least 4 bytes in UTF-8.
  Result.resize(Source.size() + 1);
  char *ResultPtr = reinterpret_cast<char *>(&Result[0]);
  const UTF8 *ErrorPtr;
  if (!ConvertUTF8toWide(sizeof(wchar_t), Source, ResultPtr, ErrorPtr)) {
    Result.clear();
    return false;
  }
  Result.resize(reinterpret_cast<wchar_t *>(ResultPtr) - &Result[0]);
  return true;
}

bool ConvertUTF8toWide(std::string_view Source, std::wstring &Result) {
  return ConvertUTF8toWideInternal(Source, Result);
}

bool ConvertUTF8toWide(const char *Source, std::wstring &Result) {
  if (!Source) {
    Result.clear();
    return true;
  }
  return ConvertUTF8toWide(std::string_view(Source), Result);
}

bool convertWideToUTF8(const std::wstring &Source, SmallVectorImpl<char> &Result) {
  if (sizeof(wchar_t) == 1) {
    const UTF8 *Start = reinterpret_cast<const UTF8 *>(Source.data());
    const UTF8 *End =
        reinterpret_cast<const UTF8 *>(Source.data() + Source.size());
    if (!isLegalUTF8String(&Start, End))
      return false;
    Result.resize(Source.size());
    memcpy(&Result[0], Source.data(), Source.size());
    return true;
  } else if (sizeof(wchar_t) == 2) {
    return convertUTF16ToUTF8String(
        std::span<const UTF16>(reinterpret_cast<const UTF16 *>(Source.data()),
                              Source.size()),
        Result);
  } else if (sizeof(wchar_t) == 4) {
    const UTF32 *Start = reinterpret_cast<const UTF32 *>(Source.data());
    const UTF32 *End =
        reinterpret_cast<const UTF32 *>(Source.data() + Source.size());
    Result.resize(UNI_MAX_UTF8_BYTES_PER_CODE_POINT * Source.size());
    UTF8 *ResultPtr = reinterpret_cast<UTF8 *>(&Result[0]);
    UTF8 *ResultEnd = reinterpret_cast<UTF8 *>(&Result[0] + Result.size());
    if (ConvertUTF32toUTF8(&Start, End, &ResultPtr, ResultEnd,
                           strictConversion) == conversionOK) {
      Result.resize(reinterpret_cast<char *>(ResultPtr) - &Result[0]);
      return true;
    } else {
      Result.clear();
      return false;
    }
  } else {
    wpi_unreachable(
        "Control should never reach this point; see static_assert further up");
  }
}

} // end namespace wpi

