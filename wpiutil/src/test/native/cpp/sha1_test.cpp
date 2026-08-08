// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

#include "wpi/util/sha1.hpp"

#include <string>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

namespace wpi::util {

/*
 * The 3 test vectors from FIPS PUB 180-1
 */

TEST_CASE("SHA1Test Standard1", "[wpiutil]") {
  SHA1 checksum;
  checksum.Update("abc");
  REQUIRE(checksum.Final() == "a9993e364706816aba3e25717850c26c9cd0d89d");
}

TEST_CASE("SHA1Test Standard2", "[wpiutil]") {
  SHA1 checksum;
  checksum.Update("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
  REQUIRE(checksum.Final() == "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
}

TEST_CASE("SHA1Test Standard3", "[wpiutil]") {
  SHA1 checksum;
  // A million repetitions of 'a'
  for (int i = 0; i < 1000000 / 200; ++i) {
    checksum.Update(
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  }
  REQUIRE(checksum.Final() == "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
}

/*
 * Other tests
 */

TEST_CASE("SHA1Test OtherNoString", "[wpiutil]") {
  SHA1 checksum;
  REQUIRE(checksum.Final() == "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST_CASE("SHA1Test OtherEmptyString", "[wpiutil]") {
  SHA1 checksum;
  checksum.Update("");
  REQUIRE(checksum.Final() == "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST_CASE("SHA1Test OtherABCDE", "[wpiutil]") {
  SHA1 checksum;
  checksum.Update("abcde");
  REQUIRE(checksum.Final() == "03de6c570bfe24bfc328ccd7ca46b76eadaf4334");
}

TEST_CASE("SHA1Test Concurrent", "[wpiutil]") {
  // Two concurrent checksum calculations
  SHA1 checksum1, checksum2;
  checksum1.Update("abc");
  REQUIRE(checksum2.Final() ==
          "da39a3ee5e6b4b0d3255bfef95601890afd80709"); /* "" */
  REQUIRE(checksum1.Final() ==
          "a9993e364706816aba3e25717850c26c9cd0d89d"); /* "abc" */
}

}  // namespace wpi::util
