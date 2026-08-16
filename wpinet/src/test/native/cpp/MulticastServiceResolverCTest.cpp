// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "MulticastHandleManager.hpp"
#include "wpi/net/MulticastServiceResolver.hpp"

namespace wpi::net {

class MulticastServiceResolverCTest {
 protected:
  static void AddData(WPI_MulticastServiceResolverHandle handle) {
    MulticastServiceResolver::ServiceData data;
    data.hostName = "host";
    data.serviceName = "service";
    GetMulticastManager().resolvers[handle]->PushData(std::move(data));
  }
};

TEST_CASE_METHOD(MulticastServiceResolverCTest,
                 "C API resolver returns result count",
                 "[multicast][service-discovery]") {
  auto handle = WPI_CreateMulticastServiceResolver("_count._tcp");
  AddData(handle);

  int32_t count = -1;
  WPI_ServiceData* result = WPI_GetMulticastServiceResolverData(handle, &count);
  REQUIRE(result != nullptr);
  CHECK(count == 1);

  WPI_FreeServiceData(result, count);
  WPI_FreeMulticastServiceResolver(handle);
}

TEST_CASE_METHOD(MulticastServiceResolverCTest, "C API resolver result layout",
                 "[multicast][service-discovery]") {
  auto handle = WPI_CreateMulticastServiceResolver("_layout._tcp");
  AddData(handle);

  int32_t count = 0;
  WPI_ServiceData* result = WPI_GetMulticastServiceResolverData(handle, &count);
  REQUIRE(result != nullptr);
  CHECK(result[0].hostName ==
        reinterpret_cast<const char*>(result) + sizeof(WPI_ServiceData));

  WPI_FreeServiceData(result, 1);
  WPI_FreeMulticastServiceResolver(handle);
}

}  // namespace wpi::net
