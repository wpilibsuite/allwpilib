// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WireDecoder.hpp"

#include <algorithm>
#include <concepts>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "Message.hpp"
#include "MessageHandler.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/SpanExtras.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mpack.h"

using namespace wpi::nt;
using namespace wpi::nt::net;
using namespace mpack;

static bool GetNumber(wpi::util::json& val, double* num) {
  if (val.is_number()) {
    *num = val.get_number();
  } else {
    return false;
  }
  return true;
}

static bool GetNumber(wpi::util::json& val, int64_t* num) {
  if (val.is_int()) {
    *num = val.get_int();
  } else {
    return false;
  }
  return true;
}

static std::string* ObjGetString(wpi::util::json& obj, std::string_view key,
                                 std::string* error) {
  auto val = obj.lookup(key);
  if (!val) {
    *error = fmt::format("no {} key", key);
    return nullptr;
  }
  if (!val->is_string()) {
    *error = fmt::format("{} must be a string", key);
    return nullptr;
  }
  return &val->get_string();
}

static bool ObjGetNumber(wpi::util::json& obj, std::string_view key,
                         std::string* error, int64_t* num) {
  auto val = obj.lookup(key);
  if (!val) {
    *error = fmt::format("no {} key", key);
    return false;
  }
  if (!GetNumber(*val, num)) {
    *error = fmt::format("{} must be a number", key);
    return false;
  }
  return true;
}

static bool ObjGetStringArray(wpi::util::json& obj, std::string_view key,
                              std::string* error,
                              std::vector<std::string>* out) {
  // prefixes
  auto val = obj.lookup(key);
  if (!val) {
    *error = fmt::format("no {} key", key);
    return false;
  }
  if (!val->is_array()) {
    *error = fmt::format("{} must be an array", key);
    return false;
  }
  auto& arr = val->get_array();
  out->resize(0);
  out->reserve(arr.size());
  for (auto&& jval : arr) {
    if (!jval.is_string()) {
      *error = fmt::format("{}/{} must be a string", key, out->size());
      return false;
    }
    out->emplace_back(jval.get_string());
  }
  return true;
}

// avoid a fmtlib "unused type alias 'char_type'" warning false positive
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif

template <typename T>
  requires(std::same_as<T, ClientMessageHandler> ||
           std::same_as<T, ServerMessageHandler>)
static bool WireDecodeTextImpl(std::string_view in, T& out,
                               wpi::util::Logger& logger) {
  auto j = wpi::util::json::parse(in);
  if (!j) {
    WPI_WARNING(logger, "could not decode JSON message: {}", j.error());
    return false;
  }

  if (!j->is_array()) {
    WPI_WARNING(logger, "expected JSON array at top level");
    return false;
  }

  bool rv = false;
  int i = -1;
  for (auto&& jmsg : j->get_array()) {
    ++i;
    std::string error;
    {
      if (!jmsg.is_object()) {
        error = "expected message to be an object";
        goto err;
      }

      auto method = ObjGetString(jmsg, "method", &error);
      if (!method) {
        goto err;
      }

      auto params = jmsg.lookup("params");
      if (!params) {
        error = "no params key";
        goto err;
      }
      if (!params->is_object()) {
        error = "params must be an object";
        goto err;
      }

      if constexpr (std::same_as<T, ClientMessageHandler>) {
        if (*method == PublishMsg::kMethodStr) {
          // name
          auto name = ObjGetString(*params, "name", &error);
          if (!name) {
            goto err;
          }

          // type
          auto typeStr = ObjGetString(*params, "type", &error);
          if (!typeStr) {
            goto err;
          }

          // pubuid
          int64_t pubuid;
          if (!ObjGetNumber(*params, "pubuid", &error, &pubuid)) {
            goto err;
          }

          // limit to 32-bit range and exclude endpoints used by
          // wpi::util::DenseMap
          if (pubuid >= 0x7fffffffLL || pubuid <= (-0x7fffffffLL - 1)) {
            error = "pubuid out of range";
            goto err;
          }

          // properties; allow missing (treated as empty)
          auto properties = params->lookup("properties");
          if (properties && !properties->is_object()) {
            error = "properties must be an object";
            goto err;
          }
          wpi::util::json propertiesEmpty;
          if (!properties) {
            propertiesEmpty = wpi::util::json::object();
            properties = &propertiesEmpty;
          }

          // complete
          out.ClientPublish(pubuid, *name, *typeStr, *properties, {});
          rv = true;
        } else if (*method == UnpublishMsg::kMethodStr) {
          // pubuid
          int64_t pubuid;
          if (!ObjGetNumber(*params, "pubuid", &error, &pubuid)) {
            goto err;
          }

          // limit to 32-bit range and exclude endpoints used by
          // wpi::util::DenseMap
          if (pubuid >= 0x7fffffffLL || pubuid <= (-0x7fffffffLL - 1)) {
            error = "pubuid out of range";
            goto err;
          }

          // complete
          out.ClientUnpublish(pubuid);
          rv = true;
        } else if (*method == SetPropertiesMsg::kMethodStr) {
          // name
          auto name = ObjGetString(*params, "name", &error);
          if (!name) {
            goto err;
          }

          // update
          auto update = params->lookup("update");
          if (!update) {
            error = "no update key";
            goto err;
          }
          if (!update->is_object()) {
            error = "update must be an object";
            goto err;
          }

          // complete
          out.ClientSetProperties(*name, *update);
        } else if (*method == SubscribeMsg::kMethodStr) {
          // subuid
          int64_t subuid;
          if (!ObjGetNumber(*params, "subuid", &error, &subuid)) {
            goto err;
          }

          // limit to 32-bit range and exclude endpoints used by
          // wpi::util::DenseMap
          if (subuid >= 0x7fffffffLL || subuid <= (-0x7fffffffLL - 1)) {
            error = "subuid out of range";
            goto err;
          }

          // options
          PubSubOptionsImpl options;
          if (auto joptions = params->lookup("options")) {
            if (!joptions->is_object()) {
              error = "options must be an object";
              goto err;
            }

            // periodic
            if (auto periodic = joptions->lookup("periodic")) {
              double val;
              if (!GetNumber(*periodic, &val)) {
                error = "periodic value must be a number";
                goto err;
              }
              options.periodic = val;
              options.periodicMs = val * 1000;
            }

            // send all changes
            if (auto sendAll = joptions->lookup("all")) {
              if (!sendAll->is_bool()) {
                error = "all value must be a boolean";
                goto err;
              }
              options.sendAll = sendAll->get_bool();
            }

            // topics only
            if (auto topicsOnly = joptions->lookup("topicsonly")) {
              if (!topicsOnly->is_bool()) {
                error = "topicsonly value must be a boolean";
                goto err;
              }
              options.topicsOnly = topicsOnly->get_bool();
            }

            // prefix match
            if (auto prefixMatch = joptions->lookup("prefix")) {
              if (!prefixMatch->is_bool()) {
                error = "prefix value must be a boolean";
                goto err;
              }
              options.prefixMatch = prefixMatch->get_bool();
            }
          }

          // topic names
          std::vector<std::string> topicNames;
          if (!ObjGetStringArray(*params, "topics", &error, &topicNames)) {
            goto err;
          }

          // complete
          out.ClientSubscribe(subuid, topicNames, options);
          rv = true;
        } else if (*method == UnsubscribeMsg::kMethodStr) {
          // subuid
          int64_t subuid;
          if (!ObjGetNumber(*params, "subuid", &error, &subuid)) {
            goto err;
          }

          // limit to 32-bit range and exclude endpoints used by
          // wpi::util::DenseMap
          if (subuid >= 0x7fffffffLL || subuid <= (-0x7fffffffLL - 1)) {
            error = "pubuid out of range";
            goto err;
          }

          // complete
          out.ClientUnsubscribe(subuid);
          rv = true;
        } else {
          error = fmt::format("unrecognized method '{}'", *method);
          goto err;
        }
      } else if constexpr (std::same_as<T, ServerMessageHandler>) {
        if (*method == AnnounceMsg::kMethodStr) {
          // name
          auto name = ObjGetString(*params, "name", &error);
          if (!name) {
            goto err;
          }

          // id
          int64_t id;
          if (!ObjGetNumber(*params, "id", &error, &id)) {
            goto err;
          }

          // limit to 32-bit range and exclude endpoints used by
          // wpi::util::DenseMap
          if (id >= 0x7fffffffLL || id <= (-0x7fffffffLL - 1)) {
            error = "id out of range";
            goto err;
          }

          // type
          auto typeStr = ObjGetString(*params, "type", &error);
          if (!typeStr) {
            goto err;
          }

          // pubuid
          std::optional<int64_t> pubuid;
          if (auto jpubuid = params->lookup("pubuid")) {
            int64_t val;
            if (!GetNumber(*jpubuid, &val)) {
              error = "pubuid value must be a number";
              goto err;
            }

            // limit to 32-bit range and exclude endpoints used by
            // wpi::util::DenseMap
            if (val >= 0x7fffffffLL || val <= (-0x7fffffffLL - 1)) {
              error = "pubuid out of range";
              goto err;
            }

            pubuid = val;
          }

          // properties
          auto properties = params->lookup("properties");
          if (!properties) {
            error = "no properties key";
            goto err;
          }
          if (!properties->is_object()) {
            WPI_WARNING(logger, "{}: properties is not an object", *name);
            *properties = wpi::util::json::object();
          }

          // complete
          out.ServerAnnounce(*name, id, *typeStr, *properties, pubuid);
        } else if (*method == UnannounceMsg::kMethodStr) {
          // name
          auto name = ObjGetString(*params, "name", &error);
          if (!name) {
            goto err;
          }

          // id
          int64_t id;
          if (!ObjGetNumber(*params, "id", &error, &id)) {
            goto err;
          }

          // limit to 32-bit range and exclude endpoints used by
          // wpi::util::DenseMap
          if (id >= 0x7fffffffLL || id <= (-0x7fffffffLL - 1)) {
            error = "id out of range";
            goto err;
          }

          // complete
          out.ServerUnannounce(*name, id);
        } else if (*method == PropertiesUpdateMsg::kMethodStr) {
          // name
          auto name = ObjGetString(*params, "name", &error);
          if (!name) {
            goto err;
          }

          // update
          auto update = params->lookup("update");
          if (!update) {
            error = "no update key";
            goto err;
          }
          if (!update->is_object()) {
            error = "update must be an object";
            goto err;
          }

          bool ack = false;
          if (auto jack = params->lookup("ack")) {
            if (!jack->is_bool()) {
              error = "ack must be a boolean";
              goto err;
            }
            ack = jack->get_bool();
          }

          // complete
          out.ServerPropertiesUpdate(*name, *update, ack);
        } else {
          error = fmt::format("unrecognized method '{}'", *method);
          goto err;
        }
      }
      continue;
    }
  err:
    WPI_WARNING(logger, "{}: {}", i, error);
  }

  return rv;
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

bool wpi::nt::net::WireDecodeText(std::string_view in,
                                  ClientMessageHandler& out,
                                  wpi::util::Logger& logger) {
  return ::WireDecodeTextImpl(in, out, logger);
}

void wpi::nt::net::WireDecodeText(std::string_view in,
                                  ServerMessageHandler& out,
                                  wpi::util::Logger& logger) {
  ::WireDecodeTextImpl(in, out, logger);
}

bool wpi::nt::net::WireDecodeBinary(std::span<const uint8_t>* in, int* outId,
                                    Value* outValue, std::string* error,
                                    int64_t localTimeOffset) {
  mpack_reader_t reader;
  mpack_reader_init_data(&reader, reinterpret_cast<const char*>(in->data()),
                         in->size());
  mpack_expect_array_match(&reader, 4);
  *outId = mpack_expect_int(&reader);
  auto time = mpack_expect_i64(&reader);
  int type = mpack_expect_int(&reader);
  switch (type) {
    case 0:  // boolean
      *outValue = Value::MakeBoolean(mpack_expect_bool(&reader), 1);
      break;
    case 2:  // integer
      *outValue = Value::MakeInteger(mpack_expect_i64(&reader), 1);
      break;
    case 3:  // float
      *outValue = Value::MakeFloat(mpack_expect_float(&reader), 1);
      break;
    case 1:  // double
      *outValue = Value::MakeDouble(mpack_expect_double(&reader), 1);
      break;
    case 4: {  // string
      auto length = mpack_expect_str(&reader);
      auto data = mpack_read_bytes_inplace(&reader, length);
      if (mpack_reader_error(&reader) == mpack_ok) {
        *outValue = Value::MakeString({data, length}, 1);
      }
      mpack_done_str(&reader);
      break;
    }
    case 5: {  // raw
      auto length = mpack_expect_bin(&reader);
      auto data = mpack_read_bytes_inplace(&reader, length);
      if (mpack_reader_error(&reader) == mpack_ok) {
        *outValue =
            Value::MakeRaw({reinterpret_cast<const uint8_t*>(data), length}, 1);
      }
      mpack_done_bin(&reader);
      break;
    }
    case 16: {  // boolean array
      auto length = mpack_expect_array(&reader);
      std::vector<int> arr;
      arr.reserve((std::min)(length, 1000u));
      for (uint32_t i = 0; i < length; ++i) {
        arr.emplace_back(mpack_expect_bool(&reader));
        if (mpack_reader_error(&reader) != mpack_ok) {
          break;
        }
      }
      if (mpack_reader_error(&reader) == mpack_ok) {
        *outValue = Value::MakeBooleanArray(std::move(arr), 1);
      }
      mpack_done_array(&reader);
      break;
    }
    case 18: {  // integer array
      auto length = mpack_expect_array(&reader);
      std::vector<int64_t> arr;
      arr.reserve((std::min)(length, 1000u));
      for (uint32_t i = 0; i < length; ++i) {
        arr.emplace_back(mpack_expect_i64(&reader));
        if (mpack_reader_error(&reader) != mpack_ok) {
          break;
        }
      }
      if (mpack_reader_error(&reader) == mpack_ok) {
        *outValue = Value::MakeIntegerArray(std::move(arr), 1);
      }
      mpack_done_array(&reader);
      break;
    }
    case 19: {  // float array
      auto length = mpack_expect_array(&reader);
      std::vector<float> arr;
      arr.reserve((std::min)(length, 1000u));
      for (uint32_t i = 0; i < length; ++i) {
        arr.emplace_back(mpack_expect_float(&reader));
        if (mpack_reader_error(&reader) != mpack_ok) {
          break;
        }
      }
      if (mpack_reader_error(&reader) == mpack_ok) {
        *outValue = Value::MakeFloatArray(std::move(arr), 1);
      }
      mpack_done_array(&reader);
      break;
    }
    case 17: {  // double array
      auto length = mpack_expect_array(&reader);
      std::vector<double> arr;
      arr.reserve((std::min)(length, 1000u));
      for (uint32_t i = 0; i < length; ++i) {
        arr.emplace_back(mpack_expect_double(&reader));
        if (mpack_reader_error(&reader) != mpack_ok) {
          break;
        }
      }
      if (mpack_reader_error(&reader) == mpack_ok) {
        *outValue = Value::MakeDoubleArray(std::move(arr), 1);
      }
      mpack_done_array(&reader);
      break;
    }
    case 20: {  // string array
      auto length = mpack_expect_array(&reader);
      std::vector<std::string> arr;
      arr.reserve((std::min)(length, 1000u));
      for (uint32_t i = 0; i < length; ++i) {
        auto length = mpack_expect_str(&reader);
        auto data = mpack_read_bytes_inplace(&reader, length);
        if (mpack_reader_error(&reader) == mpack_ok) {
          arr.emplace_back(std::string{data, length});
        } else {
          break;
        }
        mpack_done_str(&reader);
      }
      if (mpack_reader_error(&reader) == mpack_ok) {
        *outValue = Value::MakeStringArray(std::move(arr), 1);
      }
      mpack_done_array(&reader);
      break;
    }
    default:
      *error = fmt::format("unrecognized type {}", type);
      return false;
  }
  mpack_done_array(&reader);
  auto err = mpack_reader_destroy(&reader);
  if (err != mpack_ok) {
    *error = mpack_error_to_string(err);
    return false;
  }
  // set time
  outValue->SetServerTime(time);
  outValue->SetTime(time == 0 ? 0 : time + localTimeOffset);
  // update input range
  *in = wpi::util::take_back(*in, mpack_reader_remaining(&reader, nullptr));
  return true;
}
