// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#define private public
#include "wpi/net/MulticastServiceResolver.hpp"
#undef private

#include "MulticastHandleManager.hpp"

TEST_CASE("C API resolver result layout", "[multicast][service-discovery]") {
  auto handle = WPI_CreateMulticastServiceResolver("_layout._tcp");
  auto& manager = wpi::net::GetMulticastManager();
  {
    std::scoped_lock lock{manager.mutex};
    auto& data = manager.resolvers[handle]->queue.emplace_back();
    data.hostName = "host";
    data.serviceName = "service";
  }

  int32_t count = 0;
  WPI_ServiceData* result = WPI_GetMulticastServiceResolverData(handle, &count);
  REQUIRE(result != nullptr);
  CHECK(result[0].hostName ==
        reinterpret_cast<const char*>(result) + sizeof(WPI_ServiceData));

  WPI_FreeServiceData(result, 1);
  WPI_FreeMulticastServiceResolver(handle);
}
