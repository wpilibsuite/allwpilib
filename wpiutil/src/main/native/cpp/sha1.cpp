/*
    sha1.cpp - source code of

    ============
    SHA-1 in C++
    ============

    100% Public Domain.

    Original C Code
        -- Steve Reid <steve@edmweb.com>
    Small changes to fit into bglibs
        -- Bruce Guenter <bruce@untroubled.org>
    Translation to simpler C++ Code
        -- Volker Grabsch <vog@notjusthosting.com>
    Safety fixes
        -- Eugene Hopkinson <slowriot at voxelstorm dot com>
*/

#include "wpi/util/sha1.hpp"

#include <string>

#include "wpi/util/SmallVector.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/raw_istream.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace wpi;

static const size_t BLOCK_INTS =
    16; /* number of 32bit integers per SHA1 block */
static const size_t BLOCK_BYTES = BLOCK_INTS * 4;

static void reset(uint32_t digest[], size_t& buf_size, uint64_t& transforms) {
  /* SHA1 initialization constants */
  digest[0] = 0x67452301;
  digest[1] = 0xefcdab89;
  digest[2] = 0x98badcfe;
  digest[3] = 0x10325476;
  digest[4] = 0xc3d2e1f0;

  /* Reset counters */
  buf_size = 0;
  transforms = 0;
}

static uint32_t rol(const uint32_t value, const size_t bits) {
  return (value << bits) | (value >> (32 - bits));
}

static uint32_t blk(const uint32_t block[BLOCK_INTS], const size_t i) {
  return rol(block[(i + 13) & 15] ^ block[(i + 8) & 15] ^ block[(i + 2) & 15] ^
                 block[i],
             1);
}

/*
 * (R0+R1), R2, R3, R4 are the different operations used in SHA1
 */

static void R0(const uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t& w,
               const uint32_t x, const uint32_t y, uint32_t& z,
               const size_t i) {
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol(v, 5);
  w = rol(w, 30);
}

static void R1(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t& w,
               const uint32_t x, const uint32_t y, uint32_t& z,
               const size_t i) {
  block[i] = blk(block, i);
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol(v, 5);
  w = rol(w, 30);
}

static void R2(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t& w,
               const uint32_t x, const uint32_t y, uint32_t& z,
               const size_t i) {
  block[i] = blk(block, i);
  z += (w ^ x ^ y) + block[i] + 0x6ed9eba1 + rol(v, 5);
  w = rol(w, 30);
}

static void R3(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t& w,
               const uint32_t x, const uint32_t y, uint32_t& z,
               const size_t i) {
  block[i] = blk(block, i);
  z += (((w | x) & y) | (w & x)) + block[i] + 0x8f1bbcdc + rol(v, 5);
  w = rol(w, 30);
}

static void R4(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t& w,
               const uint32_t x, const uint32_t y, uint32_t& z,
               const size_t i) {
  block[i] = blk(block, i);
  z += (w ^ x ^ y) + block[i] + 0xca62c1d6 + rol(v, 5);
  w = rol(w, 30);
}

/*
 * Hash a single 512-bit block. This is the core of the algorithm.
 */

static void do_transform(uint32_t digest[], uint32_t block[BLOCK_INTS],
                         uint64_t& transforms) {
  /* Copy digest[] to working vars */
  uint32_t a = digest[0];
  uint32_t b = digest[1];
  uint32_t c = digest[2];
  uint32_t d = digest[3];
  uint32_t e = digest[4];

  /* 4 rounds of 20 operations each. Loop unrolled. */
  R0(block, a, b, c, d, e, 0);
  R0(block, e, a, b, c, d, 1);
  R0(block, d, e, a, b, c, 2);
  R0(block, c, d, e, a, b, 3);
  R0(block, b, c, d, e, a, 4);
  R0(block, a, b, c, d, e, 5);
  R0(block, e, a, b, c, d, 6);
  R0(block, d, e, a, b, c, 7);
  R0(block, c, d, e, a, b, 8);
  R0(block, b, c, d, e, a, 9);
  R0(block, a, b, c, d, e, 10);
  R0(block, e, a, b, c, d, 11);
  R0(block, d, e, a, b, c, 12);
  R0(block, c, d, e, a, b, 13);
  R0(block, b, c, d, e, a, 14);
  R0(block, a, b, c, d, e, 15);
  R1(block, e, a, b, c, d, 0);
  R1(block, d, e, a, b, c, 1);
  R1(block, c, d, e, a, b, 2);
  R1(block, b, c, d, e, a, 3);
  R2(block, a, b, c, d, e, 4);
  R2(block, e, a, b, c, d, 5);
  R2(block, d, e, a, b, c, 6);
  R2(block, c, d, e, a, b, 7);
  R2(block, b, c, d, e, a, 8);
  R2(block, a, b, c, d, e, 9);
  R2(block, e, a, b, c, d, 10);
  R2(block, d, e, a, b, c, 11);
  R2(block, c, d, e, a, b, 12);
  R2(block, b, c, d, e, a, 13);
  R2(block, a, b, c, d, e, 14);
  R2(block, e, a, b, c, d, 15);
  R2(block, d, e, a, b, c, 0);
  R2(block, c, d, e, a, b, 1);
  R2(block, b, c, d, e, a, 2);
  R2(block, a, b, c, d, e, 3);
  R2(block, e, a, b, c, d, 4);
  R2(block, d, e, a, b, c, 5);
  R2(block, c, d, e, a, b, 6);
  R2(block, b, c, d, e, a, 7);
  R3(block, a, b, c, d, e, 8);
  R3(block, e, a, b, c, d, 9);
  R3(block, d, e, a, b, c, 10);
  R3(block, c, d, e, a, b, 11);
  R3(block, b, c, d, e, a, 12);
  R3(block, a, b, c, d, e, 13);
  R3(block, e, a, b, c, d, 14);
  R3(block, d, e, a, b, c, 15);
  R3(block, c, d, e, a, b, 0);
  R3(block, b, c, d, e, a, 1);
  R3(block, a, b, c, d, e, 2);
  R3(block, e, a, b, c, d, 3);
  R3(block, d, e, a, b, c, 4);
  R3(block, c, d, e, a, b, 5);
  R3(block, b, c, d, e, a, 6);
  R3(block, a, b, c, d, e, 7);
  R3(block, e, a, b, c, d, 8);
  R3(block, d, e, a, b, c, 9);
  R3(block, c, d, e, a, b, 10);
  R3(block, b, c, d, e, a, 11);
  R4(block, a, b, c, d, e, 12);
  R4(block, e, a, b, c, d, 13);
  R4(block, d, e, a, b, c, 14);
  R4(block, c, d, e, a, b, 15);
  R4(block, b, c, d, e, a, 0);
  R4(block, a, b, c, d, e, 1);
  R4(block, e, a, b, c, d, 2);
  R4(block, d, e, a, b, c, 3);
  R4(block, c, d, e, a, b, 4);
  R4(block, b, c, d, e, a, 5);
  R4(block, a, b, c, d, e, 6);
  R4(block, e, a, b, c, d, 7);
  R4(block, d, e, a, b, c, 8);
  R4(block, c, d, e, a, b, 9);
  R4(block, b, c, d, e, a, 10);
  R4(block, a, b, c, d, e, 11);
  R4(block, e, a, b, c, d, 12);
  R4(block, d, e, a, b, c, 13);
  R4(block, c, d, e, a, b, 14);
  R4(block, b, c, d, e, a, 15);

  /* Add the working vars back into digest[] */
  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;

  /* Count the number of transformations */
  transforms++;
}

static void buffer_to_block(const unsigned char* buffer,
                            uint32_t block[BLOCK_INTS]) {
  /* Convert the std::string (byte buffer) to a uint32_t array (MSB) */
  for (size_t i = 0; i < BLOCK_INTS; i++) {
    block[i] = (buffer[4 * i + 3] & 0xff) | (buffer[4 * i + 2] & 0xff) << 8 |
               (buffer[4 * i + 1] & 0xff) << 16 |
               (buffer[4 * i + 0] & 0xff) << 24;
  }
}

SHA1::SHA1() {
  reset(digest, buf_size, transforms);
}

void SHA1::Update(std::string_view s) {
  raw_mem_istream is(std::span<const char>(s.data(), s.size()));
  Update(is);
}

void SHA1::Update(raw_istream& is) {
  while (true) {
    buf_size += is.readsome(&buffer[buf_size], BLOCK_BYTES - buf_size);
    if (buf_size != BLOCK_BYTES) {
      return;
    }
    uint32_t block[BLOCK_INTS];
    buffer_to_block(buffer, block);
    do_transform(digest, block, transforms);
    buf_size = 0;
  }
}

/*
 * Add padding and return the message digest.
 */

static void finalize(uint32_t digest[], unsigned char* buffer, size_t& buf_size,
                     uint64_t& transforms, raw_ostream& os, bool hex) {
  /* Total number of hashed bits */
  uint64_t total_bits = (transforms * BLOCK_BYTES + buf_size) * 8;

  /* Padding */
  buffer[buf_size++] = 0x80;
  for (size_t i = buf_size; i < BLOCK_BYTES; ++i) {
    buffer[i] = 0x00;
  }

  uint32_t block[BLOCK_INTS];
  buffer_to_block(buffer, block);

  if (buf_size > BLOCK_BYTES - 8) {
    do_transform(digest, block, transforms);
    for (size_t i = 0; i < BLOCK_INTS - 2; i++) {
      block[i] = 0;
    }
  }

  /* Append total_bits, split this uint64_t into two uint32_t */
  block[BLOCK_INTS - 1] = total_bits;
  block[BLOCK_INTS - 2] = (total_bits >> 32);
  do_transform(digest, block, transforms);

  /* Hex string */
  static const char* const LUT = "0123456789abcdef";
  for (size_t i = 0; i < 5; i++) {
    uint32_t v = digest[i];
    if (hex) {
      os << LUT[(v >> 28) & 0xf] << LUT[(v >> 24) & 0xf] << LUT[(v >> 20) & 0xf]
         << LUT[(v >> 16) & 0xf] << LUT[(v >> 12) & 0xf] << LUT[(v >> 8) & 0xf]
         << LUT[(v >> 4) & 0xf] << LUT[(v >> 0) & 0xf];
    } else {
      os.write(static_cast<unsigned char>((v >> 24) & 0xff));
      os.write(static_cast<unsigned char>((v >> 16) & 0xff));
      os.write(static_cast<unsigned char>((v >> 8) & 0xff));
      os.write(static_cast<unsigned char>((v >> 0) & 0xff));
    }
  }

  /* Reset for next run */
  reset(digest, buf_size, transforms);
}

std::string SHA1::Final() {
  std::string out;
  raw_string_ostream os(out);

  finalize(digest, buffer, buf_size, transforms, os, true);

  return os.str();
}

std::string_view SHA1::Final(SmallVectorImpl<char>& buf) {
  raw_svector_ostream os(buf);

  finalize(digest, buffer, buf_size, transforms, os, true);

  return os.str();
}

std::string_view SHA1::RawFinal(SmallVectorImpl<char>& buf) {
  raw_svector_ostream os(buf);

  finalize(digest, buffer, buf_size, transforms, os, false);

  return os.str();
}

std::string SHA1::FromFile(std::string_view filename) {
  std::error_code ec;
  raw_fd_istream stream(filename, ec);
  SHA1 checksum;
  checksum.Update(stream);
  return checksum.Final();
}
