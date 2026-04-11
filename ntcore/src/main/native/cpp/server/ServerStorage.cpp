// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerStorage.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "Log.hpp"
#include "server/MessagePackWriter.hpp"
#include "server/ServerClient.hpp"
#include "wpi/util/Base64.hpp"
#include "wpi/util/MessagePack.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::nt;
using namespace wpi::nt::server;
using namespace mpack;

ServerTopic* ServerStorage::CreateTopic(ServerClient* client,
                                        std::string_view name,
                                        std::string_view typeStr,
                                        const wpi::util::json& properties,
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
    DEBUG4("creating topic '{}' with type '{}' and properties {}", name,
           typeStr, properties.to_string());
    // new topic
    unsigned int id = m_topics.emplace_back(
        std::make_unique<ServerTopic>(m_logger, name, typeStr, properties));
    topic = m_topics[id].get();
    topic->id = id;
    topic->special = special;

    m_sendAnnounce(topic, client);

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

ServerTopic* ServerStorage::CreateMetaTopic(std::string_view name) {
  return CreateTopic(nullptr, name, "msgpack",
                     wpi::util::json::object("retained", true), true);
}

void ServerStorage::DeleteTopic(ServerTopic* topic) {
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

void ServerStorage::SetProperties(ServerClient* client, ServerTopic* topic,
                                  const wpi::util::json& update) {
  DEBUG4("SetProperties({}, {}, {})", client ? client->GetId() : -1,
         topic->name, update.to_string());
  bool wasPersistent = topic->persistent;
  if (topic->SetProperties(update)) {
    // update persistentChanged flag
    if (topic->persistent != wasPersistent) {
      m_persistentChanged = true;
    }
    PropertiesChanged(client, topic, update);
  }
}

void ServerStorage::SetFlags(ServerClient* client, ServerTopic* topic,
                             unsigned int flags) {
  bool wasPersistent = topic->persistent;
  if (topic->SetFlags(flags)) {
    // update persistentChanged flag
    if (topic->persistent != wasPersistent) {
      m_persistentChanged = true;
      wpi::util::json update;
      if (topic->persistent) {
        update = wpi::util::json::object("persistent", true);
      } else {
        update =
            wpi::util::json::object("persistent", wpi::util::json::object());
      }
      PropertiesChanged(client, topic, update);
    }
  }
}

void ServerStorage::SetValue(ServerClient* client, ServerTopic* topic,
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

void ServerStorage::RemoveClient(ServerClient* client) {
  // remove all publishers and subscribers for this client
  wpi::util::SmallVector<ServerTopic*, 16> toDelete;
  for (auto&& topic : m_topics) {
    bool pubChanged = false;
    bool subChanged = false;
    auto tcdIt = topic->clients.find(client);
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
}

void ServerStorage::UpdateMetaTopicPub(ServerTopic* topic) {
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
      mpack_write_object_bytes(&w, pub->GetMetaTopicData());
    }
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaPub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerStorage::UpdateMetaTopicSub(ServerTopic* topic) {
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
      mpack_write_object_bytes(&w, sub->GetMetaTopicData());
    }
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaSub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerStorage::PropertiesChanged(ServerClient* client, ServerTopic* topic,
                                      const wpi::util::json& update) {
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

static void DumpValue(wpi::util::raw_ostream& os, const Value& value) {
  switch (value.type()) {
    case NT_BOOLEAN:
      wpi::util::json::stringify_bool(os, value.GetBoolean());
      break;
    case NT_DOUBLE:
      wpi::util::json::stringify_double(os, value.GetDouble());
      break;
    case NT_FLOAT:
      wpi::util::json::stringify_float(os, value.GetFloat());
      break;
    case NT_INTEGER:
      wpi::util::json::stringify_int(os, value.GetInteger());
      break;
    case NT_STRING:
      wpi::util::json::stringify_string(os, value.GetString());
      break;
    case NT_RAW:
    case NT_RPC:
      os << '"';
      wpi::util::Base64Encode(os, value.GetRaw());
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
        wpi::util::json::stringify_bool(os, v);
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
        wpi::util::json::stringify_double(os, v);
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
        wpi::util::json::stringify_float(os, v);
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
        wpi::util::json::stringify_int(os, v);
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
        wpi::util::json::stringify_string(os, v);
      }
      os << ']';
      break;
    }
    default:
      os << "null";
      break;
  }
}

void ServerStorage::DumpPersistent(wpi::util::raw_ostream& os) {
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
    os << "  {\n    \"name\": ";
    wpi::util::json::stringify_string(os, topic->name);
    os << ",\n    \"type\": ";
    wpi::util::json::stringify_string(os, topic->typeStr);
    os << ",\n    \"value\": ";
    DumpValue(os, topic->lastValue);
    os << ",\n    \"properties\": ";
    topic->properties.marshal(os, true, 2);
    os << "\n  }";
  }
  os << "\n]\n";
}

static std::string* ObjGetString(wpi::util::json& obj, std::string_view key,
                                 std::string* error) {
  auto value = obj.lookup(key);
  if (!value) {
    *error = fmt::format("no {} key", key);
    return nullptr;
  }
  if (!value->is_string()) {
    *error = fmt::format("{} must be a string", key);
    return nullptr;
  }
  return &value->get_string();
}

std::string ServerStorage::LoadPersistent(std::string_view in) {
  if (in.empty()) {
    return {};
  }

  auto j = wpi::util::json::parse(in);
  if (!j) {
    return fmt::format("could not decode JSON: {}", j.error());
  }

  if (!j->is_array()) {
    return "expected JSON array at top level";
  }

  bool persistentChanged = m_persistentChanged;

  std::string allerrors;
  int i = -1;
  auto time = wpi::nt::Now();
  for (auto&& jitem : j->get_array()) {
    ++i;
    std::string error;
    {
      if (!jitem.is_object()) {
        error = "expected item to be an object";
        goto err;
      }

      // name
      auto name = ObjGetString(jitem, "name", &error);
      if (!name) {
        goto err;
      }

      // type
      auto typeStr = ObjGetString(jitem, "type", &error);
      if (!typeStr) {
        goto err;
      }

      // properties
      auto props = jitem.lookup("properties");
      if (!props) {
        error = "no properties key";
        goto err;
      }
      if (!props->is_object()) {
        error = "properties must be an object";
        goto err;
      }

      // check to make sure persistent property is set
      auto persistent = props->lookup("persistent");
      if (!persistent) {
        error = "no persistent property";
        goto err;
      }
      if (persistent->is_bool()) {
        if (!persistent->get_bool()) {
          error = "persistent property is false";
          goto err;
        }
      } else {
        error = "persistent property is not boolean";
        goto err;
      }

      // value
      auto jvalue = jitem.lookup("value");
      if (!jvalue) {
        error = "no value key";
        goto err;
      }
      Value value;
      if (*typeStr == "boolean") {
        if (jvalue->is_bool()) {
          value = Value::MakeBoolean(jvalue->get_bool(), time);
        } else {
          error = "value type mismatch, expected boolean";
          goto err;
        }
      } else if (*typeStr == "int") {
        if (jvalue->is_int()) {
          value = Value::MakeInteger(jvalue->get_int(), time);
        } else {
          error = "value type mismatch, expected int";
          goto err;
        }
      } else if (*typeStr == "float") {
        if (jvalue->is_number()) {
          value = Value::MakeFloat(jvalue->get_number(), time);
        } else {
          error = "value type mismatch, expected float";
          goto err;
        }
      } else if (*typeStr == "double") {
        if (jvalue->is_number()) {
          value = Value::MakeDouble(jvalue->get_number(), time);
        } else {
          error = "value type mismatch, expected double";
          goto err;
        }
      } else if (*typeStr == "string" || *typeStr == "json") {
        if (jvalue->is_string()) {
          value = Value::MakeString(jvalue->get_string(), time);
        } else {
          error = "value type mismatch, expected string";
          goto err;
        }
      } else if (*typeStr == "boolean[]") {
        if (!jvalue->is_array()) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<int> elems;
        for (auto&& jelem : jvalue->get_array()) {
          if (jelem.is_bool()) {
            elems.push_back(jelem.get_bool());
          } else {
            error = "value type mismatch, expected boolean";
          }
        }
        value = Value::MakeBooleanArray(elems, time);
      } else if (*typeStr == "int[]") {
        if (!jvalue->is_array()) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<int64_t> elems;
        for (auto&& jelem : jvalue->get_array()) {
          if (jelem.is_int()) {
            elems.push_back(jelem.get_int());
          } else {
            error = "value type mismatch, expected int";
          }
        }
        value = Value::MakeIntegerArray(elems, time);
      } else if (*typeStr == "double[]") {
        if (!jvalue->is_array()) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<double> elems;
        for (auto&& jelem : jvalue->get_array()) {
          if (jelem.is_number()) {
            elems.push_back(jelem.get_number());
          } else {
            error = "value type mismatch, expected double";
          }
        }
        value = Value::MakeDoubleArray(elems, time);
      } else if (*typeStr == "float[]") {
        if (!jvalue->is_array()) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<float> elems;
        for (auto&& jelem : jvalue->get_array()) {
          if (jelem.is_number()) {
            elems.push_back(jelem.get_number());
          } else {
            error = "value type mismatch, expected float";
          }
        }
        value = Value::MakeFloatArray(elems, time);
      } else if (*typeStr == "string[]") {
        if (!jvalue->is_array()) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<std::string> elems;
        for (auto&& jelem : jvalue->get_array()) {
          if (jelem.is_string()) {
            elems.push_back(jelem.get_string());
          } else {
            error = "value type mismatch, expected string";
          }
        }
        value = Value::MakeStringArray(std::move(elems), time);
      } else {
        // raw
        if (jvalue->is_string()) {
          std::vector<uint8_t> data;
          wpi::util::Base64Decode(jvalue->get_string(), &data);
          value = Value::MakeRaw(std::move(data), time);
        } else {
          error = "value type mismatch, expected string";
          goto err;
        }
      }

      // create persistent topic
      auto topic = CreateTopic(nullptr, *name, *typeStr, *props);

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
