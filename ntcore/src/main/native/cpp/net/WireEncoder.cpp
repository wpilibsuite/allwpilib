// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WireEncoder.h"

#include <optional>
#include <string>

#include <wpi/json.h>
#include <wpi/mpack.h>
#include <wpi/raw_ostream.h>

#include "Message.h"
#include "PubSubOptions.h"
#include "networktables/NetworkTableValue.h"

using namespace nt;
using namespace nt::net;
using namespace mpack;

void nt::net::WireEncodePublish(wpi::raw_ostream& os, int pubuid,
                                std::string_view name, std::string_view typeStr,
                                const wpi::json& properties) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << PublishMsg::kMethodStr << "\",\"params\":{";
  os << "\"name\":\"";
  s.dump_escaped(name, false);
  os << "\",\"properties\":";
  s.dump(properties, false, false, 0, 0);
  os << ",\"pubuid\":";
  s.dump_integer(pubuid);
  os << ",\"type\":\"";
  s.dump_escaped(typeStr, false);
  os << "\"}}";
}

void nt::net::WireEncodeUnpublish(wpi::raw_ostream& os, int pubuid) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << UnpublishMsg::kMethodStr << "\",\"params\":{";
  os << "\"pubuid\":";
  s.dump_integer(pubuid);
  os << "}}";
}

void nt::net::WireEncodeSetProperties(wpi::raw_ostream& os,
                                      std::string_view name,
                                      const wpi::json& update) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << SetPropertiesMsg::kMethodStr << "\",\"params\":{";
  os << "\"name\":\"";
  s.dump_escaped(name, false);
  os << "\",\"update\":";
  s.dump(update, false, false, 0, 0);
  os << "}}";
}

template <typename T>
static void EncodePrefixes(wpi::raw_ostream& os, std::span<const T> topicNames,
                           wpi::json::serializer& s) {
  os << '[';
  bool first = true;
  for (auto&& name : topicNames) {
    if (first) {
      first = false;
    } else {
      os << ',';
    }
    os << '"';
    s.dump_escaped(name, false);
    os << '"';
  }
  os << ']';
}

template <typename T>
static void WireEncodeSubscribeImpl(wpi::raw_ostream& os, int subuid,
                                    std::span<const T> topicNames,
                                    const PubSubOptionsImpl& options) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << SubscribeMsg::kMethodStr << "\",\"params\":{";
  os << "\"options\":{";
  bool first = true;
  if (options.sendAll) {
    os << "\"all\":true";
    first = false;
  }
  if (options.topicsOnly) {
    if (!first) {
      os << ',';
    }
    os << "\"topicsonly\":true";
    first = false;
  }
  if (options.prefixMatch) {
    if (!first) {
      os << ',';
    }
    os << "\"prefix\":true";
    first = false;
  }
  if (options.periodicMs != PubSubOptionsImpl::kDefaultPeriodicMs) {
    if (!first) {
      os << ',';
    }
    os << "\"periodic\":";
    s.dump_float(options.periodicMs / 1000.0);
  }
  os << "},\"topics\":";
  EncodePrefixes(os, topicNames, s);
  os << ",\"subuid\":";
  s.dump_integer(subuid);
  os << "}}";
}

void nt::net::WireEncodeSubscribe(wpi::raw_ostream& os, int subuid,
                                  std::span<const std::string_view> topicNames,
                                  const PubSubOptionsImpl& options) {
  WireEncodeSubscribeImpl(os, subuid, topicNames, options);
}

void nt::net::WireEncodeSubscribe(wpi::raw_ostream& os, int subuid,
                                  std::span<const std::string> topicNames,
                                  const PubSubOptionsImpl& options) {
  WireEncodeSubscribeImpl(os, subuid, topicNames, options);
}

void nt::net::WireEncodeUnsubscribe(wpi::raw_ostream& os, int subuid) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << UnsubscribeMsg::kMethodStr << "\",\"params\":{";
  os << "\"subuid\":";
  s.dump_integer(subuid);
  os << "}}";
}

bool nt::net::WireEncodeText(wpi::raw_ostream& os, const ClientMessage& msg) {
  if (auto m = std::get_if<PublishMsg>(&msg.contents)) {
    WireEncodePublish(os, m->pubuid, m->name, m->typeStr, m->properties);
  } else if (auto m = std::get_if<UnpublishMsg>(&msg.contents)) {
    WireEncodeUnpublish(os, m->pubuid);
  } else if (auto m = std::get_if<SetPropertiesMsg>(&msg.contents)) {
    WireEncodeSetProperties(os, m->name, m->update);
  } else if (auto m = std::get_if<SubscribeMsg>(&msg.contents)) {
    WireEncodeSubscribe(os, m->subuid, m->topicNames, m->options);
  } else if (auto m = std::get_if<UnsubscribeMsg>(&msg.contents)) {
    WireEncodeUnsubscribe(os, m->subuid);
  } else {
    return false;
  }
  return true;
}

void nt::net::WireEncodeAnnounce(wpi::raw_ostream& os, std::string_view name,
                                 int id, std::string_view typeStr,
                                 const wpi::json& properties,
                                 std::optional<int> pubuid) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << AnnounceMsg::kMethodStr << "\",\"params\":{";
  os << "\"id\":";
  s.dump_integer(id);
  os << ",\"name\":\"";
  s.dump_escaped(name, false);
  os << "\",\"properties\":";
  s.dump(properties, false, false, 0, 0);
  if (pubuid) {
    os << ",\"pubuid\":";
    s.dump_integer(*pubuid);
  }
  os << ",\"type\":\"";
  s.dump_escaped(typeStr, false);
  os << "\"}}";
}

void nt::net::WireEncodeUnannounce(wpi::raw_ostream& os, std::string_view name,
                                   int64_t id) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << UnannounceMsg::kMethodStr << "\",\"params\":{";
  os << "\"id\":";
  s.dump_integer(id);
  os << ",\"name\":\"";
  s.dump_escaped(name, false);
  os << "\"}}";
}

void nt::net::WireEncodePropertiesUpdate(wpi::raw_ostream& os,
                                         std::string_view name,
                                         const wpi::json& update, bool ack) {
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"method\":\"" << PropertiesUpdateMsg::kMethodStr
     << "\",\"params\":{";
  os << "\"name\":\"";
  s.dump_escaped(name, false);
  os << "\",\"update\":";
  s.dump(update, false, false, 0, 0);
  if (ack) {
    os << ",\"ack\":true";
  }
  os << "}}";
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

bool nt::net::WireEncodeBinary(wpi::raw_ostream& os, int id, int64_t time,
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
