#pragma once

#include <memory>

#include <wpi/json.h>

namespace wpilibws {

class HALSimBaseWebSocketConnection
    : public std::enable_shared_from_this<HALSimBaseWebSocketConnection> {
  public:
    virtual void OnSimValueChanged(const wpi::json& msg) = 0;

    std::shared_ptr<HALSimBaseWebSocketConnection> GetSharedFromThis() {
      return shared_from_this();
    }
};

} //end namespace wpilibws