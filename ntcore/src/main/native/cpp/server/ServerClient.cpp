// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerClient.h"

#include <utility>

#include <wpi/MessagePack.h>

#include "server/MessagePackWriter.h"
#include "server/ServerPublisher.h"
#include "server/ServerStorage.h"

using namespace nt::server;
using namespace mpack;

void ServerClient::UpdateMetaClientPub() {
  if (!m_metaPub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, m_publishers.size());
  for (auto&& pub : m_publishers) {
    mpack_write_object_bytes(&w, pub.second->GetMetaClientData());
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    m_storage.SetValue(nullptr, m_metaPub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerClient::UpdateMetaClientSub() {
  if (!m_metaSub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, m_subscribers.size());
  for (auto&& sub : m_subscribers) {
    mpack_write_object_bytes(&w, sub.second->GetMetaClientData());
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    m_storage.SetValue(nullptr, m_metaSub, Value::MakeRaw(std::move(w.bytes)));
  }
}

std::span<ServerSubscriber*> ServerClient::GetSubscribers(
    std::string_view name, bool special,
    wpi::SmallVectorImpl<ServerSubscriber*>& buf) {
  buf.resize(0);
  for (auto&& subPair : m_subscribers) {
    ServerSubscriber* subscriber = subPair.getSecond().get();
    if (subscriber->Matches(name, special)) {
      buf.emplace_back(subscriber);
    }
  }
  return {buf.data(), buf.size()};
}
