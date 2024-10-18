// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerSubscriber.h"

#include <utility>

#include <wpi/MessagePack.h>
#include <wpi/StringExtras.h>

#include "PubSubOptions.h"
#include "server/MessagePackWriter.h"

using namespace nt;
using namespace nt::server;
using namespace mpack;

static void WriteOptions(mpack_writer_t& w, const PubSubOptionsImpl& options) {
  int size =
      (options.sendAll ? 1 : 0) + (options.topicsOnly ? 1 : 0) +
      (options.periodicMs != PubSubOptionsImpl::kDefaultPeriodicMs ? 1 : 0) +
      (options.prefixMatch ? 1 : 0);
  mpack_start_map(&w, size);
  if (options.sendAll) {
    mpack_write_str(&w, "all");
    mpack_write_bool(&w, true);
  }
  if (options.topicsOnly) {
    mpack_write_str(&w, "topicsonly");
    mpack_write_bool(&w, true);
  }
  if (options.periodicMs != PubSubOptionsImpl::kDefaultPeriodicMs) {
    mpack_write_str(&w, "periodic");
    mpack_write_float(&w, options.periodicMs / 1000.0);
  }
  if (options.prefixMatch) {
    mpack_write_str(&w, "prefix");
    mpack_write_bool(&w, true);
  }
  mpack_finish_map(&w);
}

bool ServerSubscriber::Matches(std::string_view name, bool special) {
  for (auto&& topicName : m_topicNames) {
    if ((!m_options.prefixMatch && name == topicName) ||
        (m_options.prefixMatch && (!special || !topicName.empty()) &&
         wpi::starts_with(name, topicName))) {
      return true;
    }
  }
  return false;
}

void ServerSubscriber::UpdateMeta() {
  {
    Writer w;
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "uid");
    mpack_write_int(&w, m_subuid);
    mpack_write_str(&w, "topics");
    mpack_start_array(&w, m_topicNames.size());
    for (auto&& name : m_topicNames) {
      mpack_write_str(&w, name);
    }
    mpack_finish_array(&w);
    mpack_write_str(&w, "options");
    WriteOptions(w, m_options);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      m_metaClient = std::move(w.bytes);
    }
  }
  {
    Writer w;
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "client");
    mpack_write_str(&w, m_clientName);
    mpack_write_str(&w, "subuid");
    mpack_write_int(&w, m_subuid);
    mpack_write_str(&w, "options");
    WriteOptions(w, m_options);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      m_metaTopic = std::move(w.bytes);
    }
  }
}
