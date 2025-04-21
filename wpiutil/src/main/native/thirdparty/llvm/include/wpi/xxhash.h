/*
   xxHash - Extremely Fast Hash algorithm
   Header File
   Copyright (C) 2012-2016, Yann Collet.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - xxHash source repository : https://github.com/Cyan4973/xxHash
*/

/* based on revision d2df04efcbef7d7f6886d345861e5dfda4edacc1 Removed
 * everything but a simple interface for computing XXh64. */

#ifndef WPIUTIL_WPI_XXHASH_H
#define WPIUTIL_WPI_XXHASH_H

#include <stdint.h>

#include <span>
#include <string_view>

namespace wpi {

uint64_t xxHash64(std::string_view Data);
uint64_t xxHash64(std::span<const uint8_t> Data);

uint64_t xxh3_64bits(std::span<const uint8_t> data);
inline uint64_t xxh3_64bits(std::string_view data) {
  return xxh3_64bits(std::span(reinterpret_cast<const uint8_t*>(data.data()), data.size()));
}

/*-**********************************************************************
 *  XXH3 128-bit variant
 ************************************************************************/

/*!
 * @brief The return value from 128-bit hashes.
 *
 * Stored in little endian order, although the fields themselves are in native
 * endianness.
 */
struct XXH128_hash_t {
  uint64_t low64;  /*!< `value & 0xFFFFFFFFFFFFFFFF` */
  uint64_t high64; /*!< `value >> 64` */

  /// Convenience equality check operator.
  bool operator==(const XXH128_hash_t rhs) const {
    return low64 == rhs.low64 && high64 == rhs.high64;
  }
};

/// XXH3's 128-bit variant.
XXH128_hash_t xxh3_128bits(std::span<const uint8_t> data);

} // namespace wpi

#endif
