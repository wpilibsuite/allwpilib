// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/struct/DynamicStruct.h"

#include <algorithm>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "wpi/Endian.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_ostream.h"
#include "wpi/struct/SchemaParser.h"

using namespace wpi;

static size_t TypeToSize(StructFieldType type) {
  switch (type) {
    case StructFieldType::kBool:
    case StructFieldType::kChar:
    case StructFieldType::kInt8:
    case StructFieldType::kUint8:
      return 1;
    case StructFieldType::kInt16:
    case StructFieldType::kUint16:
      return 2;
    case StructFieldType::kInt32:
    case StructFieldType::kUint32:
    case StructFieldType::kFloat:
      return 4;
    case StructFieldType::kInt64:
    case StructFieldType::kUint64:
    case StructFieldType::kDouble:
      return 8;
    default:
      return 0;
  }
}

static StructFieldType TypeStringToType(std::string_view str) {
  if (str == "bool") {
    return StructFieldType::kBool;
  } else if (str == "char") {
    return StructFieldType::kChar;
  } else if (str == "int8") {
    return StructFieldType::kInt8;
  } else if (str == "int16") {
    return StructFieldType::kInt16;
  } else if (str == "int32") {
    return StructFieldType::kInt32;
  } else if (str == "int64") {
    return StructFieldType::kInt64;
  } else if (str == "uint8") {
    return StructFieldType::kUint8;
  } else if (str == "uint16") {
    return StructFieldType::kUint16;
  } else if (str == "uint32") {
    return StructFieldType::kUint32;
  } else if (str == "uint64") {
    return StructFieldType::kUint64;
  } else if (str == "float" || str == "float32") {
    return StructFieldType::kFloat;
  } else if (str == "double" || str == "float64") {
    return StructFieldType::kDouble;
  } else {
    return StructFieldType::kStruct;
  }
}

static inline unsigned int ToBitWidth(size_t size, unsigned int bitWidth) {
  if (bitWidth == 0) {
    return size * 8;
  } else {
    return bitWidth;
  }
}

static inline uint64_t ToBitMask(size_t size, unsigned int bitWidth) {
  if (size == 0) {
    return 0;
  } else {
    return UINT64_MAX >> (64 - ToBitWidth(size, bitWidth));
  }
}

StructFieldDescriptor::StructFieldDescriptor(
    const StructDescriptor* parent, std::string_view name, StructFieldType type,
    size_t size, size_t arraySize, unsigned int bitWidth, EnumValues enumValues,
    const StructDescriptor* structDesc, const private_init&)
    : m_parent{parent},
      m_name{name},
      m_size{size},
      m_arraySize{arraySize},
      m_enum{std::move(enumValues)},
      m_struct{structDesc},
      m_bitMask{ToBitMask(size, bitWidth)},
      m_type{type},
      m_bitWidth{ToBitWidth(size, bitWidth)} {}

const StructFieldDescriptor* StructDescriptor::FindFieldByName(
    std::string_view name) const {
  auto it = m_fieldsByName.find(name);
  if (it == m_fieldsByName.end()) {
    return nullptr;
  }
  return &m_fields[it->second];
}

bool StructDescriptor::CheckCircular(
    wpi::SmallVectorImpl<const StructDescriptor*>& stack) const {
  stack.emplace_back(this);
  for (auto&& ref : m_references) {
    if (std::find(stack.begin(), stack.end(), ref) != stack.end()) {
      [[unlikely]] return false;
    }
    if (!ref->CheckCircular(stack)) {
      [[unlikely]] return false;
    }
  }
  stack.pop_back();
  return true;
}

std::string StructDescriptor::CalculateOffsets(
    wpi::SmallVectorImpl<const StructDescriptor*>& stack) {
  size_t offset = 0;
  unsigned int shift = 0;
  size_t prevBitfieldSize = 0;
  for (auto&& field : m_fields) {
    if (!field.IsBitField()) {
      [[likely]] shift = 0;        // reset shift on non-bitfield element
      offset += prevBitfieldSize;  // finish bitfield if active
      prevBitfieldSize = 0;        // previous is now not bitfield
      field.m_offset = offset;
      if (field.m_struct) {
        if (!field.m_struct->IsValid()) {
          m_valid = false;
          [[unlikely]] return {};
        }
        field.m_size = field.m_struct->m_size;
      }
      offset += field.m_size * field.m_arraySize;
    } else {
      if (field.m_type == StructFieldType::kBool && prevBitfieldSize != 0 &&
          (shift + 1) <= (prevBitfieldSize * 8)) {
        // bool takes on size of preceding bitfield type (if it fits)
        field.m_size = prevBitfieldSize;
      } else if (field.m_size != prevBitfieldSize ||
                 (shift + field.m_bitWidth) > (field.m_size * 8)) {
        shift = 0;
        offset += prevBitfieldSize;
      }
      prevBitfieldSize = field.m_size;
      field.m_offset = offset;
      field.m_bitShift = shift;
      shift += field.m_bitWidth;
    }
  }

  // update struct size
  m_size = offset + prevBitfieldSize;
  m_valid = true;

  // now that we're valid, referring types may be too
  stack.emplace_back(this);
  for (auto&& ref : m_references) {
    if (std::find(stack.begin(), stack.end(), ref) != stack.end()) {
      [[unlikely]] return fmt::format(
          "internal error (inconsistent data): circular struct reference "
          "between {} and {}",
          m_name, ref->m_name);
    }
    auto err = ref->CalculateOffsets(stack);
    if (!err.empty()) {
      [[unlikely]] return err;
    }
  }
  stack.pop_back();
  return {};
}

const StructDescriptor* StructDescriptorDatabase::Add(std::string_view name,
                                                      std::string_view schema,
                                                      std::string* err) {
  structparser::Parser parser{schema};
  structparser::ParsedSchema parsed;
  if (!parser.Parse(&parsed)) {
    *err = fmt::format("parse error: {}", parser.GetError());
    [[unlikely]] return nullptr;
  }

  // turn parsed schema into descriptors
  auto& theStruct =
      m_structs.try_emplace(name, name, StructDescriptor::private_init{})
          .first->second;
  theStruct.m_schema = schema;
  theStruct.m_fields.clear();
  theStruct.m_fieldsByName.clear();
  theStruct.m_fields.reserve(parsed.declarations.size());
  bool isValid = true;
  for (auto&& decl : parsed.declarations) {
    auto type = TypeStringToType(decl.typeString);
    size_t size = TypeToSize(type);

    // bitfield checks
    if (decl.bitWidth != 0) {
      // only integer or boolean types are allowed
      if (type == StructFieldType::kChar || type == StructFieldType::kFloat ||
          type == StructFieldType::kDouble ||
          type == StructFieldType::kStruct) {
        *err = fmt::format("field {}: type {} cannot be bitfield", decl.name,
                           decl.typeString);
        [[unlikely]] return nullptr;
      }

      // bit width cannot be larger than field size
      if (decl.bitWidth > (size * 8)) {
        *err = fmt::format("field {}: bit width {} exceeds type size",
                           decl.name, decl.bitWidth);
        [[unlikely]] return nullptr;
      }

      // bit width must be 1 for booleans
      if (type == StructFieldType::kBool && decl.bitWidth != 1) {
        *err = fmt::format("field {}: bit width must be 1 for bool type",
                           decl.name);
        [[unlikely]] return nullptr;
      }

      // cannot combine array and bitfield (shouldn't parse, but double-check)
      if (decl.arraySize > 1) {
        *err = fmt::format("field {}: cannot combine array and bitfield",
                           decl.name);
        [[unlikely]] return nullptr;
      }
    }

    // struct handling
    const StructDescriptor* structDesc = nullptr;
    if (type == StructFieldType::kStruct) {
      // recursive definitions are not allowed
      if (decl.typeString == name) {
        *err = fmt::format("field {}: recursive struct reference", decl.name);
        [[unlikely]] return nullptr;
      }

      // cross-reference struct, creating a placeholder if necessary
      auto& aStruct = m_structs
                          .try_emplace(decl.typeString, decl.typeString,
                                       StructDescriptor::private_init{})
                          .first->second;

      // if the struct isn't valid, we can't be valid either
      if (aStruct.IsValid()) {
        size = aStruct.GetSize();
      } else {
        isValid = false;
      }

      // add to cross-references for when the struct does become valid
      aStruct.m_references.emplace_back(&theStruct);
      structDesc = &aStruct;
    }

    // create field
    if (!theStruct.m_fieldsByName.insert({decl.name, theStruct.m_fields.size()})
             .second) {
      *err = fmt::format("duplicate field {}", decl.name);
      [[unlikely]] return nullptr;
    }

    theStruct.m_fields.emplace_back(&theStruct, decl.name, type, size,
                                    decl.arraySize, decl.bitWidth,
                                    std::move(decl.enumValues), structDesc,
                                    StructFieldDescriptor::private_init{});
  }

  theStruct.m_valid = isValid;
  if (isValid) {
    // we have all the info needed, so calculate field offset & shift
    wpi::SmallVector<const StructDescriptor*, 16> stack;
    auto err2 = theStruct.CalculateOffsets(stack);
    if (!err2.empty()) {
      *err = std::move(err2);
      [[unlikely]] return nullptr;
    }
  } else {
    // check for circular reference
    wpi::SmallVector<const StructDescriptor*, 16> stack;
    if (!theStruct.CheckCircular(stack)) {
      wpi::SmallString<128> buf;
      wpi::raw_svector_ostream os{buf};
      for (auto&& elem : stack) {
        if (!buf.empty()) {
          os << " <- ";
        }
        os << elem->GetName();
      }
      *err = fmt::format("circular struct reference: {}", os.str());
      [[unlikely]] return nullptr;
    }
  }

  return &theStruct;
}

const StructDescriptor* StructDescriptorDatabase::Find(
    std::string_view name) const {
  auto it = m_structs.find(name);
  if (it == m_structs.end()) {
    return nullptr;
  }
  return &it->second;
}

uint64_t DynamicStruct::GetFieldImpl(const StructFieldDescriptor* field,
                                     size_t arrIndex) const {
  assert(field->m_parent == m_desc);
  assert(m_desc->IsValid());
  assert(arrIndex < field->m_arraySize);
  uint64_t val;
  switch (field->m_size) {
    case 1:
      val = m_data[field->m_offset + arrIndex];
      break;
    case 2:
      val = support::endian::read16le(&m_data[field->m_offset + arrIndex * 2]);
      break;
    case 4:
      val = support::endian::read32le(&m_data[field->m_offset + arrIndex * 4]);
      break;
    case 8:
      val = support::endian::read64le(&m_data[field->m_offset + arrIndex * 8]);
      break;
    default:
      assert(false && "invalid field size");
      return 0;
  }
  return (val >> field->m_bitShift) & field->m_bitMask;
}

void MutableDynamicStruct::SetData(std::span<const uint8_t> data) {
  assert(data.size() >= m_desc->GetSize());
  std::copy(data.begin(), data.begin() + m_desc->GetSize(), m_data.begin());
}

std::string_view DynamicStruct::GetStringField(
    const StructFieldDescriptor* field) const {
  assert(field->m_type == StructFieldType::kChar);
  assert(field->m_parent == m_desc);
  assert(m_desc->IsValid());
  // Find last non zero character
  size_t stringLength;
  for (stringLength = field->m_arraySize; stringLength > 0; stringLength--) {
    if (m_data[field->m_offset + stringLength - 1] != 0) {
      break;
    }
  }
  // If string is all zeroes, its empty and return an empty string.
  if (stringLength == 0) {
    return "";
  }
  // Check if the end of the string is in the middle of a continuation byte or
  // not.
  if ((m_data[field->m_offset + stringLength - 1] & 0x80) != 0) {
    // This is a UTF8 continuation byte. Make sure its valid.
    // Walk back until initial byte is found
    size_t utf8StartByte = stringLength;
    for (; utf8StartByte > 0; utf8StartByte--) {
      if ((m_data[field->m_offset + utf8StartByte - 1] & 0x40) != 0) {
        // Having 2nd bit set means start byte
        break;
      }
    }
    if (utf8StartByte == 0) {
      // This case means string only contains continuation bytes
      return "";
    }
    utf8StartByte--;
    // Check if its a 2, 3, or 4 byte
    uint8_t checkByte = m_data[field->m_offset + utf8StartByte];
    if ((checkByte & 0xE0) == 0xC0) {
      // 2 byte, need 1 more byte
      if (utf8StartByte != stringLength - 2) {
        stringLength = utf8StartByte;
      }
    } else if ((checkByte & 0xF0) == 0xE0) {
      // 3 byte, need 2 more bytes
      if (utf8StartByte != stringLength - 3) {
        stringLength = utf8StartByte;
      }
    } else if ((checkByte & 0xF8) == 0xF0) {
      // 4 byte, need 3 more bytes
      if (utf8StartByte != stringLength - 4) {
        stringLength = utf8StartByte;
      }
    }
    // If we get here, the string is either completely garbage or fine.
  }
  return {reinterpret_cast<const char*>(&m_data[field->m_offset]),
          stringLength};
}

bool MutableDynamicStruct::SetStringField(const StructFieldDescriptor* field,
                                          std::string_view value) {
  assert(field->m_type == StructFieldType::kChar);
  assert(field->m_parent == m_desc);
  assert(m_desc->IsValid());
  size_t len = (std::min)(field->m_arraySize, value.size());
  bool copiedFull = len == value.size();
  std::copy(value.begin(), value.begin() + len,
            reinterpret_cast<char*>(&m_data[field->m_offset]));
  auto toFill = m_data.subspan(field->m_offset + len, field->m_arraySize - len);
  std::fill(toFill.begin(), toFill.end(), 0);
  return copiedFull;
}

void MutableDynamicStruct::SetStructField(const StructFieldDescriptor* field,
                                          const DynamicStruct& value,
                                          size_t arrIndex) {
  assert(field->m_type == StructFieldType::kStruct);
  assert(field->m_parent == m_desc);
  assert(m_desc->IsValid());
  assert(value.GetDescriptor() == field->m_struct);
  assert(value.GetDescriptor()->IsValid());
  assert(arrIndex < field->m_arraySize);
  auto source = value.GetData();
  size_t len = field->m_struct->GetSize();
  std::copy(source.begin(), source.begin() + len,
            m_data.begin() + field->m_offset + arrIndex * len);
}

void MutableDynamicStruct::SetFieldImpl(const StructFieldDescriptor* field,
                                        uint64_t value, size_t arrIndex) {
  assert(field->m_parent == m_desc);
  assert(m_desc->IsValid());
  assert(arrIndex < field->m_arraySize);

  // common case is no bit shift and no masking
  if (!field->IsBitField()) {
    switch (field->m_size) {
      case 1:
        m_data[field->m_offset + arrIndex] = value;
        break;
      case 2:
        support::endian::write16le(&m_data[field->m_offset + arrIndex * 2],
                                   value);
        break;
      case 4:
        support::endian::write32le(&m_data[field->m_offset + arrIndex * 4],
                                   value);
        break;
      case 8:
        support::endian::write64le(&m_data[field->m_offset + arrIndex * 8],
                                   value);
        break;
      default:
        assert(false && "invalid field size");
    }
    return;
  }

  // handle bit shifting and masking into current value
  switch (field->m_size) {
    case 1: {
      uint8_t* data = &m_data[field->m_offset + arrIndex];
      *data &= ~(field->m_bitMask << field->m_bitShift);
      *data |= (value & field->m_bitMask) << field->m_bitShift;
      break;
    }
    case 2: {
      uint8_t* data = &m_data[field->m_offset + arrIndex * 2];
      uint16_t val = support::endian::read16le(data);
      val &= ~(field->m_bitMask << field->m_bitShift);
      val |= (value & field->m_bitMask) << field->m_bitShift;
      support::endian::write16le(data, val);
      break;
    }
    case 4: {
      uint8_t* data = &m_data[field->m_offset + arrIndex * 4];
      uint32_t val = support::endian::read32le(data);
      val &= ~(field->m_bitMask << field->m_bitShift);
      val |= (value & field->m_bitMask) << field->m_bitShift;
      support::endian::write32le(data, val);
      break;
    }
    case 8: {
      uint8_t* data = &m_data[field->m_offset + arrIndex * 8];
      uint64_t val = support::endian::read64le(data);
      val &= ~(field->m_bitMask << field->m_bitShift);
      val |= (value & field->m_bitMask) << field->m_bitShift;
      support::endian::write64le(data, val);
      break;
    }
    default:
      assert(false && "invalid field size");
  }
}
