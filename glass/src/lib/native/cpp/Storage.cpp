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

#define CONVERT(CapsName, LowerName, AllCapsName, CType)                    \
  static bool Convert##CapsName(Storage::Value* value) {                    \
    switch (value->type) {                                                  \
      case Storage::Value::BOOL:                                            \
        value->LowerName##Val = value->boolVal;                             \
        value->LowerName##Default = value->boolDefault;                     \
        break;                                                              \
      case Storage::Value::DOUBLE:                                          \
        value->LowerName##Val = value->doubleVal;                           \
        value->LowerName##Default = value->doubleDefault;                   \
        break;                                                              \
      case Storage::Value::FLOAT:                                           \
        value->LowerName##Val = value->floatVal;                            \
        value->LowerName##Default = value->floatDefault;                    \
        break;                                                              \
      case Storage::Value::INT:                                             \
        value->LowerName##Val = value->intVal;                              \
        value->LowerName##Default = value->intDefault;                      \
        break;                                                              \
      case Storage::Value::INT64:                                           \
        value->LowerName##Val = value->int64Val;                            \
        value->LowerName##Default = value->int64Default;                    \
        break;                                                              \
      case Storage::Value::STRING:                                          \
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
    value->type = Storage::Value::AllCapsName;                              \
    return true;                                                            \
  }

CONVERT(Int, int, INT, int)
CONVERT(Int64, int64, INT64, int64_t)
CONVERT(Float, float, FLOAT, float)
CONVERT(Double, double, DOUBLE, double)
CONVERT(Bool, bool, BOOL, bool)

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

#define CONVERT_ARRAY(CapsName, LowerName, AllCapsName)              \
  static bool Convert##CapsName##Array(Storage::Value* value) {      \
    switch (value->type) {                                           \
      case Storage::Value::DOUBLE_ARRAY:                             \
        ConvertArray(&value->LowerName##Array, &value->doubleArray); \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->doubleArrayDefault);                    \
        break;                                                       \
      case Storage::Value::FLOAT_ARRAY:                              \
        ConvertArray(&value->LowerName##Array, &value->floatArray);  \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->floatArrayDefault);                     \
        break;                                                       \
      case Storage::Value::INT_ARRAY:                                \
        ConvertArray(&value->LowerName##Array, &value->intArray);    \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->intArrayDefault);                       \
        break;                                                       \
      case Storage::Value::INT64_ARRAY:                              \
        ConvertArray(&value->LowerName##Array, &value->int64Array);  \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->int64ArrayDefault);                     \
        break;                                                       \
      default:                                                       \
        return false;                                                \
    }                                                                \
    value->type = Storage::Value::AllCapsName##_ARRAY;             \
    return true;                                                     \
  }

CONVERT_ARRAY(Int, int, INT)
CONVERT_ARRAY(Int64, int64, INT64)
CONVERT_ARRAY(Float, float, FLOAT)
CONVERT_ARRAY(Double, double, DOUBLE)

static inline bool ConvertBoolArray(Storage::Value* value) {
  return false;
}

static inline bool ConvertStringArray(Storage::Value* value) {
  return false;
}

void Storage::Value::Reset(Type newType) {
  switch (type) {
    case CHILD:
      delete child;
      break;
    case INT_ARRAY:
      delete intArray;
      delete intArrayDefault;
      break;
    case INT64_ARRAY:
      delete int64Array;
      delete int64ArrayDefault;
      break;
    case BOOL_ARRAY:
      delete boolArray;
      delete boolArrayDefault;
      break;
    case FLOAT_ARRAY:
      delete floatArray;
      delete floatArrayDefault;
      break;
    case DOUBLE_ARRAY:
      delete doubleArray;
      delete doubleArrayDefault;
      break;
    case STRING_ARRAY:
      delete stringArray;
      delete stringArrayDefault;
      break;
    case CHILD_ARRAY:
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
  return it->second.get();
}

Storage::Value& Storage::GetValue(std::string_view key) {
  auto& val = m_values[key];
  if (!val) {
    val = std::make_unique<Value>();
  }
  return *val;
}

#define DEFUN(CapsName, LowerName, AllCapsName, CType, CParamType, ArrCType)   \
  CType Storage::Read##CapsName(std::string_view key, CParamType defaultVal)   \
      const {                                                                  \
    auto it = m_values.find(key);                                              \
    if (it == m_values.end()) {                                                \
      return CType{defaultVal};                                                \
    }                                                                          \
    Value& value = *it->second;                                                \
    if (value.type != Value::AllCapsName) {                                    \
      if (!Convert##CapsName(&value)) {                                        \
        value.Reset(Value::AllCapsName);                                       \
        value.LowerName##Val = defaultVal;                                     \
        value.LowerName##Default = defaultVal;                                 \
        value.hasDefault = true;                                               \
      }                                                                        \
    }                                                                          \
    return value.LowerName##Val;                                               \
  }                                                                            \
                                                                               \
  void Storage::Set##CapsName(std::string_view key, CParamType val) {          \
    auto& valuePtr = m_values[key];                                            \
    if (!valuePtr) {                                                           \
      valuePtr = std::make_unique<Value>(Value::AllCapsName);                  \
    } else {                                                                   \
      valuePtr->Reset(Value::AllCapsName);                                     \
    }                                                                          \
    valuePtr->LowerName##Val = val;                                            \
    valuePtr->LowerName##Default = {};                                         \
  }                                                                            \
                                                                               \
  CType& Storage::Get##CapsName(std::string_view key, CParamType defaultVal) { \
    auto& valuePtr = m_values[key];                                            \
    bool setValue = false;                                                     \
    if (!valuePtr) {                                                           \
      valuePtr = std::make_unique<Value>(Value::AllCapsName);                  \
      setValue = true;                                                         \
    } else if (valuePtr->type != Value::AllCapsName) {                         \
      if (!Convert##CapsName(valuePtr.get())) {                                \
        valuePtr->Reset(Value::AllCapsName);                                   \
        setValue = true;                                                       \
      }                                                                        \
    }                                                                          \
    if (setValue) {                                                            \
      valuePtr->LowerName##Val = defaultVal;                                   \
    }                                                                          \
    if (!valuePtr->hasDefault) {                                               \
      valuePtr->LowerName##Default = defaultVal;                               \
      valuePtr->hasDefault = true;                                             \
    }                                                                          \
    return valuePtr->LowerName##Val;                                           \
  }                                                                            \
                                                                               \
  std::vector<ArrCType>& Storage::Get##CapsName##Array(                        \
      std::string_view key, std::span<const ArrCType> defaultVal) {            \
    auto& valuePtr = m_values[key];                                            \
    bool setValue = false;                                                     \
    if (!valuePtr) {                                                           \
      valuePtr = std::make_unique<Value>(Value::AllCapsName##_ARRAY);        \
      setValue = true;                                                         \
    } else if (valuePtr->type != Value::AllCapsName##_ARRAY) {               \
      if (!Convert##CapsName##Array(valuePtr.get())) {                         \
        valuePtr->Reset(Value::AllCapsName##_ARRAY);                         \
        setValue = true;                                                       \
      }                                                                        \
    }                                                                          \
    if (setValue) {                                                            \
      valuePtr->LowerName##Array =                                             \
          new std::vector<ArrCType>{defaultVal.begin(), defaultVal.end()};     \
    }                                                                          \
    if (!valuePtr->hasDefault) {                                               \
      if (defaultVal.empty()) {                                                \
        valuePtr->LowerName##ArrayDefault = nullptr;                           \
      } else {                                                                 \
        valuePtr->LowerName##ArrayDefault =                                    \
            new std::vector<ArrCType>{defaultVal.begin(), defaultVal.end()};   \
      }                                                                        \
      valuePtr->hasDefault = true;                                             \
    }                                                                          \
    assert(valuePtr->LowerName##Array);                                        \
    return *valuePtr->LowerName##Array;                                        \
  }

DEFUN(Int, int, INT, int, int, int)
DEFUN(Int64, int64, INT64, int64_t, int64_t, int64_t)
DEFUN(Bool, bool, BOOL, bool, bool, int)
DEFUN(Float, float, FLOAT, float, float, float)
DEFUN(Double, double, DOUBLE, double, double, double)
DEFUN(String, string, STRING, std::string, std::string_view, std::string)

Storage& Storage::GetChild(std::string_view label_id) {
  auto [label, id] = wpi::split(label_id, "###");
  if (id.empty()) {
    id = label;
  }
  auto& childPtr = m_values[id];
  if (!childPtr) {
    childPtr = std::make_unique<Value>();
  }
  if (childPtr->type != Value::CHILD) {
    childPtr->Reset(Value::CHILD);
    childPtr->child = new Storage;
  }
  return *childPtr->child;
}

std::vector<std::unique_ptr<Storage>>& Storage::GetChildArray(
    std::string_view key) {
  auto& valuePtr = m_values[key];
  if (!valuePtr) {
    valuePtr = std::make_unique<Value>(Value::CHILD_ARRAY);
    valuePtr->childArray = new std::vector<std::unique_ptr<Storage>>();
  } else if (valuePtr->type != Value::CHILD_ARRAY) {
    valuePtr->Reset(Value::CHILD_ARRAY);
    valuePtr->childArray = new std::vector<std::unique_ptr<Storage>>();
  }

  return *valuePtr->childArray;
}

std::unique_ptr<Storage::Value> Storage::Erase(std::string_view key) {
  auto it = m_values.find(key);
  if (it != m_values.end()) {
    auto rv = std::move(it->second);
    m_values.erase(it);
    return rv;
  }
  return nullptr;
}

void Storage::EraseChildren() {
  std::erase_if(m_values, [](const auto& kv) {
    return kv.second->type == Value::CHILD;
  });
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
      if (valuePtr->type != Storage::Value::BOOL_ARRAY) {
        valuePtr->Reset(Storage::Value::BOOL_ARRAY);
        valuePtr->boolArray = new std::vector<int>();
        valuePtr->boolArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::number_float:
      if (valuePtr->type != Storage::Value::DOUBLE_ARRAY) {
        valuePtr->Reset(Storage::Value::DOUBLE_ARRAY);
        valuePtr->doubleArray = new std::vector<double>();
        valuePtr->doubleArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::number_integer:
    case wpi::json::value_t::number_unsigned:
      if (valuePtr->type != Storage::Value::INT64_ARRAY) {
        valuePtr->Reset(Storage::Value::INT64_ARRAY);
        valuePtr->int64Array = new std::vector<int64_t>();
        valuePtr->int64ArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::string:
      if (valuePtr->type != Storage::Value::STRING_ARRAY) {
        valuePtr->Reset(Storage::Value::STRING_ARRAY);
        valuePtr->stringArray = new std::vector<std::string>();
        valuePtr->stringArrayDefault = nullptr;
      }
      break;
    case wpi::json::value_t::object:
      if (valuePtr->type != Storage::Value::CHILD_ARRAY) {
        valuePtr->Reset(Storage::Value::CHILD_ARRAY);
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
        if (valuePtr->type == Storage::Value::BOOL_ARRAY) {
          valuePtr->boolArray->push_back(jvalue.get<bool>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_float:
        if (valuePtr->type == Storage::Value::DOUBLE_ARRAY) {
          valuePtr->doubleArray->push_back(jvalue.get<double>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_integer:
        if (valuePtr->type == Storage::Value::INT64_ARRAY) {
          valuePtr->int64Array->push_back(jvalue.get<int64_t>());
        } else if (valuePtr->type == Storage::Value::DOUBLE_ARRAY) {
          valuePtr->doubleArray->push_back(jvalue.get<int64_t>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_unsigned:
        if (valuePtr->type == Storage::Value::INT64_ARRAY) {
          valuePtr->int64Array->push_back(jvalue.get<uint64_t>());
        } else if (valuePtr->type == Storage::Value::DOUBLE_ARRAY) {
          valuePtr->doubleArray->push_back(jvalue.get<uint64_t>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::string:
        if (valuePtr->type == Storage::Value::STRING_ARRAY) {
          valuePtr->stringArray->emplace_back(
              jvalue.get_ref<const std::string&>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::object:
        if (valuePtr->type == Storage::Value::CHILD_ARRAY) {
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
    auto& valuePtr = m_values[jkv.key()];
    bool created = false;
    if (!valuePtr) {
      valuePtr = std::make_unique<Value>();
      created = true;
    }
    auto& jvalue = jkv.value();
    switch (jvalue.type()) {
      case wpi::json::value_t::boolean:
        valuePtr->Reset(Value::BOOL);
        valuePtr->boolVal = jvalue.get<bool>();
        break;
      case wpi::json::value_t::number_float:
        valuePtr->Reset(Value::DOUBLE);
        valuePtr->doubleVal = jvalue.get<double>();
        break;
      case wpi::json::value_t::number_integer:
        valuePtr->Reset(Value::INT64);
        valuePtr->int64Val = jvalue.get<int64_t>();
        break;
      case wpi::json::value_t::number_unsigned:
        valuePtr->Reset(Value::INT64);
        valuePtr->int64Val = jvalue.get<uint64_t>();
        break;
      case wpi::json::value_t::string:
        valuePtr->Reset(Value::STRING);
        valuePtr->stringVal = jvalue.get_ref<const std::string&>();
        break;
      case wpi::json::value_t::object:
        if (valuePtr->type != Value::CHILD) {
          valuePtr->Reset(Value::CHILD);
          valuePtr->child = new Storage;
        }
        valuePtr->child->FromJson(jvalue, filename);  // recurse
        break;
      case wpi::json::value_t::array:
        if (!JsonArrayToStorage(valuePtr.get(), jvalue, filename)) {
          if (created) {
            m_values.erase(jkv.key());
          }
        }
        break;
      default:
        ImGui::LogText("null value in %s, ignoring", filename);
        if (created) {
          m_values.erase(jkv.key());
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
    auto& value = *kv.second;
    switch (value.type) {
#define CASE(CapsName, LowerName, AllCapsName)                           \
  case Value::AllCapsName:                                               \
    if (value.hasDefault &&                                              \
        value.LowerName##Val == value.LowerName##Default) {              \
      continue;                                                          \
    }                                                                    \
    jelem = value.LowerName##Val;                                        \
    break;                                                               \
  case Value::AllCapsName##_ARRAY:                                       \
    if (value.hasDefault &&                                              \
        ((!value.LowerName##ArrayDefault &&                              \
          value.LowerName##Array->empty()) ||                            \
         (value.LowerName##ArrayDefault &&                               \
          *value.LowerName##Array == *value.LowerName##ArrayDefault))) { \
      continue;                                                          \
    }                                                                    \
    jelem = StorageToJsonArray(*value.LowerName##Array);                 \
    break;

      CASE(Int, int, INT)
      CASE(Int64, int64, INT64)
      CASE(Bool, bool, BOOL)
      CASE(Float, float, FLOAT)
      CASE(Double, double, DOUBLE)
      CASE(String, string, STRING)

      case Value::CHILD:
        jelem = value.child->ToJson();  // recurse
        if (jelem.empty()) {
          continue;
        }
        break;
      case Value::CHILD_ARRAY:
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
    auto& value = *kv.second;
    switch (value.type) {
      case Value::INT:
        value.intVal = value.intDefault;
        break;
      case Value::INT64:
        value.int64Val = value.int64Default;
        break;
      case Value::BOOL:
        value.boolVal = value.boolDefault;
        break;
      case Value::FLOAT:
        value.floatVal = value.floatDefault;
        break;
      case Value::DOUBLE:
        value.doubleVal = value.doubleDefault;
        break;
      case Value::STRING:
        value.stringVal = value.stringDefault;
        break;
      case Value::INT_ARRAY:
        if (value.intArrayDefault) {
          *value.intArray = *value.intArrayDefault;
        } else {
          value.intArray->clear();
        }
        break;
      case Value::INT64_ARRAY:
        if (value.int64ArrayDefault) {
          *value.int64Array = *value.int64ArrayDefault;
        } else {
          value.int64Array->clear();
        }
        break;
      case Value::BOOL_ARRAY:
        if (value.boolArrayDefault) {
          *value.boolArray = *value.boolArrayDefault;
        } else {
          value.boolArray->clear();
        }
        break;
      case Value::FLOAT_ARRAY:
        if (value.floatArrayDefault) {
          *value.floatArray = *value.floatArrayDefault;
        } else {
          value.floatArray->clear();
        }
        break;
      case Value::DOUBLE_ARRAY:
        if (value.doubleArrayDefault) {
          *value.doubleArray = *value.doubleArrayDefault;
        } else {
          value.doubleArray->clear();
        }
        break;
      case Value::STRING_ARRAY:
        if (value.stringArrayDefault) {
          *value.stringArray = *value.stringArrayDefault;
        } else {
          value.stringArray->clear();
        }
        break;
      case Value::CHILD:
        value.child->Clear();
        break;
      case Value::CHILD_ARRAY:
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
    auto& value = *kv.second;
    switch (value.type) {
      case Value::CHILD:
        value.child->Apply();
        break;
      case Value::CHILD_ARRAY:
        for (auto&& child : *value.childArray) {
          child->Apply();
        }
        break;
      default:
        break;
    }
  }
}
