// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/raw_uv_ostream.hpp"

#include <catch2/catch_test_macros.hpp>

namespace wpi::net {

TEST_CASE("RawUvOstreamTest BasicWrite", "[uv][ostream]") {
  wpi::util::SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os(bufs, 1024);
  os << "12";
  os << "34";
  REQUIRE(bufs.size() == 1u);
  REQUIRE(bufs[0].len == 4u);
  REQUIRE(bufs[0].base[0] == '1');
  REQUIRE(bufs[0].base[1] == '2');
  REQUIRE(bufs[0].base[2] == '3');
  REQUIRE(bufs[0].base[3] == '4');

  for (auto& buf : bufs) {
    buf.Deallocate();
  }
}

TEST_CASE("RawUvOstreamTest BoundaryWrite", "[uv][ostream]") {
  wpi::util::SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os(bufs, 4);
  REQUIRE(bufs.size() == 0u);
  os << "12";
  REQUIRE(bufs.size() == 1u);
  os << "34";
  REQUIRE(bufs.size() == 1u);
  os << "56";
  REQUIRE(bufs.size() == 2u);

  for (auto& buf : bufs) {
    buf.Deallocate();
  }
}

TEST_CASE("RawUvOstreamTest LargeWrite", "[uv][ostream]") {
  wpi::util::SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os(bufs, 4);
  os << "123456";
  REQUIRE(bufs.size() == 2u);
  REQUIRE(bufs[1].len == 2u);
  REQUIRE(bufs[1].base[0] == '5');

  for (auto& buf : bufs) {
    buf.Deallocate();
  }
}

TEST_CASE("RawUvOstreamTest PrevDataWrite", "[uv][ostream]") {
  wpi::util::SmallVector<uv::Buffer, 4> bufs;
  bufs.emplace_back(uv::Buffer::Allocate(1024));
  raw_uv_ostream os(bufs, 1024);
  os << "1234";
  REQUIRE(bufs.size() == 2u);
  REQUIRE(bufs[0].len == 1024u);
  REQUIRE(bufs[1].len == 4u);

  for (auto& buf : bufs) {
    buf.Deallocate();
  }
}

}  // namespace wpi::net
