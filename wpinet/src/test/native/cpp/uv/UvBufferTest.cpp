// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Buffer.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

namespace wpi::uv {

TEST(UvSimpleBufferPoolTest, ConstructDefault) {
  SimpleBufferPool<> pool;
  auto buf1 = pool.Allocate();
  ASSERT_EQ(buf1.len, 4096u);  // NOLINT
  pool.Release({&buf1, 1});
}

TEST(UvSimpleBufferPoolTest, ConstructSize) {
  SimpleBufferPool<4> pool{8192};
  auto buf1 = pool.Allocate();
  ASSERT_EQ(buf1.len, 8192u);  // NOLINT
  pool.Release({&buf1, 1});
}

TEST(UvSimpleBufferPoolTest, ReleaseReuse) {
  SimpleBufferPool<4> pool;
  auto buf1 = pool.Allocate();
  auto buf1copy = buf1;
  auto origSize = buf1.len;
  buf1.len = 8;
  pool.Release({&buf1, 1});
  ASSERT_EQ(buf1.base, nullptr);
  auto buf2 = pool.Allocate();
  ASSERT_EQ(buf1copy.base, buf2.base);
  ASSERT_EQ(buf2.len, origSize);
  pool.Release({&buf2, 1});
}

TEST(UvSimpleBufferPoolTest, ClearRemaining) {
  SimpleBufferPool<4> pool;
  auto buf1 = pool.Allocate();
  pool.Release({&buf1, 1});
  ASSERT_EQ(pool.Remaining(), 1u);
  pool.Clear();
  ASSERT_EQ(pool.Remaining(), 0u);
}

}  // namespace wpi::uv
