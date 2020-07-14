#pragma once

#include <memory>

#include <HALSimBaseWebSocketConnection.h>

#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/uv/Buffer.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Stream.h>
#include <wpi/WebSocket.h>

namespace wpilibws {

class HALSimWS;

class HALSimWSClientConnection
  : public HALSimBaseWebSocketConnection {

  public:
    using BufferPool = wpi::uv::SimpleBufferPool<4>;
    using LoopFunc = std::function<void(void)>;
    using UvExecFunc = wpi::uv::AsyncFunction<void(LoopFunc)>;

    explicit HALSimWSClientConnection(std::shared_ptr<wpi::uv::Stream> stream)
      :m_stream(stream) {}

  public:
    void OnSimValueChanged(const wpi::json& msg) override;
    void Initialize();

  private:
    std::shared_ptr<wpi::uv::Stream> m_stream;

    bool m_ws_connected = false;
    wpi::WebSocket* m_websocket = nullptr;

    std::shared_ptr<UvExecFunc> m_exec;
    std::unique_ptr<BufferPool> m_buffers;
    std::mutex m_buffers_mutex;

};

}