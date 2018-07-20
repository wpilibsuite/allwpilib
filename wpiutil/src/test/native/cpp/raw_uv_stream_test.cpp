/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/raw_uv_ostream.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"

namespace wpi {

TEST(RawUvStreamTest, BasicWrite) {
  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os(bufs, 1024);
  os << "12";
  os << "34";
  ASSERT_EQ(bufs.size(), 1u);
  ASSERT_EQ(bufs[0].len, 4u);
  ASSERT_EQ(bufs[0].base[0], '1');
  ASSERT_EQ(bufs[0].base[1], '2');
  ASSERT_EQ(bufs[0].base[2], '3');
  ASSERT_EQ(bufs[0].base[3], '4');
}

TEST(RawUvStreamTest, BoundaryWrite) {
  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os(bufs, 4);
  ASSERT_EQ(bufs.size(), 0u);
  os << "12";
  ASSERT_EQ(bufs.size(), 1u);
  os << "34";
  ASSERT_EQ(bufs.size(), 1u);
  os << "56";
  ASSERT_EQ(bufs.size(), 2u);
}

TEST(RawUvStreamTest, LargeWrite) {
  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os(bufs, 4);
  os << "123456";
  ASSERT_EQ(bufs.size(), 2u);
  ASSERT_EQ(bufs[1].len, 2u);
  ASSERT_EQ(bufs[1].base[0], '5');
}

TEST(RawUvStreamTest, PrevDataWrite) {
  SmallVector<uv::Buffer, 4> bufs;
  bufs.emplace_back(uv::Buffer::Allocate(1024));
  raw_uv_ostream os(bufs, 1024);
  os << "1234";
  ASSERT_EQ(bufs.size(), 2u);
  ASSERT_EQ(bufs[0].len, 1024u);
  ASSERT_EQ(bufs[1].len, 4u);
}

}  // namespace wpi
