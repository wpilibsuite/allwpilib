// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerStorage.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/Base64.h>
#include <wpi/MessagePack.h>
#include <wpi/json.h>

#include "Log.h"
#include "server/MessagePackWriter.h"
#include "server/ServerClient.h"

using namespace nt;
using namespace nt::server;
using namespace mpack;

ServerTopic* ServerStorage::CreateTopic(ServerClient* client,
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
  return CreateTopic(nullptr, name, "msgpack", {{"retained", true}}, true);
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

void ServerStorage::SetFlags(ServerClient* client, ServerTopic* topic,
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
  wpi::SmallVector<ServerTopic*, 16> toDelete;
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

void ServerStorage::DumpPersistent(wpi::raw_ostream& os) {
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

std::string ServerStorage::LoadPersistent(std::string_view in) {
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
