#pragma once

#include <functional>
#include <memory>
#include <string>

#include <wpi/json.h>

#include "HALSimWebsocketConnection.h"

class HALSimWS;

class BaseProvider {
  public:
    explicit BaseProvider(const std::string& key);
    virtual ~BaseProvider() {}

    BaseProvider(const BaseProvider&) = delete;
    BaseProvider& operator=(const BaseProvider&) = delete;

    // Called when we have a websocket connection
    virtual void OnNetworkConnected(std::shared_ptr<HALSimWebsocketConnection> ws) = 0;

    virtual void OnEndpointValueChanged(const wpi::json& msg);

  protected:
    std::weak_ptr<HALSimWebsocketConnection> m_ws;
    std::string m_key;
};