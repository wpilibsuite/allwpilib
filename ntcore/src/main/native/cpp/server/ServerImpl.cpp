// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerImpl.h"

#include <stdint.h>

#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <fmt/ranges.h>
#include <wpi/Base64.h>
#include <wpi/MessagePack.h>
#include <wpi/SmallVector.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/json.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "Log.h"
#include "net/WireEncoder.h"
#include "net3/WireConnection3.h"
#include "net3/WireEncoder3.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"
#include "server/MessagePackWriter.h"
#include "server/ServerClient3.h"
#include "server/ServerClient4.h"
#include "server/ServerClientLocal.h"
#include "server/ServerPublisher.h"
#include "server/ServerTopic.h"

using namespace nt;
using namespace nt::server;
using namespace mpack;

ServerImpl::ServerImpl(wpi::Logger& logger) : m_logger{logger} {
  // local is client 0
  m_clients.emplace_back(std::make_unique<ServerClientLocal>(*this, 0, logger));
  m_localClient = static_cast<ServerClientLocal*>(m_clients.back().get());
}

std::pair<std::string, int> ServerImpl::AddClient(std::string_view name,
                                                  std::string_view connInfo,
                                                  bool local,
                                                  net::WireConnection& wire,
                                                  SetPeriodicFunc setPeriodic) {
  if (name.empty()) {
    name = "NT4";
  }
  size_t index = m_clients.size();
  // find an empty slot
  // just do a linear search as number of clients is typically small (<10)
  for (size_t i = 0, end = index; i < end; ++i) {
    if (!m_clients[i]) {
      index = i;
      break;
    }
  }
  if (index == m_clients.size()) {
    m_clients.emplace_back();
  }

  // ensure name is unique by suffixing index
  std::string dedupName = fmt::format("{}@{}", name, index);

  auto& clientData = m_clients[index];
  clientData = std::make_unique<ServerClient4>(dedupName, connInfo, local, wire,
                                               std::move(setPeriodic), *this,
                                               index, m_logger);

  // create client meta topics
  clientData->m_metaPub =
      CreateMetaTopic(fmt::format("$clientpub${}", dedupName));
  clientData->m_metaSub =
      CreateMetaTopic(fmt::format("$clientsub${}", dedupName));

  // update meta topics
  clientData->UpdateMetaClientPub();
  clientData->UpdateMetaClientSub();

  DEBUG3("AddClient('{}', '{}') -> {}", name, connInfo, index);
  return {std::move(dedupName), index};
}

int ServerImpl::AddClient3(std::string_view connInfo, bool local,
                           net3::WireConnection3& wire,
                           Connected3Func connected,
                           SetPeriodicFunc setPeriodic) {
  size_t index = m_clients.size();
  // find an empty slot; we can't check for duplicates until we get a hello.
  // just do a linear search as number of clients is typically small (<10)
  for (size_t i = 0, end = index; i < end; ++i) {
    if (!m_clients[i]) {
      index = i;
      break;
    }
  }
  if (index == m_clients.size()) {
    m_clients.emplace_back();
  }

  m_clients[index] = std::make_unique<ServerClient3>(
      connInfo, local, wire, std::move(connected), std::move(setPeriodic),
      *this, index, m_logger);

  DEBUG3("AddClient3('{}') -> {}", connInfo, index);
  return index;
}

std::shared_ptr<void> ServerImpl::RemoveClient(int clientId) {
  DEBUG3("RemoveClient({})", clientId);
  auto& client = m_clients[clientId];

  // remove all publishers and subscribers for this client
  wpi::SmallVector<ServerTopic*, 16> toDelete;
  for (auto&& topic : m_topics) {
    bool pubChanged = false;
    bool subChanged = false;
    auto tcdIt = topic->clients.find(client.get());
    if (tcdIt != topic->clients.end()) {
      pubChanged = !tcdIt->second.publishers.empty();
      subChanged = !tcdIt->second.subscribers.empty();
      topic->publisherCount -= tcdIt->second.publishers.size();
      topic->clients.erase(tcdIt);
    }

    if (!topic->IsPublished()) {
      toDelete.push_back(topic.get());
    } else {
      if (pubChanged) {
        UpdateMetaTopicPub(topic.get());
      }
      if (subChanged) {
        UpdateMetaTopicSub(topic.get());
      }
    }
  }

  // delete unpublished topics
  for (auto topic : toDelete) {
    DeleteTopic(topic);
  }
  DeleteTopic(client->m_metaPub);
  DeleteTopic(client->m_metaSub);

  return std::move(client);
}

bool ServerImpl::PersistentChanged() {
  bool rv = m_persistentChanged;
  m_persistentChanged = false;
  return rv;
}

static void DumpValue(wpi::raw_ostream& os, const Value& value,
                      wpi::json::serializer& s) {
  switch (value.type()) {
    case NT_BOOLEAN:
      if (value.GetBoolean()) {
        os << "true";
      } else {
        os << "false";
      }
      break;
    case NT_DOUBLE:
      s.dump_float(value.GetDouble());
      break;
    case NT_FLOAT:
      s.dump_float(value.GetFloat());
      break;
    case NT_INTEGER:
      s.dump_integer(value.GetInteger());
      break;
    case NT_STRING:
      os << '"';
      s.dump_escaped(value.GetString(), false);
      os << '"';
      break;
    case NT_RAW:
    case NT_RPC:
      os << '"';
      wpi::Base64Encode(os, value.GetRaw());
      os << '"';
      break;
    case NT_BOOLEAN_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetBooleanArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        if (v) {
          os << "true";
        } else {
          os << "false";
        }
      }
      os << ']';
      break;
    }
    case NT_DOUBLE_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetDoubleArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        s.dump_float(v);
      }
      os << ']';
      break;
    }
    case NT_FLOAT_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetFloatArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        s.dump_float(v);
      }
      os << ']';
      break;
    }
    case NT_INTEGER_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetIntegerArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        s.dump_integer(v);
      }
      os << ']';
      break;
    }
    case NT_STRING_ARRAY: {
      os << '[';
      bool first = true;
      for (auto&& v : value.GetStringArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        os << '"';
        s.dump_escaped(v, false);
        os << '"';
      }
      os << ']';
      break;
    }
    default:
      os << "null";
      break;
  }
}

void ServerImpl::DumpPersistent(wpi::raw_ostream& os) {
  wpi::json::serializer s{os, ' ', 16};
  os << "[\n";
  bool first = true;
  for (const auto& topic : m_topics) {
    if (!topic->persistent || !topic->lastValue) {
      continue;
    }
    if (first) {
      first = false;
    } else {
      os << ",\n";
    }
    os << "  {\n    \"name\": \"";
    s.dump_escaped(topic->name, false);
    os << "\",\n    \"type\": \"";
    s.dump_escaped(topic->typeStr, false);
    os << "\",\n    \"value\": ";
    DumpValue(os, topic->lastValue, s);
    os << ",\n    \"properties\": ";
    s.dump(topic->properties, true, false, 2, 4);
    os << "\n  }";
  }
  os << "\n]\n";
}

static std::string* ObjGetString(wpi::json::object_t& obj, std::string_view key,
                                 std::string* error) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    *error = fmt::format("no {} key", key);
    return nullptr;
  }
  auto val = it->second.get_ptr<std::string*>();
  if (!val) {
    *error = fmt::format("{} must be a string", key);
  }
  return val;
}

std::string ServerImpl::LoadPersistent(std::string_view in) {
  if (in.empty()) {
    return {};
  }

  wpi::json j;
  try {
    j = wpi::json::parse(in);
  } catch (wpi::json::parse_error& err) {
    return fmt::format("could not decode JSON: {}", err.what());
  }

  if (!j.is_array()) {
    return "expected JSON array at top level";
  }

  bool persistentChanged = m_persistentChanged;

  std::string allerrors;
  int i = -1;
  auto time = nt::Now();
  for (auto&& jitem : j) {
    ++i;
    std::string error;
    {
      auto obj = jitem.get_ptr<wpi::json::object_t*>();
      if (!obj) {
        error = "expected item to be an object";
        goto err;
      }

      // name
      auto name = ObjGetString(*obj, "name", &error);
      if (!name) {
        goto err;
      }

      // type
      auto typeStr = ObjGetString(*obj, "type", &error);
      if (!typeStr) {
        goto err;
      }

      // properties
      auto propsIt = obj->find("properties");
      if (propsIt == obj->end()) {
        error = "no properties key";
        goto err;
      }
      auto& props = propsIt->second;
      if (!props.is_object()) {
        error = "properties must be an object";
        goto err;
      }

      // check to make sure persistent property is set
      auto persistentIt = props.find("persistent");
      if (persistentIt == props.end()) {
        error = "no persistent property";
        goto err;
      }
      if (auto v = persistentIt->get_ptr<bool*>()) {
        if (!*v) {
          error = "persistent property is false";
          goto err;
        }
      } else {
        error = "persistent property is not boolean";
        goto err;
      }

      // value
      auto valueIt = obj->find("value");
      if (valueIt == obj->end()) {
        error = "no value key";
        goto err;
      }
      Value value;
      if (*typeStr == "boolean") {
        if (auto v = valueIt->second.get_ptr<bool*>()) {
          value = Value::MakeBoolean(*v, time);
        } else {
          error = "value type mismatch, expected boolean";
          goto err;
        }
      } else if (*typeStr == "int") {
        if (auto v = valueIt->second.get_ptr<int64_t*>()) {
          value = Value::MakeInteger(*v, time);
        } else if (auto v = valueIt->second.get_ptr<uint64_t*>()) {
          value = Value::MakeInteger(*v, time);
        } else {
          error = "value type mismatch, expected int";
          goto err;
        }
      } else if (*typeStr == "float") {
        if (auto v = valueIt->second.get_ptr<double*>()) {
          value = Value::MakeFloat(*v, time);
        } else {
          error = "value type mismatch, expected float";
          goto err;
        }
      } else if (*typeStr == "double") {
        if (auto v = valueIt->second.get_ptr<double*>()) {
          value = Value::MakeDouble(*v, time);
        } else {
          error = "value type mismatch, expected double";
          goto err;
        }
      } else if (*typeStr == "string" || *typeStr == "json") {
        if (auto v = valueIt->second.get_ptr<std::string*>()) {
          value = Value::MakeString(*v, time);
        } else {
          error = "value type mismatch, expected string";
          goto err;
        }
      } else if (*typeStr == "boolean[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<int> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<bool*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected boolean";
          }
        }
        value = Value::MakeBooleanArray(elems, time);
      } else if (*typeStr == "int[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<int64_t> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<int64_t*>()) {
            elems.push_back(*v);
          } else if (auto v = jelem.get_ptr<uint64_t*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected int";
          }
        }
        value = Value::MakeIntegerArray(elems, time);
      } else if (*typeStr == "double[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<double> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<double*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected double";
          }
        }
        value = Value::MakeDoubleArray(elems, time);
      } else if (*typeStr == "float[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<float> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<double*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected float";
          }
        }
        value = Value::MakeFloatArray(elems, time);
      } else if (*typeStr == "string[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<std::string> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<std::string*>()) {
            elems.emplace_back(*v);
          } else {
            error = "value type mismatch, expected string";
          }
        }
        value = Value::MakeStringArray(std::move(elems), time);
      } else {
        // raw
        if (auto v = valueIt->second.get_ptr<std::string*>()) {
          std::vector<uint8_t> data;
          wpi::Base64Decode(*v, &data);
          value = Value::MakeRaw(std::move(data), time);
        } else {
          error = "value type mismatch, expected string";
          goto err;
        }
      }

      // create persistent topic
      auto topic = CreateTopic(nullptr, *name, *typeStr, props);

      // set value
      SetValue(nullptr, topic, value);

      continue;
    }
  err:
    allerrors += fmt::format("{}: {}\n", i, error);
  }

  m_persistentChanged = persistentChanged;  // restore flag

  return allerrors;
}

ServerTopic* ServerImpl::CreateTopic(ServerClient* client,
                                     std::string_view name,
                                     std::string_view typeStr,
                                     const wpi::json& properties,
                                     bool special) {
  auto& topic = m_nameTopics[name];
  if (topic) {
    if (typeStr != topic->typeStr) {
      if (client) {
        WARN("client {} publish '{}' conflicting type '{}' (currently '{}')",
             client->GetName(), name, typeStr, topic->typeStr);
      }
    }
  } else {
    // new topic
    unsigned int id = m_topics.emplace_back(
        std::make_unique<ServerTopic>(m_logger, name, typeStr, properties));
    topic = m_topics[id].get();
    topic->id = id;
    topic->special = special;

    for (auto&& aClient : m_clients) {
      if (!aClient) {
        continue;
      }

      // look for subscriber matching prefixes
      wpi::SmallVector<ServerSubscriber*, 16> subscribersBuf;
      auto subscribers =
          aClient->GetSubscribers(name, topic->special, subscribersBuf);

      // don't announce to this client if no subscribers
      if (subscribers.empty()) {
        continue;
      }

      auto& tcd = topic->clients[aClient.get()];
      bool added = false;
      for (auto subscriber : subscribers) {
        if (tcd.AddSubscriber(subscriber)) {
          added = true;
        }
      }
      if (added) {
        aClient->UpdatePeriod(tcd, topic);
      }

      if (aClient.get() == client) {
        continue;  // don't announce to requesting client again
      }

      DEBUG4("client {}: announce {}", aClient->GetId(), topic->name);
      aClient->SendAnnounce(topic, std::nullopt);
    }

    // create meta topics; don't create if topic is itself a meta topic
    if (!special) {
      topic->metaPub = CreateMetaTopic(fmt::format("$pub${}", name));
      topic->metaSub = CreateMetaTopic(fmt::format("$sub${}", name));
      UpdateMetaTopicPub(topic);
      UpdateMetaTopicSub(topic);
    }
  }

  return topic;
}

ServerTopic* ServerImpl::CreateMetaTopic(std::string_view name) {
  return CreateTopic(nullptr, name, "msgpack", {{"retained", true}}, true);
}

void ServerImpl::DeleteTopic(ServerTopic* topic) {
  if (!topic) {
    return;
  }

  // delete meta topics
  if (topic->metaPub) {
    DeleteTopic(topic->metaPub);
  }
  if (topic->metaSub) {
    DeleteTopic(topic->metaSub);
  }

  // unannounce to all subscribers
  for (auto&& tcd : topic->clients) {
    if (!tcd.second.subscribers.empty()) {
      tcd.first->UpdatePeriod(tcd.second, topic);
      tcd.first->SendUnannounce(topic);
    }
  }

  // erase the topic
  m_nameTopics.erase(topic->name);
  m_topics.erase(topic->id);
}

void ServerImpl::SetProperties(ServerClient* client, ServerTopic* topic,
                               const wpi::json& update) {
  DEBUG4("SetProperties({}, {}, {})", client ? client->GetId() : -1,
         topic->name, update.dump());
  bool wasPersistent = topic->persistent;
  if (topic->SetProperties(update)) {
    // update persistentChanged flag
    if (topic->persistent != wasPersistent) {
      m_persistentChanged = true;
    }
    PropertiesChanged(client, topic, update);
  }
}

void ServerImpl::SetFlags(ServerClient* client, ServerTopic* topic,
                          unsigned int flags) {
  bool wasPersistent = topic->persistent;
  if (topic->SetFlags(flags)) {
    // update persistentChanged flag
    if (topic->persistent != wasPersistent) {
      m_persistentChanged = true;
      wpi::json update;
      if (topic->persistent) {
        update = {{"persistent", true}};
      } else {
        update = {{"persistent", wpi::json::object()}};
      }
      PropertiesChanged(client, topic, update);
    }
  }
}

void ServerImpl::SetValue(ServerClient* client, ServerTopic* topic,
                          const Value& value) {
  // update retained value if from same client or timestamp newer
  if (topic->cached && (!topic->lastValue || topic->lastValueClient == client ||
                        topic->lastValue.time() == 0 ||
                        value.time() >= topic->lastValue.time())) {
    DEBUG4("updating '{}' last value (time was {} is {})", topic->name,
           topic->lastValue.time(), value.time());
    topic->lastValue = value;
    topic->lastValueClient = client;

    // if persistent, update flag
    if (topic->persistent) {
      m_persistentChanged = true;
    }
  }

  for (auto&& tcd : topic->clients) {
    if (tcd.first != client &&
        tcd.second.sendMode != net::ValueSendMode::kDisabled) {
      tcd.first->SendValue(topic, value, tcd.second.sendMode);
    }
  }
}

void ServerImpl::UpdateMetaClients(const std::vector<ConnectionInfo>& conns) {
  Writer w;
  mpack_start_array(&w, conns.size());
  for (auto&& conn : conns) {
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "id");
    mpack_write_str(&w, conn.remote_id);
    mpack_write_str(&w, "conn");
    mpack_write_str(&w, fmt::format("{}:{}", conn.remote_ip, conn.remote_port));
    mpack_write_str(&w, "ver");
    mpack_write_u16(&w, conn.protocol_version);
    mpack_finish_map(&w);
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, m_metaClients, Value::MakeRaw(std::move(w.bytes)));
  } else {
    DEBUG4("failed to encode $clients");
  }
}

void ServerImpl::UpdateMetaTopicPub(ServerTopic* topic) {
  if (!topic->metaPub) {
    return;
  }
  Writer w;
  uint32_t count = 0;
  for (auto&& tcd : topic->clients) {
    count += tcd.second.publishers.size();
  }
  mpack_start_array(&w, count);
  for (auto&& tcd : topic->clients) {
    for (auto&& pub : tcd.second.publishers) {
      mpack_write_object_bytes(
          &w, reinterpret_cast<const char*>(pub->metaTopic.data()),
          pub->metaTopic.size());
    }
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaPub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerImpl::UpdateMetaTopicSub(ServerTopic* topic) {
  if (!topic->metaSub) {
    return;
  }
  Writer w;
  uint32_t count = 0;
  for (auto&& tcd : topic->clients) {
    count += tcd.second.subscribers.size();
  }
  mpack_start_array(&w, count);
  for (auto&& tcd : topic->clients) {
    for (auto&& sub : tcd.second.subscribers) {
      mpack_write_object_bytes(
          &w, reinterpret_cast<const char*>(sub->metaTopic.data()),
          sub->metaTopic.size());
    }
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaSub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerImpl::PropertiesChanged(ServerClient* client, ServerTopic* topic,
                                   const wpi::json& update) {
  // removing some properties can result in the topic being unpublished
  if (!topic->IsPublished()) {
    DeleteTopic(topic);
  } else {
    // send updated announcement to all subscribers
    for (auto&& tcd : topic->clients) {
      tcd.first->SendPropertiesUpdate(topic, update, tcd.first == client);
    }
  }
}

void ServerImpl::SendAllOutgoing(uint64_t curTimeMs, bool flush) {
  for (auto&& client : m_clients) {
    if (client) {
      client->SendOutgoing(curTimeMs, flush);
    }
  }
}

void ServerImpl::SendOutgoing(int clientId, uint64_t curTimeMs) {
  if (auto client = m_clients[clientId].get()) {
    client->SendOutgoing(curTimeMs, false);
  }
}

void ServerImpl::SetLocal(net::ServerMessageHandler* local,
                          net::ClientMessageQueue* queue) {
  DEBUG4("SetLocal()");
  m_local = local;
  m_localClient->SetQueue(queue);

  // create server meta topics
  m_metaClients = CreateMetaTopic("$clients");

  // create local client meta topics
  m_localClient->m_metaPub = CreateMetaTopic("$serverpub");
  m_localClient->m_metaSub = CreateMetaTopic("$serversub");

  // update meta topics
  m_localClient->UpdateMetaClientPub();
  m_localClient->UpdateMetaClientSub();
}

bool ServerImpl::ProcessIncomingText(int clientId, std::string_view data) {
  if (auto client = m_clients[clientId].get()) {
    return client->ProcessIncomingText(data);
  } else {
    return false;
  }
}

bool ServerImpl::ProcessIncomingBinary(int clientId,
                                       std::span<const uint8_t> data) {
  if (auto client = m_clients[clientId].get()) {
    return client->ProcessIncomingBinary(data);
  } else {
    return false;
  }
}

bool ServerImpl::ProcessIncomingMessages(size_t max) {
  DEBUG4("ProcessIncomingMessages({})", max);
  bool rv = false;
  for (auto&& client : m_clients) {
    if (client && client->ProcessIncomingMessages(max)) {
      rv = true;
    }
  }
  return rv;
}

bool ServerImpl::ProcessLocalMessages(size_t max) {
  DEBUG4("ProcessLocalMessages({})", max);
  return m_localClient->ProcessIncomingMessages(max);
}

void ServerImpl::ConnectionsChanged(const std::vector<ConnectionInfo>& conns) {
  UpdateMetaClients(conns);
}

std::string ServerImpl::DumpPersistent() {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  DumpPersistent(os);
  os.flush();
  return rv;
}
