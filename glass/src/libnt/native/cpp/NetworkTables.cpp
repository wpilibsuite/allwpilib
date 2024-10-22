// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NetworkTables.h"

#include <algorithm>
#include <cinttypes>
#include <concepts>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <ntcore_c.h>
#include <ntcore_cpp.h>
#include <ntcore_cpp_types.h>
#include <wpi/MessagePack.h>
#include <wpi/SmallString.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/mpack.h>
#include <wpi/print.h>
#include <wpi/raw_ostream.h>

#ifndef NO_PROTOBUF
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#endif

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"
#include "glass/support/ExtraGuiWidgets.h"

using namespace glass;
using namespace mpack;

namespace {
enum ShowCategory {
  ShowPersistent,
  ShowRetained,
  ShowTransitory,
  ShowAll,
};
}  // namespace

static bool IsVisible(ShowCategory category, bool persistent, bool retained) {
  switch (category) {
    case ShowPersistent:
      return persistent;
    case ShowRetained:
      return retained && !persistent;
    case ShowTransitory:
      return !retained && !persistent;
    case ShowAll:
      return true;
    default:
      return false;
  }
}

static std::string StringArrayToString(std::span<const std::string> in) {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  os << '[';
  bool first = true;
  for (auto&& v : in) {
    if (!first) {
      os << ',';
    }
    first = false;
    os << '"';
    os.write_escaped(v);
    os << '"';
  }
  os << ']';
  return rv;
}

NetworkTablesModel::NetworkTablesModel()
    : NetworkTablesModel{nt::NetworkTableInstance::GetDefault()} {}

NetworkTablesModel::NetworkTablesModel(nt::NetworkTableInstance inst)
    : m_inst{inst}, m_poller{inst} {
  m_poller.AddListener({{"", "$"}}, nt::EventFlags::kTopic |
                                        nt::EventFlags::kValueAll |
                                        nt::EventFlags::kImmediate);
}

NetworkTablesModel::Entry::~Entry() {
  if (publisher != 0) {
    nt::Unpublish(publisher);
  }
}

void NetworkTablesModel::Entry::UpdateInfo(nt::TopicInfo&& info_) {
  info = std::move(info_);
  properties = info.GetProperties();

  persistent = false;
  auto it = properties.find("persistent");
  if (it != properties.end()) {
    if (auto v = it->get_ptr<const bool*>()) {
      persistent = *v;
    }
  }

  retained = false;
  it = properties.find("retained");
  if (it != properties.end()) {
    if (auto v = it->get_ptr<const bool*>()) {
      retained = *v;
    }
  }
}

static void UpdateMsgpackValueSource(NetworkTablesModel& model,
                                     NetworkTablesModel::ValueSource* out,
                                     mpack_reader_t& r, std::string_view name,
                                     int64_t time) {
  mpack_tag_t tag = mpack_read_tag(&r);
  switch (mpack_tag_type(&tag)) {
    case mpack::mpack_type_bool:
      out->value = nt::Value::MakeBoolean(mpack_tag_bool_value(&tag), time);
      out->UpdateFromValue(model, name, "");
      break;
    case mpack::mpack_type_int:
      out->value = nt::Value::MakeInteger(mpack_tag_int_value(&tag), time);
      out->UpdateFromValue(model, name, "");
      break;
    case mpack::mpack_type_uint:
      out->value = nt::Value::MakeInteger(mpack_tag_uint_value(&tag), time);
      out->UpdateFromValue(model, name, "");
      break;
    case mpack::mpack_type_float:
      out->value = nt::Value::MakeFloat(mpack_tag_float_value(&tag), time);
      out->UpdateFromValue(model, name, "");
      break;
    case mpack::mpack_type_double:
      out->value = nt::Value::MakeDouble(mpack_tag_double_value(&tag), time);
      out->UpdateFromValue(model, name, "");
      break;
    case mpack::mpack_type_str: {
      std::string str;
      mpack_read_str(&r, &tag, &str);
      out->value = nt::Value::MakeString(std::move(str), time);
      out->UpdateFromValue(model, name, "");
      break;
    }
    case mpack::mpack_type_bin:
      // just skip it
      mpack_skip_bytes(&r, mpack_tag_bin_length(&tag));
      mpack_done_bin(&r);
      break;
    case mpack::mpack_type_array: {
      if (out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = false;
      }
      out->valueChildren.resize(mpack_tag_array_count(&tag));
      unsigned int i = 0;
      for (auto&& child : out->valueChildren) {
        if (child.name.empty()) {
          child.name = fmt::format("[{}]", i);
          child.path = fmt::format("{}{}", name, child.name);
        }
        ++i;
        UpdateMsgpackValueSource(model, &child, r, child.path,
                                 time);  // recurse
      }
      mpack_done_array(&r);
      break;
    }
    case mpack::mpack_type_map: {
      if (!out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = true;
      }
      wpi::StringMap<size_t> elems;
      for (size_t i = 0, size = out->valueChildren.size(); i < size; ++i) {
        elems[out->valueChildren[i].name] = i;
      }
      bool added = false;
      uint32_t count = mpack_tag_map_count(&tag);
      for (uint32_t i = 0; i < count; ++i) {
        std::string key;
        if (mpack_expect_str(&r, &key) == mpack_ok) {
          auto it = elems.find(key);
          if (it != elems.end()) {
            auto& child = out->valueChildren[it->second];
            UpdateMsgpackValueSource(model, &child, r, child.path, time);
            elems.erase(it);
          } else {
            added = true;
            out->valueChildren.emplace_back();
            auto& child = out->valueChildren.back();
            child.name = std::move(key);
            child.path = fmt::format("{}/{}", name, child.name);
            UpdateMsgpackValueSource(model, &child, r, child.path, time);
          }
        }
      }
      // erase unmatched keys
      out->valueChildren.erase(
          std::remove_if(
              out->valueChildren.begin(), out->valueChildren.end(),
              [&](const auto& child) { return elems.count(child.name) > 0; }),
          out->valueChildren.end());
      if (added) {
        // sort by name
        std::sort(out->valueChildren.begin(), out->valueChildren.end(),
                  [](const auto& a, const auto& b) { return a.name < b.name; });
      }
      mpack_done_map(&r);
      break;
    }
    default:
      out->value = {};
      mpack_done_type(&r, mpack_tag_type(&tag));
      break;
  }
}

static void UpdateStructValueSource(NetworkTablesModel& model,
                                    NetworkTablesModel::ValueSource* out,
                                    const wpi::DynamicStruct& s,
                                    std::string_view name, int64_t time) {
  auto desc = s.GetDescriptor();
  out->typeStr = "struct:" + desc->GetName();
  auto& fields = desc->GetFields();
  if (!out->valueChildrenMap || fields.size() != out->valueChildren.size()) {
    out->valueChildren.clear();
    out->valueChildrenMap = true;
    out->valueChildren.reserve(fields.size());
    for (auto&& field : fields) {
      out->valueChildren.emplace_back();
      auto& child = out->valueChildren.back();
      child.name = field.GetName();
      child.path = fmt::format("{}/{}", name, child.name);
    }
  }
  auto outIt = out->valueChildren.begin();
  for (auto&& field : fields) {
    auto& child = *outIt++;
    switch (field.GetType()) {
      case wpi::StructFieldType::kBool:
        if (field.IsArray()) {
          std::vector<int> v;
          v.reserve(field.GetArraySize());
          for (size_t i = 0; i < field.GetArraySize(); ++i) {
            v.emplace_back(s.GetBoolField(&field, i));
          }
          child.value = nt::Value::MakeBooleanArray(std::move(v), time);
        } else {
          child.value = nt::Value::MakeBoolean(s.GetBoolField(&field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case wpi::StructFieldType::kChar:
        child.value = nt::Value::MakeString(s.GetStringField(&field), time);
        child.UpdateFromValue(model, child.path, "");
        break;
      case wpi::StructFieldType::kInt8:
      case wpi::StructFieldType::kInt16:
      case wpi::StructFieldType::kInt32:
      case wpi::StructFieldType::kInt64:
      case wpi::StructFieldType::kUint8:
      case wpi::StructFieldType::kUint16:
      case wpi::StructFieldType::kUint32:
      case wpi::StructFieldType::kUint64: {
        bool isUint = field.IsUint();
        if (field.IsArray()) {
          std::vector<int64_t> v;
          v.reserve(field.GetArraySize());
          for (size_t i = 0; i < field.GetArraySize(); ++i) {
            if (isUint) {
              v.emplace_back(s.GetUintField(&field, i));
            } else {
              v.emplace_back(s.GetIntField(&field, i));
            }
          }
          child.value = nt::Value::MakeIntegerArray(std::move(v), time);
        } else {
          if (isUint) {
            child.value = nt::Value::MakeInteger(s.GetUintField(&field), time);
          } else {
            child.value = nt::Value::MakeInteger(s.GetIntField(&field), time);
          }
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      }
      case wpi::StructFieldType::kFloat:
        if (field.IsArray()) {
          std::vector<float> v;
          v.reserve(field.GetArraySize());
          for (size_t i = 0; i < field.GetArraySize(); ++i) {
            v.emplace_back(s.GetFloatField(&field, i));
          }
          child.value = nt::Value::MakeFloatArray(std::move(v), time);
        } else {
          child.value = nt::Value::MakeFloat(s.GetFloatField(&field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case wpi::StructFieldType::kDouble:
        if (field.IsArray()) {
          std::vector<double> v;
          v.reserve(field.GetArraySize());
          for (size_t i = 0; i < field.GetArraySize(); ++i) {
            v.emplace_back(s.GetDoubleField(&field, i));
          }
          child.value = nt::Value::MakeDoubleArray(std::move(v), time);
        } else {
          child.value = nt::Value::MakeDouble(s.GetDoubleField(&field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case wpi::StructFieldType::kStruct:
        if (field.IsArray()) {
          if (child.valueChildrenMap) {
            child.valueChildren.clear();
            child.valueChildrenMap = false;
          }
          child.valueChildren.resize(field.GetArraySize());
          unsigned int i = 0;
          for (auto&& child2 : child.valueChildren) {
            if (child2.name.empty()) {
              child2.name = fmt::format("[{}]", i);
              child2.path = fmt::format("{}{}", name, child.name);
            }
            UpdateStructValueSource(model, &child2, s.GetStructField(&field, i),
                                    child2.path, time);  // recurse
            ++i;
          }
        } else {
          UpdateStructValueSource(model, &child, s.GetStructField(&field),
                                  child.path, time);  // recurse
        }
        break;
    }
  }
}

#ifndef NO_PROTOBUF
static void UpdateProtobufValueSource(NetworkTablesModel& model,
                                      NetworkTablesModel::ValueSource* out,
                                      const google::protobuf::Message& msg,
                                      std::string_view name, int64_t time) {
  auto desc = msg.GetDescriptor();
  out->typeStr = "proto:" + desc->full_name();
  if (!out->valueChildrenMap ||
      desc->field_count() != static_cast<int>(out->valueChildren.size())) {
    out->valueChildren.clear();
    out->valueChildrenMap = true;
    out->valueChildren.reserve(desc->field_count());
    for (int i = 0, end = desc->field_count(); i < end; ++i) {
      out->valueChildren.emplace_back();
      auto& child = out->valueChildren.back();
      child.name = desc->field(i)->name();
      child.path = fmt::format("{}/{}", name, child.name);
    }
  }
  auto refl = msg.GetReflection();
  auto outIt = out->valueChildren.begin();
  for (int fieldNum = 0, end = desc->field_count(); fieldNum < end;
       ++fieldNum) {
    auto field = desc->field(fieldNum);
    auto& child = *outIt++;
    switch (field->cpp_type()) {
      case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<int> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedBool(msg, field, i));
          }
          child.value = nt::Value::MakeBooleanArray(std::move(v), time);
        } else {
          child.value = nt::Value::MakeBoolean(refl->GetBool(msg, field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<std::string> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedString(msg, field, i));
          }
          child.value = nt::Value::MakeStringArray(std::move(v), time);
        } else {
          child.value =
              nt::Value::MakeString(refl->GetString(msg, field), time);
          child.UpdateFromValue(model, child.path, "");
        }
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<int64_t> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedInt32(msg, field, i));
          }
          child.value = nt::Value::MakeIntegerArray(std::move(v), time);
        } else {
          child.value =
              nt::Value::MakeInteger(refl->GetInt32(msg, field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<int64_t> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedInt64(msg, field, i));
          }
          child.value = nt::Value::MakeIntegerArray(std::move(v), time);
        } else {
          child.value =
              nt::Value::MakeInteger(refl->GetInt64(msg, field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<int64_t> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedUInt32(msg, field, i));
          }
          child.value = nt::Value::MakeIntegerArray(std::move(v), time);
        } else {
          child.value =
              nt::Value::MakeInteger(refl->GetUInt32(msg, field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<int64_t> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedUInt64(msg, field, i));
          }
          child.value = nt::Value::MakeIntegerArray(std::move(v), time);
        } else {
          child.value =
              nt::Value::MakeInteger(refl->GetUInt64(msg, field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<float> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedFloat(msg, field, i));
          }
          child.value = nt::Value::MakeFloatArray(std::move(v), time);
        } else {
          child.value = nt::Value::MakeFloat(refl->GetFloat(msg, field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<double> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedDouble(msg, field, i));
          }
          child.value = nt::Value::MakeDoubleArray(std::move(v), time);
        } else {
          child.value =
              nt::Value::MakeDouble(refl->GetDouble(msg, field), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        if (field->is_repeated()) {
          size_t size = refl->FieldSize(msg, field);
          std::vector<std::string> v;
          v.reserve(size);
          for (size_t i = 0; i < size; ++i) {
            v.emplace_back(refl->GetRepeatedEnum(msg, field, i)->name());
          }
          child.value = nt::Value::MakeStringArray(std::move(v), time);
        } else {
          child.value =
              nt::Value::MakeString(refl->GetEnum(msg, field)->name(), time);
        }
        child.UpdateFromValue(model, child.path, "");
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        if (field->is_repeated()) {
          if (child.valueChildrenMap) {
            child.valueChildren.clear();
            child.valueChildrenMap = false;
          }
          size_t size = refl->FieldSize(msg, field);
          child.valueChildren.resize(size);
          unsigned int i = 0;
          for (auto&& child2 : child.valueChildren) {
            if (child2.name.empty()) {
              child2.name = fmt::format("[{}]", i);
              child2.path = fmt::format("{}{}", name, child.name);
            }
            UpdateProtobufValueSource(model, &child2,
                                      refl->GetRepeatedMessage(msg, field, i),
                                      child2.path, time);  // recurse
            ++i;
          }
        } else {
          UpdateProtobufValueSource(
              model, &child,
              refl->GetMessage(msg, field,
                               model.GetProtobufDatabase().GetMessageFactory()),
              child.path, time);  // recurse
        }
        break;
    }
  }
}
#endif

static void UpdateJsonValueSource(NetworkTablesModel& model,
                                  NetworkTablesModel::ValueSource* out,
                                  const wpi::json& j, std::string_view name,
                                  int64_t time) {
  switch (j.type()) {
    case wpi::json::value_t::object: {
      if (!out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = true;
      }
      wpi::StringMap<size_t> elems;
      for (size_t i = 0, size = out->valueChildren.size(); i < size; ++i) {
        elems[out->valueChildren[i].name] = i;
      }
      bool added = false;
      for (auto&& kv : j.items()) {
        auto it = elems.find(kv.key());
        if (it != elems.end()) {
          auto& child = out->valueChildren[it->second];
          UpdateJsonValueSource(model, &child, kv.value(), child.path, time);
          elems.erase(it);
        } else {
          added = true;
          out->valueChildren.emplace_back();
          auto& child = out->valueChildren.back();
          child.name = kv.key();
          child.path = fmt::format("{}/{}", name, child.name);
          UpdateJsonValueSource(model, &child, kv.value(), child.path, time);
        }
      }
      // erase unmatched keys
      out->valueChildren.erase(
          std::remove_if(
              out->valueChildren.begin(), out->valueChildren.end(),
              [&](const auto& child) { return elems.count(child.name) > 0; }),
          out->valueChildren.end());
      if (added) {
        // sort by name
        std::sort(out->valueChildren.begin(), out->valueChildren.end(),
                  [](const auto& a, const auto& b) { return a.name < b.name; });
      }
      break;
    }
    case wpi::json::value_t::array: {
      if (out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = false;
      }
      out->valueChildren.resize(j.size());
      unsigned int i = 0;
      for (auto&& child : out->valueChildren) {
        if (child.name.empty()) {
          child.name = fmt::format("[{}]", i);
          child.path = fmt::format("{}{}", name, child.name);
        }
        // recurse
        UpdateJsonValueSource(model, &child, j[i++], child.path, time);
      }
      break;
    }
    case wpi::json::value_t::string:
      out->value = nt::Value::MakeString(j.get_ref<const std::string&>(), time);
      out->UpdateFromValue(model, name, "");
      break;
    case wpi::json::value_t::boolean:
      out->value = nt::Value::MakeBoolean(j.get<bool>(), time);
      out->UpdateFromValue(model, name, "");
      break;
    case wpi::json::value_t::number_integer:
      out->value = nt::Value::MakeInteger(j.get<int64_t>(), time);
      out->UpdateFromValue(model, name, "");
      break;
    case wpi::json::value_t::number_unsigned:
      out->value = nt::Value::MakeInteger(j.get<uint64_t>(), time);
      out->UpdateFromValue(model, name, "");
      break;
    case wpi::json::value_t::number_float:
      out->value = nt::Value::MakeDouble(j.get<double>(), time);
      out->UpdateFromValue(model, name, "");
      break;
    default:
      out->value = {};
      break;
  }
}

void NetworkTablesModel::ValueSource::UpdateDiscreteSource(
    std::string_view name, double value, int64_t time, bool digital) {
  valueChildren.clear();
  if (!source) {
    source = std::make_unique<DataSource>(fmt::format("NT:{}", name));
  }
  source->SetValue(value, time);
  source->SetDigital(digital);
}

template <typename T, typename MakeValue>
void NetworkTablesModel::ValueSource::UpdateDiscreteArray(
    std::string_view name, std::span<const T> arr, int64_t time,
    MakeValue makeValue, bool digital) {
  if (valueChildrenMap) {
    valueChildren.clear();
    valueChildrenMap = false;
  }
  valueChildren.resize(arr.size());
  unsigned int i = 0;
  for (auto&& child : valueChildren) {
    if (child.name.empty()) {
      child.name = fmt::format("[{}]", i);
      child.path = fmt::format("{}{}", name, child.name);
    }
    child.value = makeValue(arr[i], time);
    child.UpdateDiscreteSource(child.path, arr[i], time, digital);
    ++i;
  }
}

void NetworkTablesModel::ValueSource::UpdateFromValue(
    NetworkTablesModel& model, std::string_view name,
    std::string_view typeStr) {
  switch (value.type()) {
    case NT_BOOLEAN:
      UpdateDiscreteSource(name, value.GetBoolean() ? 1 : 0, value.time(),
                           true);
      break;
    case NT_INTEGER:
      UpdateDiscreteSource(name, value.GetInteger(), value.time());
      break;
    case NT_FLOAT:
      UpdateDiscreteSource(name, value.GetFloat(), value.time());
      break;
    case NT_DOUBLE:
      UpdateDiscreteSource(name, value.GetDouble(), value.time());
      break;
    case NT_BOOLEAN_ARRAY:
      UpdateDiscreteArray(name, value.GetBooleanArray(), value.time(),
                          nt::Value::MakeBoolean, true);
      break;
    case NT_INTEGER_ARRAY:
      UpdateDiscreteArray(name, value.GetIntegerArray(), value.time(),
                          nt::Value::MakeInteger);
      break;
    case NT_FLOAT_ARRAY:
      UpdateDiscreteArray(name, value.GetFloatArray(), value.time(),
                          nt::Value::MakeFloat);
      break;
    case NT_DOUBLE_ARRAY:
      UpdateDiscreteArray(name, value.GetDoubleArray(), value.time(),
                          nt::Value::MakeDouble);
      break;
    case NT_STRING_ARRAY: {
      auto arr = value.GetStringArray();
      if (valueChildrenMap) {
        valueChildren.clear();
        valueChildrenMap = false;
      }
      valueChildren.resize(arr.size());
      unsigned int i = 0;
      for (auto&& child : valueChildren) {
        if (child.name.empty()) {
          child.name = fmt::format("[{}]", i);
          child.path = fmt::format("{}{}", name, child.name);
        }
        child.value = nt::Value::MakeString(arr[i++], value.time());
        child.UpdateFromValue(model, child.path, "");
      }
      break;
    }
    case NT_STRING:
      if (typeStr == "json") {
        try {
          UpdateJsonValueSource(model, this,
                                wpi::json::parse(value.GetString()), name,
                                value.last_change());
        } catch (wpi::json::exception&) {
          // ignore
        }
      } else {
        valueChildren.clear();
        valueStr.clear();
        wpi::raw_string_ostream os{valueStr};
        os << '"';
        os.write_escaped(value.GetString());
        os << '"';
      }
      break;
    case NT_RAW:
      if (typeStr == "msgpack") {
        mpack_reader_t r;
        mpack_reader_init_data(&r, value.GetRaw());
        UpdateMsgpackValueSource(model, this, r, name, value.last_change());
        mpack_reader_destroy(&r);
      } else if (auto structNameOpt = wpi::remove_prefix(typeStr, "struct:")) {
        auto structName = *structNameOpt;
        auto withoutArray = wpi::remove_suffix(structName, "[]");
        bool isArray = withoutArray.has_value();
        if (isArray) {
          structName = *withoutArray;
        }
        auto desc = model.m_structDb.Find(structName);
        if (desc && desc->IsValid() && desc->GetSize() != 0) {
          if (isArray) {
            // array of struct at top level
            if (valueChildrenMap) {
              valueChildren.clear();
              valueChildrenMap = false;
            }
            auto raw = value.GetRaw();
            valueChildren.resize(raw.size() / desc->GetSize());
            unsigned int i = 0;
            for (auto&& child : valueChildren) {
              if (child.name.empty()) {
                child.name = fmt::format("[{}]", i);
                child.path = fmt::format("{}{}", name, child.name);
              }
              wpi::DynamicStruct s{desc, raw};
              UpdateStructValueSource(model, &child, s, child.path,
                                      value.last_change());
              ++i;
              raw = wpi::drop_front(raw, desc->GetSize());
            }
          } else {
            wpi::DynamicStruct s{desc, value.GetRaw()};
            UpdateStructValueSource(model, this, s, name, value.last_change());
          }
        } else {
          valueChildren.clear();
        }
      } else if (auto filename = wpi::remove_prefix(typeStr, "proto:")) {
#ifndef NO_PROTOBUF
        auto msg = model.m_protoDb.Find(*filename);
        if (msg) {
          msg->Clear();
          auto raw = value.GetRaw();
          if (msg->ParseFromArray(raw.data(), raw.size())) {
            UpdateProtobufValueSource(model, this, *msg, name,
                                      value.last_change());
          } else {
            valueChildren.clear();
          }
        } else {
          valueChildren.clear();
        }
#else
        valueChildren.clear();
#endif
      } else {
        valueChildren.clear();
      }
      break;
    default:
      valueChildren.clear();
      break;
  }
}

void NetworkTablesModel::Update() {
  bool updateTree = false;
  for (auto&& event : m_poller.ReadQueue()) {
    if (auto info = event.GetTopicInfo()) {
      auto& entry = m_entries[info->topic];
      if (event.flags & nt::EventFlags::kPublish) {
        if (!entry) {
          entry = std::make_unique<Entry>();
          m_sortedEntries.emplace_back(entry.get());
          updateTree = true;
        }
      }
      if (event.flags & nt::EventFlags::kUnpublish) {
        // meta topic handling
        if (wpi::starts_with(info->name, '$')) {
          // meta topic handling
          if (info->name == "$clients") {
            m_clients.clear();
          } else if (info->name == "$serverpub") {
            m_server.publishers.clear();
          } else if (info->name == "$serversub") {
            m_server.subscribers.clear();
          } else if (auto client =
                         wpi::remove_prefix(info->name, "$clientpub$")) {
            auto it = m_clients.find(*client);
            if (it != m_clients.end()) {
              it->second.publishers.clear();
            }
          } else if (auto client =
                         wpi::remove_prefix(info->name, "$clientsub$")) {
            auto it = m_clients.find(*client);
            if (it != m_clients.end()) {
              it->second.subscribers.clear();
            }
          }
        }
        auto it = std::find(m_sortedEntries.begin(), m_sortedEntries.end(),
                            entry.get());
        // will be removed completely below
        if (it != m_sortedEntries.end()) {
          *it = nullptr;
        }
        m_entries.erase(info->topic);
        updateTree = true;
        continue;
      }
      if (event.flags & nt::EventFlags::kProperties) {
        updateTree = true;
      }
      if (entry) {
        entry->UpdateTopic(std::move(event));
      }
    } else if (auto valueData = event.GetValueEventData()) {
      auto& entry = m_entries[valueData->topic];
      if (entry) {
        entry->value = std::move(valueData->value);
        entry->UpdateFromValue(*this);
        if (wpi::starts_with(entry->info.name, '$') && entry->value.IsRaw() &&
            entry->info.type_str == "msgpack") {
          // meta topic handling
          if (entry->info.name == "$clients") {
            // need to remove deleted entries as UpdateClients() uses GetEntry()
            if (updateTree) {
              std::erase(m_sortedEntries, nullptr);
            }
            UpdateClients(entry->value.GetRaw());
          } else if (entry->info.name == "$serverpub") {
            m_server.UpdatePublishers(entry->value.GetRaw());
          } else if (entry->info.name == "$serversub") {
            m_server.UpdateSubscribers(entry->value.GetRaw());
          } else if (auto client =
                         wpi::remove_prefix(entry->info.name, "$clientpub$")) {
            auto it = m_clients.find(*client);
            if (it != m_clients.end()) {
              it->second.UpdatePublishers(entry->value.GetRaw());
            }
          } else if (auto client =
                         wpi::remove_prefix(entry->info.name, "$clientsub$")) {
            auto it = m_clients.find(*client);
            if (it != m_clients.end()) {
              it->second.UpdateSubscribers(entry->value.GetRaw());
            }
          }
        } else if (auto typeStr =
                       wpi::remove_prefix(entry->info.name, "/.schema/struct:");
                   entry->value.IsRaw() && typeStr &&
                   entry->info.type_str == "structschema") {
          // struct schema handling
          std::string_view schema{
              reinterpret_cast<const char*>(entry->value.GetRaw().data()),
              entry->value.GetRaw().size()};
          std::string err;
          auto desc = m_structDb.Add(*typeStr, schema, &err);
          if (!desc) {
            wpi::print("could not decode struct '{}' schema '{}': {}\n",
                       entry->info.name, schema, err);
          } else if (desc->IsValid()) {
            // loop over all entries with this type and update
            for (auto&& entryPair : m_entries) {
              if (!entryPair.second) {
                continue;
              }
              if (auto ts = wpi::remove_prefix(entryPair.second->info.type_str,
                                               "struct:")) {
                if (*ts == *typeStr ||
                    wpi::remove_suffix(*ts, "[]").value_or(*ts) == *typeStr) {
                  entryPair.second->UpdateFromValue(*this);
                }
              }
            }
          }
        } else if (auto filename =
                       wpi::remove_prefix(entry->info.name, "/.schema/proto:");
                   entry->value.IsRaw() && filename &&
                   entry->info.type_str == "proto:FileDescriptorProto") {
#ifndef NO_PROTOBUF
          // protobuf descriptor handling
          if (!m_protoDb.Add(*filename, entry->value.GetRaw())) {
            wpi::print("could not decode protobuf '{}' filename '{}'\n",
                       entry->info.name, *filename);
          } else {
            // loop over all protobuf entries and update (conservatively)
            for (auto&& entryPair : m_entries) {
              if (!entryPair.second) {
                continue;
              }
              std::string_view ts = entryPair.second->info.type_str;
              if (wpi::starts_with(ts, "proto:")) {
                entryPair.second->UpdateFromValue(*this);
              }
            }
          }
#endif
        }
      }
    }
  }

  // shortcut common case (updates)
  if (!updateTree) {
    return;
  }

  // remove deleted entries
  std::erase(m_sortedEntries, nullptr);

  RebuildTree();
}

void NetworkTablesModel::RebuildTree() {
  // sort by name
  std::sort(
      m_sortedEntries.begin(), m_sortedEntries.end(),
      [](const auto& a, const auto& b) { return a->info.name < b->info.name; });

  RebuildTreeImpl(&m_root, ShowAll);
  RebuildTreeImpl(&m_persistentRoot, ShowPersistent);
  RebuildTreeImpl(&m_retainedRoot, ShowRetained);
  RebuildTreeImpl(&m_transitoryRoot, ShowTransitory);
}

void NetworkTablesModel::RebuildTreeImpl(std::vector<TreeNode>* tree,
                                         int category) {
  tree->clear();
  wpi::SmallVector<std::string_view, 16> parts;
  for (auto& entry : m_sortedEntries) {
    if (!IsVisible(static_cast<ShowCategory>(category), entry->persistent,
                   entry->retained)) {
      continue;
    }
    parts.clear();
    wpi::split(entry->info.name, parts, '/', -1, false);

    // ignore a raw "/" key
    if (parts.empty()) {
      continue;
    }

    // get to leaf
    auto nodes = tree;
    for (auto part : wpi::drop_back(std::span{parts.begin(), parts.end()})) {
      auto it =
          std::find_if(nodes->begin(), nodes->end(),
                       [&](const auto& node) { return node.name == part; });
      if (it == nodes->end()) {
        nodes->emplace_back(part);
        // path is from the beginning of the string to the end of the current
        // part; this works because part is a reference to the internals of
        // entry->info.name
        nodes->back().path.assign(
            entry->info.name.data(),
            part.data() + part.size() - entry->info.name.data());
        it = nodes->end() - 1;
      }
      nodes = &it->children;
    }

    auto it = std::find_if(nodes->begin(), nodes->end(), [&](const auto& node) {
      return node.name == parts.back();
    });
    if (it == nodes->end()) {
      nodes->emplace_back(parts.back());
      // no need to set path, as it's identical to entry->name
      it = nodes->end() - 1;
    }
    it->entry = entry;
  }
}

bool NetworkTablesModel::Exists() {
  return true;
}

NetworkTablesModel::Entry* NetworkTablesModel::GetEntry(std::string_view name) {
  auto entryIt = std::lower_bound(
      m_sortedEntries.begin(), m_sortedEntries.end(), name,
      [](auto&& entry, auto&& name) { return entry->info.name < name; });
  if (entryIt == m_sortedEntries.end() || (*entryIt)->info.name != name) {
    return nullptr;
  }
  return *entryIt;
}

NetworkTablesModel::Entry* NetworkTablesModel::AddEntry(NT_Topic topic) {
  auto& entry = m_entries[topic];
  if (!entry) {
    entry = std::make_unique<Entry>();
    entry->info = nt::GetTopicInfo(topic);
    entry->properties = entry->info.GetProperties();
    m_sortedEntries.emplace_back(entry.get());
  }
  RebuildTree();
  return entry.get();
}

NetworkTablesModel::Client::Subscriber::Subscriber(
    nt::meta::ClientSubscriber&& oth)
    : ClientSubscriber{std::move(oth)},
      topicsStr{StringArrayToString(topics)} {}

void NetworkTablesModel::Client::UpdatePublishers(
    std::span<const uint8_t> data) {
  if (auto pubs = nt::meta::DecodeClientPublishers(data)) {
    publishers = std::move(*pubs);
  } else {
    wpi::print(stderr, "Failed to update publishers\n");
  }
}

void NetworkTablesModel::Client::UpdateSubscribers(
    std::span<const uint8_t> data) {
  if (auto subs = nt::meta::DecodeClientSubscribers(data)) {
    subscribers.clear();
    subscribers.reserve(subs->size());
    for (auto&& sub : *subs) {
      subscribers.emplace_back(std::move(sub));
    }
  } else {
    wpi::print(stderr, "Failed to update subscribers\n");
  }
}

void NetworkTablesModel::UpdateClients(std::span<const uint8_t> data) {
  auto clientsArr = nt::meta::DecodeClients(data);
  if (!clientsArr) {
    return;
  }

  // we need to create a new map so deletions are reflected
  std::map<std::string, Client, std::less<>> newClients;
  for (auto&& client : *clientsArr) {
    auto& newClient = newClients[client.id];
    newClient = std::move(client);
    auto it = m_clients.find(newClient.id);
    if (it != m_clients.end()) {
      // transfer from existing
      newClient.publishers = std::move(it->second.publishers);
      newClient.subscribers = std::move(it->second.subscribers);
    } else {
      // initially populate
      if (Entry* entry = GetEntry(fmt::format("$clientpub${}", newClient.id))) {
        if (entry->value.IsRaw() && entry->info.type_str == "msgpack") {
          newClient.UpdatePublishers(entry->value.GetRaw());
        }
      }
      if (Entry* entry = GetEntry(fmt::format("$clientsub${}", newClient.id))) {
        if (entry->value.IsRaw() && entry->info.type_str == "msgpack") {
          newClient.UpdateSubscribers(entry->value.GetRaw());
        }
      }
    }
  }

  // replace map
  m_clients = std::move(newClients);
}

static bool GetHeadingTypeString(std::string_view* ts) {
  if (auto withoutProto = wpi::remove_prefix(*ts, "proto:")) {
    *ts = *withoutProto;
    auto lastdot = ts->rfind('.');
    if (lastdot != std::string_view::npos) {
      *ts = wpi::substr(*ts, lastdot + 1);
    }
    if (auto withoutProtobuf = wpi::remove_prefix(*ts, "Protobuf")) {
      *ts = *withoutProtobuf;
    }
    return true;
  } else if (auto withoutStruct = wpi::remove_prefix(*ts, "struct:")) {
    *ts = *withoutStruct;
    return true;
  }
  return false;
}

static const char* GetShortTypeString(std::string_view ts) {
  if (wpi::starts_with(ts, "proto:")) {
    return "protobuf";
  } else if (wpi::starts_with(ts, "struct:")) {
    return "struct";
  } else {
    return ts.data();
  }
}

static const char* GetTypeString(NT_Type type, const char* overrideTypeStr) {
  if (overrideTypeStr) {
    return GetShortTypeString(overrideTypeStr);
  }
  switch (type) {
    case NT_BOOLEAN:
      return "boolean";
    case NT_INTEGER:
      return "int";
    case NT_FLOAT:
      return "float";
    case NT_DOUBLE:
      return "double";
    case NT_STRING:
      return "string";
    case NT_BOOLEAN_ARRAY:
      return "boolean[]";
    case NT_INTEGER_ARRAY:
      return "int[]";
    case NT_FLOAT_ARRAY:
      return "float[]";
    case NT_DOUBLE_ARRAY:
      return "double[]";
    case NT_STRING_ARRAY:
      return "string[]";
    case NT_RAW:
      return "raw";
    case NT_RPC:
      return "rpc";
    default:
      return "other";
  }
}

static void EmitEntryValueReadonly(const NetworkTablesModel::ValueSource& entry,
                                   const char* overrideTypeStr,
                                   NetworkTablesFlags flags) {
  auto& val = entry.value;
  if (!val) {
    return;
  }

  const char* typeStr = GetTypeString(val.type(), overrideTypeStr);
  ImGui::SetNextItemWidth(
      -1 * (ImGui::CalcTextSize(typeStr).x + ImGui::GetStyle().FramePadding.x));

  switch (val.type()) {
    case NT_BOOLEAN:
      ImGui::LabelText(typeStr, "%s", val.GetBoolean() ? "true" : "false");
      break;
    case NT_INTEGER:
      ImGui::LabelText(typeStr, "%" PRId64, val.GetInteger());
      break;
    case NT_FLOAT:
      ImGui::LabelText(typeStr, "%.6f", val.GetFloat());
      break;
    case NT_DOUBLE: {
      unsigned char precision = (flags & NetworkTablesFlags_Precision) >>
                                kNetworkTablesFlags_PrecisionBitShift;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
      ImGui::LabelText(typeStr, fmt::format("%.{}f", precision).c_str(),
                       val.GetDouble());
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      break;
    }
    case NT_STRING: {
      ImGui::LabelText(typeStr, "%s", entry.valueStr.c_str());
      break;
    }
    case NT_BOOLEAN_ARRAY:
    case NT_INTEGER_ARRAY:
    case NT_FLOAT_ARRAY:
    case NT_DOUBLE_ARRAY:
    case NT_STRING_ARRAY:
      ImGui::LabelText(typeStr, "[]");
      break;
    case NT_RAW: {
      ImGui::LabelText(typeStr, val.GetRaw().empty() ? "[]" : "[...]");
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        if (overrideTypeStr) {
          ImGui::TextUnformatted(overrideTypeStr);
        }
        ImGui::Text("%u bytes", static_cast<unsigned int>(val.GetRaw().size()));
        ImGui::EndTooltip();
      }
      break;
    }
    default:
      ImGui::LabelText(typeStr, "?");
      break;
  }
}

static constexpr size_t kTextBufferSize = 4096;

static char* GetTextBuffer(std::string_view in) {
  static char textBuffer[kTextBufferSize];
  size_t len = (std::min)(in.size(), kTextBufferSize - 1);
  std::memcpy(textBuffer, in.data(), len);
  textBuffer[len] = '\0';
  return textBuffer;
}

namespace {
class ArrayEditor {
 public:
  virtual ~ArrayEditor() = default;
  virtual bool Emit() = 0;
};

template <int NTType, typename T>
class ArrayEditorImpl final : public ArrayEditor {
 public:
  ArrayEditorImpl(NetworkTablesModel& model, std::string name,
                  NetworkTablesFlags flags, std::span<const T> value)
      : m_model{model},
        m_name{std::move(name)},
        m_flags{flags},
        m_arr{value.begin(), value.end()} {}

  bool Emit() final;

 private:
  NetworkTablesModel& m_model;
  std::string m_name;
  NetworkTablesFlags m_flags;
  std::vector<T> m_arr;
};

template <int NTType, typename T>
bool ArrayEditorImpl<NTType, T>::Emit() {
  if (ImGui::BeginTable(
          "arrayvalues", 1,
          ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit |
              ImGuiTableFlags_RowBg,
          ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 16))) {
    ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
    int toAdd = -1;
    int toRemove = -1;
    ImGuiListClipper clipper;
    clipper.Begin(m_arr.size());
    while (clipper.Step()) {
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::PushID(row);
        char label[16];
        wpi::format_to_n_c_str(label, sizeof(label), "[{}]", row);
        if constexpr (NTType == NT_BOOLEAN_ARRAY) {
          static const char* boolOptions[] = {"false", "true"};
          ImGui::Combo(label, &m_arr[row], boolOptions, 2);
        } else if constexpr (NTType == NT_FLOAT_ARRAY) {
          ImGui::InputFloat(label, &m_arr[row], 0, 0, "%.6f");
        } else if constexpr (NTType == NT_DOUBLE_ARRAY) {
          unsigned char precision = (m_flags & NetworkTablesFlags_Precision) >>
                                    kNetworkTablesFlags_PrecisionBitShift;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
          ImGui::InputDouble(label, &m_arr[row], 0, 0,
                             fmt::format("%.{}f", precision).c_str());
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
        } else if constexpr (NTType == NT_INTEGER_ARRAY) {
          ImGui::InputScalar(label, ImGuiDataType_S64, &m_arr[row]);
        } else if constexpr (NTType == NT_STRING_ARRAY) {
          ImGui::InputText(label, &m_arr[row]);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+")) {
          toAdd = row;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("-")) {
          toRemove = row;
        }
        ImGui::PopID();
      }
    }
    if (toAdd != -1) {
      m_arr.emplace(m_arr.begin() + toAdd);
    } else if (toRemove != -1) {
      m_arr.erase(m_arr.begin() + toRemove);
    }
    ImGui::EndTable();
  }
  if (ImGui::Button("Add to end")) {
    m_arr.emplace_back();
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel")) {
    return true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Apply")) {
    auto* entry = m_model.GetEntry(m_name);
    if (!entry) {
      entry = m_model.AddEntry(
          nt::GetTopic(m_model.GetInstance().GetHandle(), m_name));
    }
    if constexpr (NTType == NT_BOOLEAN_ARRAY) {
      if (entry->publisher == 0) {
        entry->publisher =
            nt::Publish(entry->info.topic, NT_BOOLEAN_ARRAY, "boolean[]");
      }
      nt::SetBooleanArray(entry->publisher, m_arr);
    } else if constexpr (NTType == NT_FLOAT_ARRAY) {
      if (entry->publisher == 0) {
        entry->publisher =
            nt::Publish(entry->info.topic, NT_FLOAT_ARRAY, "float[]");
      }
      nt::SetFloatArray(entry->publisher, m_arr);
    } else if constexpr (NTType == NT_DOUBLE_ARRAY) {
      if (entry->publisher == 0) {
        entry->publisher =
            nt::Publish(entry->info.topic, NT_DOUBLE_ARRAY, "double[]");
      }
      nt::SetDoubleArray(entry->publisher, m_arr);
    } else if constexpr (NTType == NT_INTEGER_ARRAY) {
      if (entry->publisher == 0) {
        entry->publisher =
            nt::Publish(entry->info.topic, NT_INTEGER_ARRAY, "int[]");
      }
      nt::SetIntegerArray(entry->publisher, m_arr);
    } else if constexpr (NTType == NT_STRING_ARRAY) {
      if (entry->publisher == 0) {
        entry->publisher =
            nt::Publish(entry->info.topic, NT_STRING_ARRAY, "string[]");
      }
      nt::SetStringArray(entry->publisher, m_arr);
    }
    return true;
  }
  return false;
}
}  // namespace

static ImGuiID gArrayEditorID;
static std::unique_ptr<ArrayEditor> gArrayEditor;

static void EmitEntryValueEditable(NetworkTablesModel* model,
                                   NetworkTablesModel::Entry& entry,
                                   NetworkTablesFlags flags) {
  auto& val = entry.value;
  if (!val) {
    return;
  }

  const char* typeStr = GetTypeString(
      val.type(),
      entry.info.type_str.empty() ? nullptr : entry.info.type_str.c_str());
  ImGui::SetNextItemWidth(
      -1 * (ImGui::CalcTextSize(typeStr).x + ImGui::GetStyle().FramePadding.x));

  ImGui::PushID(entry.info.name.c_str());
  switch (val.type()) {
    case NT_BOOLEAN: {
      static const char* boolOptions[] = {"false", "true"};
      int v = val.GetBoolean() ? 1 : 0;
      if (ImGui::Combo(typeStr, &v, boolOptions, 2)) {
        if (entry.publisher == 0) {
          entry.publisher =
              nt::Publish(entry.info.topic, NT_BOOLEAN, "boolean");
        }
        nt::SetBoolean(entry.publisher, v);
      }
      break;
    }
    case NT_INTEGER: {
      int64_t v = val.GetInteger();
      if (InputExpr<int64_t>(typeStr, &v, "%d",
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (entry.publisher == 0) {
          entry.publisher = nt::Publish(entry.info.topic, NT_INTEGER, "int");
        }
        nt::SetInteger(entry.publisher, v);
      }
      break;
    }
    case NT_FLOAT: {
      float v = val.GetFloat();
      if (InputExpr<float>(typeStr, &v, "%.6f",
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (entry.publisher == 0) {
          entry.publisher = nt::Publish(entry.info.topic, NT_FLOAT, "float");
        }
        nt::SetFloat(entry.publisher, v);
      }
      break;
    }
    case NT_DOUBLE: {
      double v = val.GetDouble();
      unsigned char precision = (flags & NetworkTablesFlags_Precision) >>
                                kNetworkTablesFlags_PrecisionBitShift;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
      if (InputExpr<double>(typeStr, &v,
                            fmt::format("%.{}f", precision).c_str(),
                            ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (entry.publisher == 0) {
          entry.publisher = nt::Publish(entry.info.topic, NT_DOUBLE, "double");
        }
        nt::SetDouble(entry.publisher, v);
      }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      break;
    }
    case NT_STRING: {
      char* v = GetTextBuffer(entry.valueStr);
      ImGui::InputText(typeStr, v, kTextBufferSize,
                       ImGuiInputTextFlags_EnterReturnsTrue);
      if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (v[0] == '"') {
          if (entry.publisher == 0) {
            entry.publisher =
                nt::Publish(entry.info.topic, NT_STRING, "string");
          }
          wpi::SmallString<128> buf;
          nt::SetString(entry.publisher,
                        wpi::UnescapeCString(v + 1, buf).first);
        }
      }
      break;
    }
    case NT_BOOLEAN_ARRAY:
      ImGui::LabelText(typeStr, "[]");
      if (ImGui::BeginPopupContextItem("boolean[]")) {
        if (ImGui::Selectable("Edit Array")) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_BOOLEAN_ARRAY, int>>(
                  *model, entry.info.name, flags,
                  entry.value.GetBooleanArray());
          ImGui::OpenPopup(gArrayEditorID);
        }
        ImGui::EndPopup();
      }
      break;
    case NT_INTEGER_ARRAY:
      ImGui::LabelText(typeStr, "[]");
      if (ImGui::BeginPopupContextItem("int[]")) {
        if (ImGui::Selectable("Edit Array")) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_INTEGER_ARRAY, int64_t>>(
                  *model, entry.info.name, flags,
                  entry.value.GetIntegerArray());
          ImGui::OpenPopup(gArrayEditorID);
        }
        ImGui::EndPopup();
      }
      break;
    case NT_FLOAT_ARRAY:
      ImGui::LabelText(typeStr, "[]");
      if (ImGui::BeginPopupContextItem("float[]")) {
        if (ImGui::Selectable("Edit Array")) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_FLOAT_ARRAY, float>>(
                  *model, entry.info.name, flags, entry.value.GetFloatArray());
          ImGui::OpenPopup(gArrayEditorID);
        }
        ImGui::EndPopup();
      }
      break;
    case NT_DOUBLE_ARRAY:
      ImGui::LabelText(typeStr, "[]");
      if (ImGui::BeginPopupContextItem("double[]")) {
        if (ImGui::Selectable("Edit Array")) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_DOUBLE_ARRAY, double>>(
                  *model, entry.info.name, flags, entry.value.GetDoubleArray());
          ImGui::OpenPopup(gArrayEditorID);
        }
        ImGui::EndPopup();
      }
      break;
    case NT_STRING_ARRAY:
      ImGui::LabelText(typeStr, "[]");
      if (ImGui::BeginPopupContextItem("string[]")) {
        if (ImGui::Selectable("Edit Array")) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_STRING_ARRAY, std::string>>(
                  *model, entry.info.name, flags, entry.value.GetStringArray());
          ImGui::OpenPopup(gArrayEditorID);
        }
        ImGui::EndPopup();
        break;
      }
      break;
    case NT_RAW: {
      ImGui::LabelText(typeStr, val.GetRaw().empty() ? "[]" : "[...]");
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        if (!entry.info.type_str.empty()) {
          ImGui::TextUnformatted(entry.info.type_str.c_str());
        }
        ImGui::Text("%u bytes", static_cast<unsigned int>(val.GetRaw().size()));
        ImGui::EndTooltip();
      }
      break;
    }
    case NT_RPC:
      ImGui::LabelText(typeStr, "[...]");
      break;
    default:
      ImGui::LabelText(typeStr, "?");
      break;
  }
  ImGui::PopID();
}

static void CreateTopicMenuItem(NetworkTablesModel* model,
                                std::string_view path, NT_Type type,
                                const char* typeStr, bool enabled) {
  if (ImGui::MenuItem(typeStr, nullptr, false, enabled)) {
    auto entry =
        model->AddEntry(nt::GetTopic(model->GetInstance().GetHandle(), path));
    if (entry->publisher == 0) {
      entry->publisher = nt::Publish(entry->info.topic, type, typeStr);
      // publish a default value so it's editable
      switch (type) {
        case NT_BOOLEAN:
          nt::SetDefaultBoolean(entry->publisher, false);
          break;
        case NT_INTEGER:
          nt::SetDefaultInteger(entry->publisher, 0);
          break;
        case NT_FLOAT:
          nt::SetDefaultFloat(entry->publisher, 0.0);
          break;
        case NT_DOUBLE:
          nt::SetDefaultDouble(entry->publisher, 0.0);
          break;
        case NT_STRING:
          nt::SetDefaultString(entry->publisher, "");
          break;
        case NT_BOOLEAN_ARRAY:
          nt::SetDefaultBooleanArray(entry->publisher, {});
          break;
        case NT_INTEGER_ARRAY:
          nt::SetDefaultIntegerArray(entry->publisher, {});
          break;
        case NT_FLOAT_ARRAY:
          nt::SetDefaultFloatArray(entry->publisher, {});
          break;
        case NT_DOUBLE_ARRAY:
          nt::SetDefaultDoubleArray(entry->publisher, {});
          break;
        case NT_STRING_ARRAY:
          nt::SetDefaultStringArray(entry->publisher, {});
          break;
        default:
          break;
      }
    }
  }
}

void glass::DisplayNetworkTablesAddMenu(NetworkTablesModel* model,
                                        std::string_view path,
                                        NetworkTablesFlags flags) {
  static char nameBuffer[kTextBufferSize];

  if (ImGui::BeginMenu("Add new...")) {
    if (ImGui::IsWindowAppearing()) {
      nameBuffer[0] = '\0';
    }

    ImGui::InputTextWithHint("New item name", "example", nameBuffer,
                             kTextBufferSize);
    std::string fullNewPath;
    if (path == "/") {
      path = "";
    }
    fullNewPath = fmt::format("{}/{}", path, nameBuffer);

    ImGui::Text("Adding: %s", fullNewPath.c_str());
    ImGui::Separator();
    auto entry = model->GetEntry(fullNewPath);
    bool exists = entry && entry->info.type != NT_Type::NT_UNASSIGNED;
    bool enabled = (flags & NetworkTablesFlags_CreateNoncanonicalKeys ||
                    nameBuffer[0] != '\0') &&
                   !exists;

    CreateTopicMenuItem(model, fullNewPath, NT_STRING, "string", enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_INTEGER, "int", enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_FLOAT, "float", enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_DOUBLE, "double", enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_BOOLEAN, "boolean", enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_STRING_ARRAY, "string[]",
                        enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_INTEGER_ARRAY, "int[]", enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_FLOAT_ARRAY, "float[]", enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_DOUBLE_ARRAY, "double[]",
                        enabled);
    CreateTopicMenuItem(model, fullNewPath, NT_BOOLEAN_ARRAY, "boolean[]",
                        enabled);

    ImGui::EndMenu();
  }
}

static void EmitParentContextMenu(NetworkTablesModel* model,
                                  const std::string& path,
                                  NetworkTablesFlags flags) {
  if (ImGui::BeginPopupContextItem(path.c_str())) {
    ImGui::Text("%s", path.c_str());
    ImGui::Separator();

    DisplayNetworkTablesAddMenu(model, path, flags);

    ImGui::EndPopup();
  }
}

static void EmitValueName(DataSource* source, const char* name,
                          const char* path) {
  if (source) {
    ImGui::Selectable(name);
    source->EmitDrag();
  } else {
    ImGui::TextUnformatted(name);
  }
  if (ImGui::BeginPopupContextItem(path)) {
    ImGui::TextUnformatted(path);
    ImGui::EndPopup();
  }
}

static void EmitValueTree(
    const std::vector<NetworkTablesModel::EntryValueTreeNode>& children,
    NetworkTablesFlags flags) {
  for (auto&& child : children) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    EmitValueName(child.source.get(), child.name.c_str(), child.path.c_str());

    ImGui::TableNextColumn();
    if (!child.valueChildren.empty()) {
      auto pos = ImGui::GetCursorPos();
      char label[128];
      std::string_view ts = child.typeStr;
      bool havePopup = GetHeadingTypeString(&ts);
      wpi::format_to_n_c_str(label, sizeof(label), "{}##v_{}", ts.data(),
                             child.name.c_str());
      bool valueChildrenOpen =
          TreeNodeEx(label, ImGuiTreeNodeFlags_SpanFullWidth);
      if (havePopup) {
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::TextUnformatted(child.typeStr.c_str());
          ImGui::EndTooltip();
        }
      }
      // make it look like a normal label w/type
      ImGui::SetCursorPos(pos);
      ImGui::LabelText(child.valueChildrenMap ? "{...}" : "[...]", "%s", "");
      if (valueChildrenOpen) {
        EmitValueTree(child.valueChildren, flags);
        TreePop();
      }
    } else {
      EmitEntryValueReadonly(child, nullptr, flags);
    }
  }
}

static void EmitEntry(NetworkTablesModel* model,
                      NetworkTablesModel::Entry& entry, const char* name,
                      NetworkTablesFlags flags, ShowCategory category) {
  if (!IsVisible(category, entry.persistent, entry.retained)) {
    return;
  }

  bool valueChildrenOpen = false;
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  EmitValueName(entry.source.get(), name, entry.info.name.c_str());

  ImGui::TableNextColumn();
  if (!entry.valueChildren.empty()) {
    auto pos = ImGui::GetCursorPos();
    char label[128];
    std::string_view ts = entry.info.type_str;
    bool havePopup = GetHeadingTypeString(&ts);
    wpi::format_to_n_c_str(label, sizeof(label), "{}##v_{}", ts.data(),
                           entry.info.name.c_str());
    valueChildrenOpen =
        TreeNodeEx(label, ImGuiTreeNodeFlags_SpanFullWidth |
                              ImGuiTreeNodeFlags_AllowItemOverlap);
    if (havePopup) {
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(entry.info.type_str.c_str());
        ImGui::EndTooltip();
      }
    }
    // make it look like a normal label w/type
    const char* typeStr = GetTypeString(
        NT_RAW,
        entry.info.type_str.empty() ? nullptr : entry.info.type_str.c_str());
    ImGui::SetCursorPos(pos);
    ImGui::SetNextItemWidth(-1 * (ImGui::CalcTextSize(typeStr).x +
                                  ImGui::GetStyle().FramePadding.x));
    ImGui::LabelText(typeStr, "%s", "");
    if ((entry.value.IsBooleanArray() || entry.value.IsFloatArray() ||
         entry.value.IsDoubleArray() || entry.value.IsIntegerArray() ||
         entry.value.IsStringArray()) &&
        ImGui::BeginPopupContextItem(label)) {
      if (ImGui::Selectable("Edit Array")) {
        if (entry.value.IsBooleanArray()) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_BOOLEAN_ARRAY, int>>(
                  *model, entry.info.name, flags,
                  entry.value.GetBooleanArray());
        } else if (entry.value.IsFloatArray()) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_FLOAT_ARRAY, float>>(
                  *model, entry.info.name, flags, entry.value.GetFloatArray());
        } else if (entry.value.IsDoubleArray()) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_DOUBLE_ARRAY, double>>(
                  *model, entry.info.name, flags, entry.value.GetDoubleArray());
        } else if (entry.value.IsIntegerArray()) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_INTEGER_ARRAY, int64_t>>(
                  *model, entry.info.name, flags,
                  entry.value.GetIntegerArray());
        } else if (entry.value.IsStringArray()) {
          gArrayEditor =
              std::make_unique<ArrayEditorImpl<NT_STRING_ARRAY, std::string>>(
                  *model, entry.info.name, flags, entry.value.GetStringArray());
        }
        ImGui::OpenPopup(gArrayEditorID);
      }
      ImGui::EndPopup();
    }
  } else if (flags & NetworkTablesFlags_ReadOnly) {
    EmitEntryValueReadonly(
        entry,
        entry.info.type_str.empty() ? nullptr : entry.info.type_str.c_str(),
        flags);
  } else {
    EmitEntryValueEditable(model, entry, flags);
  }

  if (flags & NetworkTablesFlags_ShowProperties) {
    ImGui::TableNextColumn();
    ImGui::Text("%s", entry.info.properties.c_str());
    if (ImGui::BeginPopupContextItem(entry.info.name.c_str())) {
      if (ImGui::Checkbox("persistent", &entry.persistent)) {
        nt::SetTopicPersistent(entry.info.topic, entry.persistent);
      }
      if (ImGui::Checkbox("retained", &entry.retained)) {
        if (entry.retained) {
          nt::SetTopicProperty(entry.info.topic, "retained", true);
        } else {
          nt::DeleteTopicProperty(entry.info.topic, "retained");
        }
      }
      ImGui::EndPopup();
    }
  }

  if (flags & NetworkTablesFlags_ShowTimestamp) {
    ImGui::TableNextColumn();
    if (entry.value) {
      ImGui::Text("%f", (entry.value.last_change() * 1.0e-6) -
                            (GetZeroTime() * 1.0e-6));
    } else {
      ImGui::TextUnformatted("");
    }
  }

  if (flags & NetworkTablesFlags_ShowServerTimestamp) {
    ImGui::TableNextColumn();
    if (entry.value && entry.value.server_time() != 0) {
      if (entry.value.server_time() == 1) {
        ImGui::TextUnformatted("---");
      } else {
        ImGui::Text("%f", entry.value.server_time() * 1.0e-6);
      }
    } else {
      ImGui::TextUnformatted("");
    }
  }

  if (valueChildrenOpen) {
    EmitValueTree(entry.valueChildren, flags);
    TreePop();
  }
}

static void EmitTree(NetworkTablesModel* model,
                     const std::vector<NetworkTablesModel::TreeNode>& tree,
                     NetworkTablesFlags flags, ShowCategory category,
                     bool root) {
  for (auto&& node : tree) {
    if (root && (flags & NetworkTablesFlags_ShowSpecial) == 0 &&
        wpi::starts_with(node.name, '$')) {
      continue;
    }
    if (node.entry) {
      EmitEntry(model, *node.entry, node.name.c_str(), flags, category);
    }

    if (!node.children.empty()) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      bool open =
          TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
      EmitParentContextMenu(model, node.path, flags);
      if (open) {
        EmitTree(model, node.children, flags, category, false);
        TreePop();
      }
    }
  }
}

static void DisplayTable(NetworkTablesModel* model,
                         const std::vector<NetworkTablesModel::TreeNode>& tree,
                         NetworkTablesFlags flags, ShowCategory category) {
  if (tree.empty()) {
    return;
  }

  const bool showProperties = (flags & NetworkTablesFlags_ShowProperties);
  const bool showTimestamp = (flags & NetworkTablesFlags_ShowTimestamp);
  const bool showServerTimestamp =
      (flags & NetworkTablesFlags_ShowServerTimestamp);

  ImGui::BeginTable("values",
                    2 + (showProperties ? 1 : 0) + (showTimestamp ? 1 : 0) +
                        (showServerTimestamp ? 1 : 0),
                    ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_BordersInner);
  ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed,
                          0.35f * ImGui::GetWindowWidth());
  ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed,
                          12 * ImGui::GetFontSize());
  if (showProperties) {
    ImGui::TableSetupColumn("Properties", ImGuiTableColumnFlags_WidthFixed,
                            12 * ImGui::GetFontSize());
  }
  if (showTimestamp) {
    ImGui::TableSetupColumn("Time");
  }
  if (showServerTimestamp) {
    ImGui::TableSetupColumn("Server Time");
  }
  ImGui::TableHeadersRow();

  if (flags & NetworkTablesFlags_TreeView) {
    switch (category) {
      case ShowPersistent:
        PushID("persistent");
        break;
      case ShowRetained:
        PushID("retained");
        break;
      case ShowTransitory:
        PushID("transitory");
        break;
      default:
        break;
    }
    EmitTree(model, tree, flags, category, true);
    if (category != ShowAll) {
      PopID();
    }
  } else {
    for (auto entry : model->GetEntries()) {
      if ((flags & NetworkTablesFlags_ShowSpecial) != 0 ||
          !wpi::starts_with(entry->info.name, '$')) {
        EmitEntry(model, *entry, entry->info.name.c_str(), flags, category);
      }
    }
  }
  ImGui::EndTable();
}

static void DisplayClient(const NetworkTablesModel::Client& client) {
  if (CollapsingHeader("Publishers")) {
    ImGui::BeginTable("publishers", 2, ImGuiTableFlags_Resizable);
    ImGui::TableSetupColumn("UID", ImGuiTableColumnFlags_WidthFixed,
                            10 * ImGui::GetFontSize());
    ImGui::TableSetupColumn("Topic");
    ImGui::TableHeadersRow();
    for (auto&& pub : client.publishers) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%" PRId64, pub.uid);
      ImGui::TableNextColumn();
      ImGui::Text("%s", pub.topic.c_str());
    }
    ImGui::EndTable();
  }
  if (CollapsingHeader("Subscribers")) {
    ImGui::BeginTable(
        "subscribers", 6,
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);
    ImGui::TableSetupColumn("UID", ImGuiTableColumnFlags_WidthFixed,
                            10 * ImGui::GetFontSize());
    ImGui::TableSetupColumn("Topics", ImGuiTableColumnFlags_WidthStretch, 6.0f);
    ImGui::TableSetupColumn("Periodic", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableSetupColumn("Topics Only", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableSetupColumn("Send All", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableSetupColumn("Prefix Match", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableHeadersRow();
    for (auto&& sub : client.subscribers) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%" PRId64, sub.uid);
      ImGui::TableNextColumn();
      ImGui::Text("%s", sub.topicsStr.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%0.3f", sub.options.periodic);
      ImGui::TableNextColumn();
      ImGui::Text(sub.options.topicsOnly ? "Yes" : "No");
      ImGui::TableNextColumn();
      ImGui::Text(sub.options.sendAll ? "Yes" : "No");
      ImGui::TableNextColumn();
      ImGui::Text(sub.options.prefixMatch ? "Yes" : "No");
    }
    ImGui::EndTable();
  }
}

void glass::DisplayNetworkTablesInfo(NetworkTablesModel* model) {
  auto inst = model->GetInstance();

  if (CollapsingHeader("Connections")) {
    ImGui::BeginTable("connections", 4, ImGuiTableFlags_Resizable);
    ImGui::TableSetupColumn("Id");
    ImGui::TableSetupColumn("Address");
    ImGui::TableSetupColumn("Updated");
    ImGui::TableSetupColumn("Proto");
    ImGui::TableSetupScrollFreeze(1, 0);
    ImGui::TableHeadersRow();
    for (auto&& i : inst.GetConnections()) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%s", i.remote_id.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%s", i.remote_ip.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%llu",
                  static_cast<unsigned long long>(  // NOLINT(runtime/int)
                      i.last_update));
      ImGui::TableNextColumn();
      ImGui::Text("%d.%d", i.protocol_version >> 8, i.protocol_version & 0xff);
    }
    ImGui::EndTable();
  }

  auto netMode = inst.GetNetworkMode();
  if (netMode == NT_NET_MODE_SERVER || netMode == NT_NET_MODE_CLIENT4) {
    if (CollapsingHeader("Server")) {
      PushID("Server");
      ImGui::Indent();
      DisplayClient(model->GetServer());
      ImGui::Unindent();
      PopID();
    }
    if (CollapsingHeader("Clients")) {
      ImGui::Indent();
      for (auto&& client : model->GetClients()) {
        if (CollapsingHeader(client.second.id.c_str())) {
          PushID(client.second.id.c_str());
          ImGui::Indent();
          ImGui::Text("%s (version %u.%u)", client.second.conn.c_str(),
                      client.second.version >> 8, client.second.version & 0xff);
          DisplayClient(client.second);
          ImGui::Unindent();
          PopID();
        }
      }
      ImGui::Unindent();
    }
  }
}

void glass::DisplayNetworkTables(NetworkTablesModel* model,
                                 NetworkTablesFlags flags) {
  gArrayEditorID = ImGui::GetID("Array Editor");
  if (ImGui::BeginPopupModal("Array Editor", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    if (!gArrayEditor || gArrayEditor->Emit()) {
      ImGui::CloseCurrentPopup();
      gArrayEditor.release();
    }
    ImGui::EndPopup();
  }

  if (flags & NetworkTablesFlags_CombinedView) {
    DisplayTable(model, model->GetTreeRoot(), flags, ShowAll);
  } else {
    if (CollapsingHeader("Persistent Values", ImGuiTreeNodeFlags_DefaultOpen)) {
      DisplayTable(model, model->GetPersistentTreeRoot(), flags,
                   ShowPersistent);
    }

    if (CollapsingHeader("Retained Values", ImGuiTreeNodeFlags_DefaultOpen)) {
      DisplayTable(model, model->GetRetainedTreeRoot(), flags, ShowRetained);
    }

    if (CollapsingHeader("Transitory Values", ImGuiTreeNodeFlags_DefaultOpen)) {
      DisplayTable(model, model->GetTransitoryTreeRoot(), flags,
                   ShowTransitory);
    }
  }
}

void NetworkTablesFlagsSettings::Update() {
  if (!m_pTreeView) {
    auto& storage = GetStorage();
    m_pTreeView =
        &storage.GetBool("tree", m_defaultFlags & NetworkTablesFlags_TreeView);
    m_pCombinedView = &storage.GetBool(
        "combined", m_defaultFlags & NetworkTablesFlags_CombinedView);
    m_pShowSpecial = &storage.GetBool(
        "special", m_defaultFlags & NetworkTablesFlags_ShowSpecial);
    m_pShowProperties = &storage.GetBool(
        "properties", m_defaultFlags & NetworkTablesFlags_ShowProperties);
    m_pShowTimestamp = &storage.GetBool(
        "timestamp", m_defaultFlags & NetworkTablesFlags_ShowTimestamp);
    m_pShowServerTimestamp = &storage.GetBool(
        "serverTimestamp",
        m_defaultFlags & NetworkTablesFlags_ShowServerTimestamp);
    m_pCreateNoncanonicalKeys = &storage.GetBool(
        "createNonCanonical",
        m_defaultFlags & NetworkTablesFlags_CreateNoncanonicalKeys);
    m_pPrecision = &storage.GetInt(
        "precision", (m_defaultFlags & NetworkTablesFlags_Precision) >>
                         kNetworkTablesFlags_PrecisionBitShift);
  }

  m_flags &= ~(
      NetworkTablesFlags_TreeView | NetworkTablesFlags_CombinedView |
      NetworkTablesFlags_ShowSpecial | NetworkTablesFlags_ShowProperties |
      NetworkTablesFlags_ShowTimestamp |
      NetworkTablesFlags_ShowServerTimestamp |
      NetworkTablesFlags_CreateNoncanonicalKeys | NetworkTablesFlags_Precision);
  m_flags |=
      (*m_pTreeView ? NetworkTablesFlags_TreeView : 0) |
      (*m_pCombinedView ? NetworkTablesFlags_CombinedView : 0) |
      (*m_pShowSpecial ? NetworkTablesFlags_ShowSpecial : 0) |
      (*m_pShowProperties ? NetworkTablesFlags_ShowProperties : 0) |
      (*m_pShowTimestamp ? NetworkTablesFlags_ShowTimestamp : 0) |
      (*m_pShowServerTimestamp ? NetworkTablesFlags_ShowServerTimestamp : 0) |
      (*m_pCreateNoncanonicalKeys ? NetworkTablesFlags_CreateNoncanonicalKeys
                                  : 0) |
      (*m_pPrecision << kNetworkTablesFlags_PrecisionBitShift);
}

void NetworkTablesFlagsSettings::DisplayMenu() {
  if (!m_pTreeView) {
    return;
  }
  ImGui::MenuItem("Tree View", "", m_pTreeView);
  ImGui::MenuItem("Combined View", "", m_pCombinedView);
  ImGui::MenuItem("Show Special", "", m_pShowSpecial);
  ImGui::MenuItem("Show Properties", "", m_pShowProperties);
  ImGui::MenuItem("Show Timestamp", "", m_pShowTimestamp);
  ImGui::MenuItem("Show Server Timestamp", "", m_pShowServerTimestamp);
  if (ImGui::BeginMenu("Decimal Precision")) {
    static const char* precisionOptions[] = {"1", "2", "3", "4", "5",
                                             "6", "7", "8", "9", "10"};
    for (int i = 1; i <= 10; i++) {
      if (ImGui::MenuItem(precisionOptions[i - 1], nullptr,
                          i == *m_pPrecision)) {
        *m_pPrecision = i;
      }
    }
    ImGui::EndMenu();
  }
  ImGui::Separator();
  ImGui::MenuItem("Allow creation of non-canonical keys", "",
                  m_pCreateNoncanonicalKeys);
}

void NetworkTablesView::Display() {
  m_flags.Update();
  DisplayNetworkTables(m_model, m_flags.GetFlags());
}

void NetworkTablesView::Settings() {
  m_flags.DisplayMenu();
  DisplayNetworkTablesAddMenu(m_model, {}, m_flags.GetFlags());
}

bool NetworkTablesView::HasSettings() {
  return true;
}
