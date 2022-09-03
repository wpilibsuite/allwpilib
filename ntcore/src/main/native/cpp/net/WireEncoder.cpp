// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WireEncoder.h"

#include <optional>

#include <wpi/json.h>
#include <wpi/mpack.h>
#include <wpi/raw_ostream.h>

#include "Handle.h"
#include "Message.h"
#include "PubSubOptions.h"
#include "networktables/NetworkTableValue.h"

using namespace nt;
using namespace nt::net;
using namespace mpack;

void nt::net::WireEncodePublish(wpi::raw_ostream& os, int64_t pubuid,
                                std::string_view name, std::string_view typeStr,
                                const wpi::json& properties) {
  wpi::json json = {{"method", PublishMsg::kMethodStr},
                    {"params",
                     {{"name", name},
                      {"properties", properties},
                      {"pubuid", pubuid},
                      {"type", typeStr}}}};
  os << json.dump();
}

void nt::net::WireEncodeUnpublish(wpi::raw_ostream& os, int64_t pubuid) {
  wpi::json json = {{"method", UnpublishMsg::kMethodStr},
                    {"params", {{"pubuid", pubuid}}}};
  os << json.dump();
}

void nt::net::WireEncodeSetProperties(wpi::raw_ostream& os,
                                      std::string_view name,
                                      const wpi::json& update) {
  wpi::json json = {{"method", SetPropertiesMsg::kMethodStr},
                    {"params", {{"name", name}, {"update", update}}}};
  os << json.dump();
}

template <typename T>
static void WireEncodeSubscribeImpl(wpi::raw_ostream& os, int64_t subuid,
                                    std::span<const T> topicNames,
                                    const PubSubOptionsImpl& options) {
  wpi::json json;
  json["method"] = SubscribeMsg::kMethodStr;
  bool hasOptions = false;
  if (options.sendAll) {
    json["params"]["options"]["all"] = true;
    hasOptions = true;
  }
  if (options.topicsOnly) {
    json["params"]["options"]["topicsonly"] = true;
    hasOptions = true;
  }
  if (options.prefixMatch) {
    json["params"]["options"]["prefix"] = true;
    hasOptions = true;
  }
  if (options.periodicMs != PubSubOptionsImpl::kDefaultPeriodicMs) {
    json["params"]["options"]["periodic"] = options.periodicMs / 1000.0;
    hasOptions = true;
  }
  if (!hasOptions) {
    json["params"]["options"] = wpi::json::object();
  }
  json["params"]["topics"] = topicNames;
  json["params"]["subuid"] = subuid;
  os << json.dump();
}

void nt::net::WireEncodeSubscribe(wpi::raw_ostream& os, int64_t subuid,
                                  std::span<const std::string_view> topicNames,
                                  const PubSubOptionsImpl& options) {
  WireEncodeSubscribeImpl(os, subuid, topicNames, options);
}

void nt::net::WireEncodeSubscribe(wpi::raw_ostream& os, int64_t subuid,
                                  std::span<const std::string> topicNames,
                                  const PubSubOptionsImpl& options) {
  WireEncodeSubscribeImpl(os, subuid, topicNames, options);
}

void nt::net::WireEncodeUnsubscribe(wpi::raw_ostream& os, int64_t subHandle) {
  wpi::json json = {{"method", UnsubscribeMsg::kMethodStr},
                    {"params", {{"subuid", subHandle}}}};
  os << json.dump();
}

bool nt::net::WireEncodeText(wpi::raw_ostream& os, const ClientMessage& msg) {
  if (auto m = std::get_if<PublishMsg>(&msg.contents)) {
    WireEncodePublish(os, Handle{m->pubHandle}.GetIndex(), m->name, m->typeStr,
                      m->properties);
  } else if (auto m = std::get_if<UnpublishMsg>(&msg.contents)) {
    WireEncodeUnpublish(os, Handle{m->pubHandle}.GetIndex());
  } else if (auto m = std::get_if<SetPropertiesMsg>(&msg.contents)) {
    WireEncodeSetProperties(os, m->name, m->update);
  } else if (auto m = std::get_if<SubscribeMsg>(&msg.contents)) {
    WireEncodeSubscribe(os, m->subHandle, m->topicNames, m->options);
  } else if (auto m = std::get_if<UnsubscribeMsg>(&msg.contents)) {
    WireEncodeUnsubscribe(os, m->subHandle);
  } else {
    return false;
  }
  return true;
}

void nt::net::WireEncodeAnnounce(wpi::raw_ostream& os, std::string_view name,
                                 int64_t id, std::string_view typeStr,
                                 const wpi::json& properties,
                                 std::optional<int64_t> pubHandle) {
  wpi::json json;
  json["method"] = AnnounceMsg::kMethodStr;
  json["params"]["id"] = id;
  json["params"]["name"] = name;
  json["params"]["properties"] = properties;
  if (pubHandle) {
    json["params"]["pubuid"] = *pubHandle;
  }
  json["params"]["type"] = typeStr;
  os << json.dump();
}

void nt::net::WireEncodeUnannounce(wpi::raw_ostream& os, std::string_view name,
                                   int64_t id) {
  wpi::json json = {{"method", UnannounceMsg::kMethodStr},
                    {"params", {{"id", id}, {"name", name}}}};
  os << json.dump();
}

void nt::net::WireEncodePropertiesUpdate(wpi::raw_ostream& os,
                                         std::string_view name,
                                         const wpi::json& update, bool ack) {
  wpi::json json;
  json["method"] = PropertiesUpdateMsg::kMethodStr;
  json["params"]["name"] = name;
  json["params"]["update"] = update;
  if (ack) {
    json["params"]["ack"] = true;
  }
  os << json.dump();
}

bool nt::net::WireEncodeText(wpi::raw_ostream& os, const ServerMessage& msg) {
  if (auto m = std::get_if<AnnounceMsg>(&msg.contents)) {
    WireEncodeAnnounce(os, m->name, m->id, m->typeStr, m->properties,
                       m->pubuid);
  } else if (auto m = std::get_if<UnannounceMsg>(&msg.contents)) {
    WireEncodeUnannounce(os, m->name, m->id);
  } else if (auto m = std::get_if<PropertiesUpdateMsg>(&msg.contents)) {
    WireEncodePropertiesUpdate(os, m->name, m->update, m->ack);
  } else {
    return false;
  }
  return true;
}

bool nt::net::WireEncodeBinary(wpi::raw_ostream& os, int64_t id, int64_t time,
                               const Value& value) {
  char buf[128];
  mpack_writer_t writer;
  mpack_writer_init(&writer, buf, sizeof(buf));
  mpack_writer_set_context(&writer, &os);
  mpack_writer_set_flush(
      &writer, [](mpack_writer_t* writer, const char* buffer, size_t count) {
        static_cast<wpi::raw_ostream*>(writer->context)->write(buffer, count);
      });
  mpack_start_array(&writer, 4);
  mpack_write_int(&writer, id);
  mpack_write_int(&writer, time);
  switch (value.type()) {
    case NT_BOOLEAN:
      mpack_write_u8(&writer, 0);
      mpack_write_bool(&writer, value.GetBoolean());
      break;
    case NT_INTEGER:
      mpack_write_u8(&writer, 2);
      mpack_write_int(&writer, value.GetInteger());
      break;
    case NT_FLOAT:
      mpack_write_u8(&writer, 3);
      mpack_write_float(&writer, value.GetFloat());
      break;
    case NT_DOUBLE:
      mpack_write_u8(&writer, 1);
      mpack_write_double(&writer, value.GetDouble());
      break;
    case NT_STRING: {
      auto v = value.GetString();
      mpack_write_u8(&writer, 4);
      mpack_write_str(&writer, v.data(), v.size());
      break;
    }
    case NT_RPC:
    case NT_RAW: {
      auto v = value.GetRaw();
      mpack_write_u8(&writer, 5);
      mpack_write_bin(&writer, reinterpret_cast<const char*>(v.data()),
                      v.size());
      break;
    }
    case NT_BOOLEAN_ARRAY: {
      auto v = value.GetBooleanArray();
      mpack_write_u8(&writer, 16);
      mpack_start_array(&writer, v.size());
      for (auto val : v) {
        mpack_write_bool(&writer, val);
      }
      mpack_finish_array(&writer);
      break;
    }
    case NT_INTEGER_ARRAY: {
      auto v = value.GetIntegerArray();
      mpack_write_u8(&writer, 18);
      mpack_start_array(&writer, v.size());
      for (auto val : v) {
        mpack_write_int(&writer, val);
      }
      mpack_finish_array(&writer);
      break;
    }
    case NT_FLOAT_ARRAY: {
      auto v = value.GetFloatArray();
      mpack_write_u8(&writer, 19);
      mpack_start_array(&writer, v.size());
      for (auto val : v) {
        mpack_write_float(&writer, val);
      }
      mpack_finish_array(&writer);
      break;
    }
    case NT_DOUBLE_ARRAY: {
      auto v = value.GetDoubleArray();
      mpack_write_u8(&writer, 17);
      mpack_start_array(&writer, v.size());
      for (auto val : v) {
        mpack_write_double(&writer, val);
      }
      mpack_finish_array(&writer);
      break;
    }
    case NT_STRING_ARRAY: {
      auto v = value.GetStringArray();
      mpack_write_u8(&writer, 20);
      mpack_start_array(&writer, v.size());
      for (auto&& val : v) {
        mpack_write_str(&writer, val.data(), val.size());
      }
      mpack_finish_array(&writer);
      break;
    }
    default:
      return false;
  }
  mpack_finish_array(&writer);
  return mpack_writer_destroy(&writer) == mpack_ok;
}
