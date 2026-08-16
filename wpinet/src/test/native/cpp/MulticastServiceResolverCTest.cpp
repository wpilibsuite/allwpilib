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

TEST_CASE("C API resolver returns result count",
          "[multicast][service-discovery]") {
  auto handle = WPI_CreateMulticastServiceResolver("_count._tcp");
  auto& manager = wpi::net::GetMulticastManager();
  {
    std::scoped_lock lock{manager.mutex};
    auto& data = manager.resolvers[handle]->queue.emplace_back();
    data.hostName = "host";
    data.serviceName = "service";
  }

  int32_t count = -1;
  WPI_ServiceData* result = WPI_GetMulticastServiceResolverData(handle, &count);
  REQUIRE(result != nullptr);
  CHECK(count == 1);

  WPI_FreeServiceData(result, count);
  WPI_FreeMulticastServiceResolver(handle);
}
