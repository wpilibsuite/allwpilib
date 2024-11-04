// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/Storage.h"

#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <imgui.h>
#include <wpi/StringExtras.h>
#include <wpi/json.h>

using namespace glass;

template <typename To>
bool ConvertFromString(To* out, std::string_view str) {
  if constexpr (std::same_as<To, bool>) {
    if (str == "true") {
      *out = true;
    } else if (str == "false") {
      *out = false;
    } else if (auto val = wpi::parse_integer<int>(str, 10)) {
      *out = val.value() != 0;
    } else {
      return false;
    }
  } else if constexpr (std::floating_point<To>) {
    if (auto val = wpi::parse_float<To>(str)) {
      *out = val.value();
    } else {
      return false;
    }
  } else {
    if (auto val = wpi::parse_integer<To>(str, 10)) {
      *out = val.value();
    } else {
      return false;
    }
  }
  return true;
}

#define CONVERT(CapsName, LowerName, CType)                                 \
  static bool Convert##CapsName(Storage::Value* value) {                    \
    switch (value->type) {                                                  \
      case Storage::Value::kBool:                                           \
        value->LowerName##Val = value->boolVal;                             \
        value->LowerName##Default = value->boolDefault;                     \
        break;                                                              \
      case Storage::Value::kDouble:                                         \
        value->LowerName##Val = value->doubleVal;                           \
        value->LowerName##Default = value->doubleDefault;                   \
        break;                                                              \
      case Storage::Value::kFloat:                                          \
        value->LowerName##Val = value->floatVal;                            \
        value->LowerName##Default = value->floatDefault;                    \
        break;                                                              \
      case Storage::Value::kInt:                                            \
        value->LowerName##Val = value->intVal;                              \
        value->LowerName##Default = value->intDefault;                      \
        break;                                                              \
      case Storage::Value::kInt64:                                          \
        value->LowerName##Val = value->int64Val;                            \
        value->LowerName##Default = value->int64Default;                    \
        break;                                                              \
      case Storage::Value::kString:                                         \
        if (!ConvertFromString(&value->LowerName##Val, value->stringVal)) { \
          return false;                                                     \
        }                                                                   \
        if (!ConvertFromString(&value->LowerName##Default,                  \
                               value->stringDefault)) {                     \
          return false;                                                     \
        }                                                                   \
        break;                                                              \
      default:                                                              \
        return false;                                                       \
    }                                                                       \
    value->type = Storage::Value::k##CapsName;                              \
    return true;                                                            \
  }

CONVERT(Int, int, int)
CONVERT(Int64, int64, int64_t)
CONVERT(Float, float, float)
CONVERT(Double, double, double)
CONVERT(Bool, bool, bool)

static inline bool ConvertString(Storage::Value* value) {
  return false;
}

// Arrays can only come from JSON, so we only have to worry about conversions
// between the various number types, not bool or string

template <typename From, typename To>
static void ConvertArray(std::vector<To>** outPtr, std::vector<From>** inPtr) {
  if (*inPtr) {
    if (*outPtr) {
      (*outPtr)->assign((*inPtr)->begin(), (*inPtr)->end());
    } else {
      std::vector<To>* tmp;
      tmp = new std::vector<To>{(*inPtr)->begin(), (*inPtr)->end()};
      delete *inPtr;
      *outPtr = tmp;
    }
  } else {
    *outPtr = nullptr;
  }
}

#define CONVERT_ARRAY(CapsName, LowerName)                           \
  static bool Convert##CapsName##Array(Storage::Value* value) {      \
    switch (value->type) {                                           \
      case Storage::Value::kDoubleArray:                             \
        ConvertArray(&value->LowerName##Array, &value->doubleArray); \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->doubleArrayDefault);                    \
        break;                                                       \
      case Storage::Value::kFloatArray:                              \
        ConvertArray(&value->LowerName##Array, &value->floatArray);  \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->floatArrayDefault);                     \
        break;                                                       \
      case Storage::Value::kIntArray:                                \
        ConvertArray(&value->LowerName##Array, &value->intArray);    \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->intArrayDefault);                       \
        break;                                                       \
      case Storage::Value::kInt64Array:                              \
        ConvertArray(&value->LowerName##Array, &value->int64Array);  \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->int64ArrayDefault);                     \
        break;                                                       \
      default:                                                       \
        return false;                                                \
    }                                                                \
    value->type = Storage::Value::k##CapsName##Array;                \
    return true;                                                     \
  }

CONVERT_ARRAY(Int, int)
CONVERT_ARRAY(Int64, int64)
CONVERT_ARRAY(Float, float)
CONVERT_ARRAY(Double, double)

static inline bool ConvertBoolArray(Storage::Value* value) {
  return false;
}

static inline bool ConvertStringArray(Storage::Value* value) {
  return false;
}

void Storage::Value::Reset(Type newType) {
  switch (type) {
    case kChild:
      delete child;
      break;
    case kIntArray:
      delete intArray;
      delete intArrayDefault;
      break;
    case kInt64Array:
      delete int64Array;
      delete int64ArrayDefault;
      break;
    case kBoolArray:
      delete boolArray;
      delete boolArrayDefault;
      break;
    case kFloatArray:
      delete floatArray;
      delete floatArrayDefault;
      break;
    case kDoubleArray:
      delete doubleArray;
      delete doubleArrayDefault;
      break;
    case kStringArray:
      delete stringArray;
      delete stringArrayDefault;
      break;
    case kChildArray:
      delete childArray;
      break;
    default:
      break;
  }
  type = newType;
}

Storage::Value* Storage::FindValue(std::string_view key) {
  auto it = m_values.find(key);
  if (it == m_values.end()) {
    return nullptr;
  }
  return &it->second;
}

#define DEFUN(CapsName, LowerName, CType, CParamType, ArrCType)                \
  CType Storage::Read##CapsName(std::string_view key, CParamType defaultVal)   \
      const {                                                                  \
    auto it = m_values.find(key);                                              \
    if (it == m_values.end()) {                                                \
      return CType{defaultVal};                                                \
    }                                                                          \
    Value& value = it->second;                                                 \
    if (value.type != Value::k##CapsName) {                                    \
      if (!Convert##CapsName(&value)) {                                        \
        value.Reset(Value::k##CapsName);                                       \
        value.LowerName##Val = defaultVal;                                     \
        value.LowerName##Default = defaultVal;                                 \
        value.hasDefault = true;                                               \
      }                                                                        \
    }                                                                          \
    return value.LowerName##Val;                                               \
  }                                                                            \
                                                                               \
  void Storage::Set##CapsName(std::string_view key, CParamType val) {          \
    auto [it, isNew] = m_values.try_emplace(key, Value::k##CapsName);          \
    if (!isNew) {                                                              \
      it->second.Reset(Value::k##CapsName);                                    \
    }                                                                          \
    it->second.LowerName##Val = val;                                           \
    it->second.LowerName##Default = {};                                        \
  }                                                                            \
                                                                               \
  CType& Storage::Get##CapsName(std::string_view key, CParamType defaultVal) { \
    auto [it, setValue] = m_values.try_emplace(key, Value::k##CapsName);       \
    if (!setValue && it->second.type != Value::k##CapsName) {                  \
      if (!Convert##CapsName(&it->second)) {                                   \
        it->second.Reset(Value::k##CapsName);                                  \
        setValue = true;                                                       \
      }                                                                        \
    }                                                                          \
    Value& value = it->second;                                                 \
    if (setValue) {                                                            \
      value.LowerName##Val = defaultVal;                                       \
    }                                                                          \
    if (!value.hasDefault) {                                                   \
      value.LowerName##Default = defaultVal;                                   \
      value.hasDefault = true;                                                 \
    }                                                                          \
    return value.LowerName##Val;                                               \
  }                                                                            \
                                                                               \
  std::vector<ArrCType>& Storage::Get##CapsName##Array(                        \
      std::string_view key, std::span<const ArrCType> defaultVal) {            \
    auto [it, setValue] =                                                      \
        m_values.try_emplace(key, Value::k##CapsName##Array);                  \
    if (!setValue && it->second.type != Value::k##CapsName##Array) {           \
      if (!Convert##CapsName##Array(&it->second)) {                            \
        it->second.Reset(Value::k##CapsName##Array);                           \
        setValue = true;                                                       \
      }                                                                        \
    }                                                                          \
    Value& value = it->second;                                                 \
    if (setValue) {                                                            \
      value.LowerName##Array =                                                 \
          new std::vector<ArrCType>{defaultVal.begin(), defaultVal.end()};     \
    }                                                                          \
    if (!value.hasDefault) {                                                   \
      if (defaultVal.empty()) {                                                \
        value.LowerName##ArrayDefault = nullptr;                               \
      } else {                                                                 \
        value.LowerName##ArrayDefault =                                        \
            new std::vector<ArrCType>{defaultVal.begin(), defaultVal.end()};   \
      }                                                                        \
      value.hasDefault = true;                                                 \
    }                                                                          \
    assert(value.LowerName##Array);                                            \
    return *value.LowerName##Array;                                            \
  }

DEFUN(Int, int, int, int, int)
DEFUN(Int64, int64, int64_t, int64_t, int64_t)
DEFUN(Bool, bool, bool, bool, int)
DEFUN(Float, float, float, float, float)
DEFUN(Double, double, double, double, double)
DEFUN(String, string, std::string, std::string_view, std::string)

Storage& Storage::GetChild(std::string_view label_id) {
  auto [label, id] = wpi::split(label_id, "###");
  if (id.empty()) {
    id = label;
  }
  Value& childValue = GetValue(id);
  if (childValue.type != Value::kChild) {
    childValue.Reset(Value::kChild);
    childValue.child = new Storage;
  }
  return *childValue.child;
}

std::vector<std::unique_ptr<Storage>>& Storage::GetChildArray(
    std::string_view key) {
  auto [it, isNew] = m_values.try_emplace(key, Value::kChildArray);
  if (!isNew && it->second.type != Value::kChildArray) {
    it->second.Reset(Value::kChildArray);
    it->second.childArray = new std::vector<std::unique_ptr<Storage>>();
  }

  return *it->second.childArray;
}

void Storage::Erase(std::string_view key) {
  auto it = m_values.find(key);
  if (it != m_values.end()) {
    m_values.erase(it);
  }
}

void Storage::EraseChildren() {
  std::erase_if(m_values,
                [](const auto& kv) { return kv.second.type == Value::kChild; });
}

static bool JsonArrayToStorage(Storage::Value* valuePtr, const wpi::json& jarr,
                               const char* filename) {
  auto& arr = jarr.get_ref<const wpi::json::array_t&>();
  if (arr.empty()) {
    ImGui::LogText("empty array in %s, ignoring", filename);
    return false;
  }

  // guess array type from first element
  switch (arr[0].type()) {
    case wpi::json::value_t::boolean:
      if (valuePtr->type != Storage::Value::kBoolArray) {
        valuePtr->Reset(Storage::Value::kBoolArray);
        valuePtr->boolArray = new std::vector<int>();
        valuePtr->boolArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::number_float:
      if (valuePtr->type != Storage::Value::kDoubleArray) {
        valuePtr->Reset(Storage::Value::kDoubleArray);
        valuePtr->doubleArray = new std::vector<double>();
        valuePtr->doubleArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::number_integer:
    case wpi::json::value_t::number_unsigned:
      if (valuePtr->type != Storage::Value::kInt64Array) {
        valuePtr->Reset(Storage::Value::kInt64Array);
        valuePtr->int64Array = new std::vector<int64_t>();
        valuePtr->int64ArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::string:
      if (valuePtr->type != Storage::Value::kStringArray) {
        valuePtr->Reset(Storage::Value::kStringArray);
        valuePtr->stringArray = new std::vector<std::string>();
        valuePtr->stringArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::object:
      if (valuePtr->type != Storage::Value::kChildArray) {
        valuePtr->Reset(Storage::Value::kChildArray);
        valuePtr->childArray = new std::vector<std::unique_ptr<Storage>>();
      }
      break;
    case wpi::json::value_t::array:
      ImGui::LogText("nested array in %s, ignoring", filename);
      return false;
    default:
      ImGui::LogText("null value in %s, ignoring", filename);
      return false;
  }

  // loop over array to store elements
  for (auto jvalue : arr) {
    switch (jvalue.type()) {
      case wpi::json::value_t::boolean:
        if (valuePtr->type == Storage::Value::kBoolArray) {
          valuePtr->boolArray->push_back(jvalue.get<bool>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_float:
        if (valuePtr->type == Storage::Value::kDoubleArray) {
          valuePtr->doubleArray->push_back(jvalue.get<double>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_integer:
        if (valuePtr->type == Storage::Value::kInt64Array) {
          valuePtr->int64Array->push_back(jvalue.get<int64_t>());
        } else if (valuePtr->type == Storage::Value::kDoubleArray) {
          valuePtr->doubleArray->push_back(jvalue.get<int64_t>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_unsigned:
        if (valuePtr->type == Storage::Value::kInt64Array) {
          valuePtr->int64Array->push_back(jvalue.get<uint64_t>());
        } else if (valuePtr->type == Storage::Value::kDoubleArray) {
          valuePtr->doubleArray->push_back(jvalue.get<uint64_t>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::string:
        if (valuePtr->type == Storage::Value::kStringArray) {
          valuePtr->stringArray->emplace_back(
              jvalue.get_ref<const std::string&>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::object:
        if (valuePtr->type == Storage::Value::kChildArray) {
          valuePtr->childArray->emplace_back(std::make_unique<Storage>());
          valuePtr->childArray->back()->FromJson(jvalue, filename);
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::array:
        ImGui::LogText("nested array in %s, ignoring", filename);
        return false;
      default:
        ImGui::LogText("null value in %s, ignoring", filename);
        return false;
    }
  }
  return true;

error:
  ImGui::LogText("array with variant types in %s, ignoring", filename);
  return false;
}

bool Storage::FromJson(const wpi::json& json, const char* filename) {
  if (m_fromJson) {
    return m_fromJson(json, filename);
  }

  if (!json.is_object()) {
    ImGui::LogText("non-object in %s", filename);
    return false;
  }
  for (auto&& jkv : json.items()) {
    auto [it, created] = m_values.try_emplace(jkv.key());
    auto& jvalue = jkv.value();
    switch (jvalue.type()) {
      case wpi::json::value_t::boolean:
        it->second.Reset(Value::kBool);
        it->second.boolVal = jvalue.get<bool>();
        break;
      case wpi::json::value_t::number_float:
        it->second.Reset(Value::kDouble);
        it->second.doubleVal = jvalue.get<double>();
        break;
      case wpi::json::value_t::number_integer:
        it->second.Reset(Value::kInt64);
        it->second.int64Val = jvalue.get<int64_t>();
        break;
      case wpi::json::value_t::number_unsigned:
        it->second.Reset(Value::kInt64);
        it->second.int64Val = jvalue.get<uint64_t>();
        break;
      case wpi::json::value_t::string:
        it->second.Reset(Value::kString);
        it->second.stringVal = jvalue.get_ref<const std::string&>();
        break;
      case wpi::json::value_t::object:
        if (it->second.type != Value::kChild) {
          it->second.Reset(Value::kChild);
          it->second.child = new Storage;
        }
        it->second.child->FromJson(jvalue, filename);  // recurse
        break;
      case wpi::json::value_t::array:
        if (!JsonArrayToStorage(&it->second, jvalue, filename)) {
          if (created) {
            m_values.erase(it);
          }
        }
        break;
      default:
        ImGui::LogText("null value in %s, ignoring", filename);
        if (created) {
          m_values.erase(it);
        }
        break;
    }
  }
  return true;
}

template <typename T>
static wpi::json StorageToJsonArray(const std::vector<T>& arr) {
  wpi::json jarr = wpi::json::array();
  for (auto&& v : arr) {
    jarr.emplace_back(v);
  }
  return jarr;
}

template <>
wpi::json StorageToJsonArray<std::unique_ptr<Storage>>(
    const std::vector<std::unique_ptr<Storage>>& arr) {
  wpi::json jarr = wpi::json::array();
  for (auto&& v : arr) {
    jarr.emplace_back(v->ToJson());
  }
  // remove any trailing empty items
  while (!jarr.empty() && jarr.back().empty()) {
    jarr.get_ref<wpi::json::array_t&>().pop_back();
  }
  return jarr;
}

wpi::json Storage::ToJson() const {
  if (m_toJson) {
    return m_toJson();
  }

  wpi::json j = wpi::json::object();
  for (auto&& kv : m_values) {
    wpi::json jelem;
    Value& value = kv.second;
    switch (value.type) {
#define CASE(CapsName, LowerName)                                        \
  case Value::k##CapsName:                                               \
    if (value.hasDefault &&                                              \
        value.LowerName##Val == value.LowerName##Default) {              \
      continue;                                                          \
    }                                                                    \
    jelem = value.LowerName##Val;                                        \
    break;                                                               \
  case Value::k##CapsName##Array:                                        \
    if (value.hasDefault &&                                              \
        ((!value.LowerName##ArrayDefault &&                              \
          value.LowerName##Array->empty()) ||                            \
         (value.LowerName##ArrayDefault &&                               \
          *value.LowerName##Array == *value.LowerName##ArrayDefault))) { \
      continue;                                                          \
    }                                                                    \
    jelem = StorageToJsonArray(*value.LowerName##Array);                 \
    break;

      CASE(Int, int)
      CASE(Int64, int64)
      CASE(Bool, bool)
      CASE(Float, float)
      CASE(Double, double)
      CASE(String, string)

      case Value::kChild:
        jelem = value.child->ToJson();  // recurse
        if (jelem.empty()) {
          continue;
        }
        break;
      case Value::kChildArray:
        jelem = StorageToJsonArray(*value.childArray);
        if (jelem.empty()) {
          continue;
        }
        break;
      default:
        continue;
    }
    j.emplace(kv.first, std::move(jelem));
  }
  return j;
}

void Storage::Clear() {
  if (m_clear) {
    return m_clear();
  }

  ClearValues();
}

void Storage::ClearValues() {
  for (auto&& kv : m_values) {
    Value& value = kv.second;
    switch (value.type) {
      case Value::kInt:
        value.intVal = value.intDefault;
        break;
      case Value::kInt64:
        value.int64Val = value.int64Default;
        break;
      case Value::kBool:
        value.boolVal = value.boolDefault;
        break;
      case Value::kFloat:
        value.floatVal = value.floatDefault;
        break;
      case Value::kDouble:
        value.doubleVal = value.doubleDefault;
        break;
      case Value::kString:
        value.stringVal = value.stringDefault;
        break;
      case Value::kIntArray:
        if (value.intArrayDefault) {
          *value.intArray = *value.intArrayDefault;
        } else {
          value.intArray->clear();
        }
        break;
      case Value::kInt64Array:
        if (value.int64ArrayDefault) {
          *value.int64Array = *value.int64ArrayDefault;
        } else {
          value.int64Array->clear();
        }
        break;
      case Value::kBoolArray:
        if (value.boolArrayDefault) {
          *value.boolArray = *value.boolArrayDefault;
        } else {
          value.boolArray->clear();
        }
        break;
      case Value::kFloatArray:
        if (value.floatArrayDefault) {
          *value.floatArray = *value.floatArrayDefault;
        } else {
          value.floatArray->clear();
        }
        break;
      case Value::kDoubleArray:
        if (value.doubleArrayDefault) {
          *value.doubleArray = *value.doubleArrayDefault;
        } else {
          value.doubleArray->clear();
        }
        break;
      case Value::kStringArray:
        if (value.stringArrayDefault) {
          *value.stringArray = *value.stringArrayDefault;
        } else {
          value.stringArray->clear();
        }
        break;
      case Value::kChild:
        value.child->Clear();
        break;
      case Value::kChildArray:
        for (auto&& child : *value.childArray) {
          child->Clear();
        }
        break;
      default:
        break;
    }
  }
}

void Storage::Apply() {
  if (m_apply) {
    return m_apply();
  }

  ApplyChildren();
}

void Storage::ApplyChildren() {
  for (auto&& kv : m_values) {
    Value& value = kv.second;
    switch (value.type) {
      case Value::kChild:
        value.child->Apply();
        break;
      case Value::kChildArray:
        for (auto&& child : *value.childArray) {
          child->Apply();
        }
        break;
      default:
        break;
    }
  }
}
