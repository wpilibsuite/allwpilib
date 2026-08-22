// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/UsbDeviceDetector.hpp"

#include <catch2/catch_test_macros.hpp>

#include "UsbDeviceDetectorImpl.hpp"

namespace wpi::net {

TEST_CASE("UsbDeviceDetector validates port", "[usb]") {
  REQUIRE_NOTHROW(UsbDeviceDetector{0, {}, {}});
  REQUIRE_NOTHROW(UsbDeviceDetector{3, {}, {}});
  REQUIRE_THROWS_AS(UsbDeviceDetector(-1, {}, {}), std::out_of_range);
  REQUIRE_THROWS_AS(UsbDeviceDetector(4, {}, {}), std::out_of_range);
}

TEST_CASE("SystemCore USB topology maps to physical ports", "[usb]") {
  REQUIRE(detail::GetSystemCoreUsbPort("3-1.1") == 0);
  REQUIRE(detail::GetSystemCoreUsbPort("3-1.2") == 1);
  REQUIRE(detail::GetSystemCoreUsbPort("3-1.3") == 2);
  REQUIRE(detail::GetSystemCoreUsbPort("3-1.4") == 3);
  REQUIRE(detail::GetSystemCoreUsbPort("3-1.1.2") == 0);

  REQUIRE_FALSE(detail::GetSystemCoreUsbPort("3-1"));
  REQUIRE_FALSE(detail::GetSystemCoreUsbPort("3-1.0"));
  REQUIRE_FALSE(detail::GetSystemCoreUsbPort("3-1.5"));
  REQUIRE_FALSE(detail::GetSystemCoreUsbPort("3-1.10"));
  REQUIRE_FALSE(detail::GetSystemCoreUsbPort("2-1.1"));
}

}  // namespace wpi::net
