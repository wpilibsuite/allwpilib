// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WireDecoder.h"

#include <algorithm>
#include <concepts>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/Logger.h>
#include <wpi/SpanExtras.h>
#include <wpi/json.h>
#include <wpi/mpack.h>

#include "Message.h"
#include "MessageHandler.h"

using namespace nt;
using namespace nt::net;
using namespace mpack;

static bool GetNumber(wpi::json& val, double* num) {
  if (auto v = val.get_ptr<const int64_t*>()) {
    *num = *v;
  } else if (auto v = val.get_ptr<const uint64_t*>()) {
    *num = *v;
  } else if (auto v = val.get_ptr<const double*>()) {
    *num = *v;
  } else {
    return false;
  }
  return true;
}

static bool GetNumber(wpi::json& val, int64_t* num) {
  if (auto v = val.get_ptr<const int64_t*>()) {
    *num = *v;
  } else if (auto v = val.get_ptr<const uint64_t*>()) {
    *num = *v;
  } else {
    return false;
  }
  return true;
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

static bool ObjGetNumber(wpi::json::object_t& obj, std::string_view key,
                         std::string* error, int64_t* num) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    *error = fmt::format("no {} key", key);
    return false;
  }
  if (!GetNumber(it->second, num)) {
    *error = fmt::format("{} must be a number", key);
    return false;
  }
  return true;
}

static bool ObjGetStringArray(wpi::json::object_t& obj, std::string_view key,
                              std::string* error,
                              std::vector<std::string>* out) {
  // prefixes
  auto it = obj.find(key);
  if (it == obj.end()) {
    *error = fmt::format("no {} key", key);
    return false;
  }
  auto jarr = it->second.get_ptr<wpi::json::array_t*>();
  if (!jarr) {
    *error = fmt::format("{} must be an array", key);
    return false;
  }
  out->resize(0);
  out->reserve(jarr->size());
  for (auto&& jval : *jarr) {
    auto str = jval.get_ptr<std::string*>();
    if (!str) {
      *error = fmt::format("{}/{} must be a string", key, out->size());
      return false;
    }
    out->emplace_back(std::move(*str));
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
                               wpi::Logger& logger) {
  wpi::json j;
  try {
    j = wpi::json::parse(in);
  } catch (wpi::json::parse_error& err) {
    WPI_WARNING(logger, "could not decode JSON message: {}", err.what());
    return false;
  }

  if (!j.is_array()) {
    WPI_WARNING(logger, "expected JSON array at top level");
    return false;
  }

  bool rv = false;
  int i = -1;
  for (auto&& jmsg : j) {
    ++i;
    std::string error;
    {
      auto obj = jmsg.get_ptr<wpi::json::object_t*>();
      if (!obj) {
        error = "expected message to be an object";
        goto err;
      }

      auto method = ObjGetString(*obj, "method", &error);
      if (!method) {
        goto err;
      }

      auto paramsIt = obj->find("params");
      if (paramsIt == obj->end()) {
        error = "no params key";
        goto err;
      }
      auto params = paramsIt->second.get_ptr<wpi::json::object_t*>();
      if (!params) {
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

          // properties; allow missing (treated as empty)
          wpi::json* properties = nullptr;
          auto propertiesIt = params->find("properties");
          if (propertiesIt != params->end()) {
            properties = &propertiesIt->second;
            if (!properties->is_object()) {
              error = "properties must be an object";
              goto err;
            }
          }
          wpi::json propertiesEmpty;
          if (!properties) {
            propertiesEmpty = wpi::json::object();
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
          auto updateIt = params->find("update");
          if (updateIt == params->end()) {
            error = "no update key";
            goto err;
          }
          auto update = &updateIt->second;
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

          // options
          PubSubOptionsImpl options;
          auto optionsIt = params->find("options");
          if (optionsIt != params->end()) {
            auto joptions = optionsIt->second.get_ptr<wpi::json::object_t*>();
            if (!joptions) {
              error = "options must be an object";
              goto err;
            }

            // periodic
            auto periodicIt = joptions->find("periodic");
            if (periodicIt != joptions->end()) {
              double val;
              if (!GetNumber(periodicIt->second, &val)) {
                error = "periodic value must be a number";
                goto err;
              }
              options.periodic = val;
              options.periodicMs = val * 1000;
            }

            // send all changes
            auto sendAllIt = joptions->find("all");
            if (sendAllIt != joptions->end()) {
              auto sendAll = sendAllIt->second.get_ptr<bool*>();
              if (!sendAll) {
                error = "all value must be a boolean";
                goto err;
              }
              options.sendAll = *sendAll;
            }

            // topics only
            auto topicsOnlyIt = joptions->find("topicsonly");
            if (topicsOnlyIt != joptions->end()) {
              auto topicsOnly = topicsOnlyIt->second.get_ptr<bool*>();
              if (!topicsOnly) {
                error = "topicsonly value must be a boolean";
                goto err;
              }
              options.topicsOnly = *topicsOnly;
            }

            // prefix match
            auto prefixMatchIt = joptions->find("prefix");
            if (prefixMatchIt != joptions->end()) {
              auto prefixMatch = prefixMatchIt->second.get_ptr<bool*>();
              if (!prefixMatch) {
                error = "prefix value must be a boolean";
                goto err;
              }
              options.prefixMatch = *prefixMatch;
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

          // type
          auto typeStr = ObjGetString(*params, "type", &error);
          if (!typeStr) {
            goto err;
          }

          // pubuid
          std::optional<int64_t> pubuid;
          auto pubuidIt = params->find("pubuid");
          if (pubuidIt != params->end()) {
            int64_t val;
            if (!GetNumber(pubuidIt->second, &val)) {
              error = "pubuid value must be a number";
              goto err;
            }
            pubuid = val;
          }

          // properties
          auto propertiesIt = params->find("properties");
          if (propertiesIt == params->end()) {
            error = "no properties key";
            goto err;
          }
          auto properties = &propertiesIt->second;
          if (!properties->is_object()) {
            WPI_WARNING(logger, "{}: properties is not an object", *name);
            *properties = wpi::json::object();
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

          // complete
          out.ServerUnannounce(*name, id);
        } else if (*method == PropertiesUpdateMsg::kMethodStr) {
          // name
          auto name = ObjGetString(*params, "name", &error);
          if (!name) {
            goto err;
          }

          // update
          auto updateIt = params->find("update");
          if (updateIt == params->end()) {
            error = "no update key";
            goto err;
          }
          auto update = &updateIt->second;
          if (!update->is_object()) {
            error = "update must be an object";
            goto err;
          }

          bool ack = false;
          auto ackIt = params->find("ack");
          if (ackIt != params->end()) {
            auto val = ackIt->second.get_ptr<bool*>();
            if (!val) {
              error = "ack must be a boolean";
              goto err;
            }
            ack = *val;
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

bool nt::net::WireDecodeText(std::string_view in, ClientMessageHandler& out,
                             wpi::Logger& logger) {
  return ::WireDecodeTextImpl(in, out, logger);
}

void nt::net::WireDecodeText(std::string_view in, ServerMessageHandler& out,
                             wpi::Logger& logger) {
  ::WireDecodeTextImpl(in, out, logger);
}

bool nt::net::WireDecodeBinary(std::span<const uint8_t>* in, int* outId,
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
  *in = wpi::take_back(*in, mpack_reader_remaining(&reader, nullptr));
  return true;
}
