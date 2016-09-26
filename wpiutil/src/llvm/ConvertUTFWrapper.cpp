//===-- ConvertUTFWrapper.cpp - Wrap ConvertUTF.h with clang data types -----===
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/ConvertUTF.h"
#include <string>
#include <vector>

namespace llvm {

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

bool hasUTF16ByteOrderMark(ArrayRef<char> S) {
  return (S.size() >= 2 &&
          ((S[0] == '\xff' && S[1] == '\xfe') ||
           (S[0] == '\xfe' && S[1] == '\xff')));
}

bool convertUTF16ToUTF8String(ArrayRef<UTF16> SrcUTF16,
                              SmallVectorImpl<char> &DstUTF8) {
  assert(DstUTF8.empty());

  // Avoid OOB by returning early on empty input.
  if (SrcUTF16.empty())
    return true;

  const UTF16 *Src = SrcUTF16.begin();
  const UTF16 *SrcEnd = SrcUTF16.end();

  // Byteswap if necessary.
  std::vector<UTF16> ByteSwapped;
  if (Src[0] == UNI_UTF16_BYTE_ORDER_MARK_SWAPPED) {
    ByteSwapped.insert(ByteSwapped.end(), Src, SrcEnd);
    for (unsigned I = 0, E = ByteSwapped.size(); I != E; ++I)
      ByteSwapped[I] = (ByteSwapped[I] << 8) | (ByteSwapped[I] >> 8);
    Src = &ByteSwapped[0];
    SrcEnd = &ByteSwapped[ByteSwapped.size() - 1] + 1;
  }

  // Skip the BOM for conversion.
  if (Src[0] == UNI_UTF16_BYTE_ORDER_MARK_NATIVE)
    Src++;

  // Just allocate enough space up front.  We'll shrink it later.  Allocate
  // enough that we can fit a null terminator without reallocating.
  DstUTF8.resize(SrcUTF16.size() * UNI_MAX_UTF8_BYTES_PER_CODE_POINT + 1);
  UTF8 *Dst = reinterpret_cast<UTF8*>(&DstUTF8[0]);
  UTF8 *DstEnd = Dst + DstUTF8.size();

  ConversionResult CR =
      ConvertUTF16toUTF8(&Src, SrcEnd, &Dst, DstEnd, strictConversion);
  assert(CR != targetExhausted);

  if (CR != conversionOK) {
    DstUTF8.clear();
    return false;
  }

  DstUTF8.resize(reinterpret_cast<char*>(Dst) - &DstUTF8[0]);
  DstUTF8.push_back(0);
  DstUTF8.pop_back();
  return true;
}

bool convertUTF8ToUTF16String(StringRef SrcUTF8,
                              SmallVectorImpl<UTF16> &DstUTF16) {
  assert(DstUTF16.empty());

  // Avoid OOB by returning early on empty input.
  if (SrcUTF8.empty()) {
    DstUTF16.push_back(0);
    DstUTF16.pop_back();
    return true;
  }

  const UTF8 *Src = reinterpret_cast<const UTF8 *>(SrcUTF8.begin());
  const UTF8 *SrcEnd = reinterpret_cast<const UTF8 *>(SrcUTF8.end());

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

} // end namespace llvm

