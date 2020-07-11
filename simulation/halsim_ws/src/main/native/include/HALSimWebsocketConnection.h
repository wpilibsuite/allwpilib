#pragma once

#include <memory>

#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/uv/Buffer.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Stream.h>
#include <wpi/WebSocket.h>

class HALSimWS;

class HALSimWebsocketConnection
  : public std::enable_shared_from_this<HALSimWebsocketConnection> {
  public:
    using BufferPool = wpi::uv::SimpleBufferPool<4>;

    using LoopFunc = std::function<void(void)>;
    using UvExecFunc = wpi::uv::AsyncFunction<void(LoopFunc)>;

    explicit HALSimWebsocketConnection(std::shared_ptr<wpi::uv::Stream> stream)
      :m_stream(stream) {}

  public:
    // callable from any thread
    // TODO This should be renamed to SendToEndpoint
    void OnSimValueChanged(const wpi::json& msg);
    void Initialize();

  private:
    std::shared_ptr<wpi::uv::Stream> m_stream;

    bool m_ws_connected = false;
    wpi::WebSocket* m_websocket = nullptr;


    // Only valid id the websocket is connected
    std::shared_ptr<UvExecFunc> m_exec;
    std::unique_ptr<BufferPool> m_buffers;
    std::mutex m_buffers_mutex;
};