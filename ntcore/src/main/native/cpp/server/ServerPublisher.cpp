// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerPublisher.hpp"

#include <utility>

#include "server/MessagePackWriter.hpp"
#include "server/ServerTopic.hpp"
#include "wpi/util/MessagePack.hpp"

using namespace wpi::nt::server;
using namespace mpack;

void ServerPublisher::UpdateMeta() {
  {
    Writer w;
    mpack_start_map(&w, 2);
    mpack_write_str(&w, "uid");
    mpack_write_int(&w, m_pubuid);
    mpack_write_str(&w, "topic");
    mpack_write_str(&w, m_topic->name);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      m_metaClient = std::move(w.bytes);
    }
  }
  {
    Writer w;
    mpack_start_map(&w, 2);
    mpack_write_str(&w, "client");
    mpack_write_str(&w, m_clientName);
    mpack_write_str(&w, "pubuid");
    mpack_write_int(&w, m_pubuid);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      m_metaTopic = std::move(w.bytes);
    }
  }
}
