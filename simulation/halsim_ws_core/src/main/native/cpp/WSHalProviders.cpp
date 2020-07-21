/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSHalProviders.h"

namespace wpilibws {

static void performDiff(const wpi::json& last, const wpi::json& result,
                        wpi::json& out) {
  for (auto iter = result.cbegin(); iter != result.cend(); ++iter) {
    auto key = iter.key();
    auto value = iter.value();
    auto otherIter = last.find(key);
    if (otherIter == last.end()) {
      out[key] = value;
    } else {
      auto otherValue = otherIter.value();

      // object comparisons should recurse
      if (value.type() == wpi::json::value_t::object) {
        wpi::json vout;
        performDiff(otherValue, value, vout);
        if (!vout.empty()) {
          out[key] = vout;
        }

        // everything else is just a comparison
        // .. this would be inefficient with an array of objects, so don't do
        // that!
      } else if (otherValue != value) {
        out[key] = value;
      }
    }
  }
}

void HALSimWSHalProvider::OnNetworkConnected(
    std::shared_ptr<HALSimBaseWebSocketConnection> ws) {
  {
    std::lock_guard lock(mutex);
    // previous values don't matter anymore
    last.clear();
    // store a weak reference to the websocket object
    m_ws = ws;
  }

  // trigger a send of the current state
  // -> even if this gets called before, it's ok, because we cleared the
  //    state above atomically
  OnSimCallback("");
}

void HALSimWSHalProvider::OnStaticSimCallback(const char* name, void* param,
                                              const struct HAL_Value* value) {
  static_cast<HALSimWSHalProvider*>(param)->OnSimCallback(name);
}

void HALSimWSHalProvider::OnSimCallback(const char* cbName) {
  wpi::json diff;

  // Ensures all operations are performed in-order
  // -> this includes the network send
  std::lock_guard lock(mutex);

  auto result = OnSimValueChanged(cbName);
  performDiff(last, result, diff);
  last = std::move(result);

  // send it out if it's not empty
  if (!diff.empty()) {
    auto ws = m_ws.lock();
    if (ws) {
      wpi::json netValue = {
          {"type", m_type}, {"device", m_deviceId}, {"data", diff}};
      ws->OnSimValueChanged(netValue);
    }
  }
}

HALSimWSHalChanProvider::HALSimWSHalChanProvider(int32_t channel,
                                                 const std::string& key,
                                                 const std::string& type)
    : HALSimWSHalProvider(key, type), m_channel(channel) {
  m_deviceId = std::to_string(channel);
}

}  // namespace wpilibws
