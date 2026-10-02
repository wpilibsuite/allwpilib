// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/net/uv/Buffer.hpp"
// clang-format on

#include <catch2/catch_test_macros.hpp>

namespace wpi::net::uv {

TEST_CASE("UvSimpleBufferPoolTest ConstructDefault", "[uv][buffer]") {
  SimpleBufferPool<> pool;
  auto buf1 = pool.Allocate();
  REQUIRE(buf1.len == 4096u);  // NOLINT
  pool.Release({&buf1, 1});
}

TEST_CASE("UvSimpleBufferPoolTest ConstructSize", "[uv][buffer]") {
  SimpleBufferPool<4> pool{8192};
  auto buf1 = pool.Allocate();
  REQUIRE(buf1.len == 8192u);  // NOLINT
  pool.Release({&buf1, 1});
}

TEST_CASE("UvSimpleBufferPoolTest ReleaseReuse", "[uv][buffer]") {
  SimpleBufferPool<4> pool;
  auto buf1 = pool.Allocate();
  auto buf1copy = buf1;
  auto origSize = buf1.len;
  buf1.len = 8;
  pool.Release({&buf1, 1});
  REQUIRE(buf1.base == nullptr);
  auto buf2 = pool.Allocate();
  REQUIRE(buf1copy.base == buf2.base);
  REQUIRE(buf2.len == origSize);
  pool.Release({&buf2, 1});
}

TEST_CASE("UvSimpleBufferPoolTest ClearRemaining", "[uv][buffer]") {
  SimpleBufferPool<4> pool;
  auto buf1 = pool.Allocate();
  pool.Release({&buf1, 1});
  REQUIRE(pool.Remaining() == 1u);
  pool.Clear();
  REQUIRE(pool.Remaining() == 0u);
}

}  // namespace wpi::net::uv
