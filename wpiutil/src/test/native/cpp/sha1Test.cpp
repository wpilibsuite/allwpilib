/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*
    test_sha1.cpp - test program of

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
*/

#include <string>

#include "gtest/gtest.h"
#include "wpi/sha1.h"

namespace wpi {

/*
 * The 3 test vectors from FIPS PUB 180-1
 */

TEST(SHA1Test, Standard1) {
  SHA1 checksum;
  checksum.Update("abc");
  ASSERT_EQ(checksum.Final(), "a9993e364706816aba3e25717850c26c9cd0d89d");
}

TEST(SHA1Test, Standard2) {
  SHA1 checksum;
  checksum.Update("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
  ASSERT_EQ(checksum.Final(), "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
}

TEST(SHA1Test, Standard3) {
  SHA1 checksum;
  // A million repetitions of 'a'
  for (int i = 0; i < 1000000 / 200; ++i) {
    checksum.Update(
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  }
  ASSERT_EQ(checksum.Final(), "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
}

/*
 * Other tests
 */

TEST(SHA1Test, OtherNoString) {
  SHA1 checksum;
  ASSERT_EQ(checksum.Final(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST(SHA1Test, OtherEmptyString) {
  SHA1 checksum;
  checksum.Update("");
  ASSERT_EQ(checksum.Final(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST(SHA1Test, OtherABCDE) {
  SHA1 checksum;
  checksum.Update("abcde");
  ASSERT_EQ(checksum.Final(), "03de6c570bfe24bfc328ccd7ca46b76eadaf4334");
}

TEST(SHA1Test, Concurrent) {
  // Two concurrent checksum calculations
  SHA1 checksum1, checksum2;
  checksum1.Update("abc");
  ASSERT_EQ(checksum2.Final(),
            "da39a3ee5e6b4b0d3255bfef95601890afd80709"); /* "" */
  ASSERT_EQ(checksum1.Final(),
            "a9993e364706816aba3e25717850c26c9cd0d89d"); /* "abc" */
}

}  // namespace wpi
