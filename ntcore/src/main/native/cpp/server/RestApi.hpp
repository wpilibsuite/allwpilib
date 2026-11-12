// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

namespace wpi::nt::server {

class ServerStorage;

struct RestResponse {
  int status;
  std::string_view statusText;
  std::string body;
  std::string_view allow;
  std::string_view contentType = "application/json";
};

/**
 * Handle a REST request on the server's event loop.
 *
 * @param storage server topic storage
 * @param method HTTP method
 * @param target HTTP request target (including query)
 * @param body request body
 * @param contentType request Content-Type header
 * @param accept request Accept header (empty defaults to JSON)
 * @return encoded response, media type, and HTTP status
 */
RestResponse HandleRestRequest(
    ServerStorage& storage, std::string_view method, std::string_view target,
    std::string_view body, std::string_view contentType = "application/json",
    std::string_view accept = {});

}  // namespace wpi::nt::server
