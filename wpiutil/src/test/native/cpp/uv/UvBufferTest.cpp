/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Buffer.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

namespace wpi {
namespace uv {

TEST(UvSimpleBufferPool, ConstructDefault) {
  SimpleBufferPool<> pool;
  auto buf1 = pool.Allocate();
  ASSERT_EQ(buf1.len, 4096u);
}

TEST(UvSimpleBufferPool, ConstructSize) {
  SimpleBufferPool<4> pool{8192};
  auto buf1 = pool.Allocate();
  ASSERT_EQ(buf1.len, 8192u);
}

TEST(UvSimpleBufferPool, ReleaseReuse) {
  SimpleBufferPool<4> pool;
  auto buf1 = pool.Allocate();
  auto buf1copy = buf1;
  auto origSize = buf1.len;
  buf1.len = 8;
  pool.Release(buf1);
  ASSERT_EQ(buf1.base, nullptr);
  auto buf2 = pool.Allocate();
  ASSERT_EQ(buf1copy.base, buf2.base);
  ASSERT_EQ(buf2.len, origSize);
}

TEST(UvSimpleBufferPool, ClearRemaining) {
  SimpleBufferPool<4> pool;
  auto buf1 = pool.Allocate();
  pool.Release(buf1);
  ASSERT_EQ(pool.Remaining(), 1u);
  pool.Clear();
  ASSERT_EQ(pool.Remaining(), 0u);
}

}  // namespace uv
}  // namespace wpi
