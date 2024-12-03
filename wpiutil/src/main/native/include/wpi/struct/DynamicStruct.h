// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cassert>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "wpi/StringMap.h"
#include "wpi/bit.h"

namespace wpi {

template <typename T>
class SmallVectorImpl;

class DynamicStruct;
class MutableDynamicStruct;
class StructDescriptor;
class StructDescriptorDatabase;

/**
 * Known data types for raw struct dynamic fields (see StructFieldDescriptor).
 */
enum class StructFieldType {
  /// bool.
  kBool,
  /// char.
  kChar,
  /// int8.
  kInt8,
  /// int16.
  kInt16,
  /// int32.
  kInt32,
  /// int64.
  kInt64,
  /// uint8.
  kUint8,
  /// uint16.
  kUint16,
  /// uint32.
  kUint32,
  /// uint64.
  kUint64,
  /// float.
  kFloat,
  /// double.
  kDouble,
  /// struct.
  kStruct
};

/**
 * Raw struct dynamic field descriptor.
 */
class StructFieldDescriptor {
  struct private_init {};
  friend class DynamicStruct;
  friend class MutableDynamicStruct;
  friend class StructDescriptor;
  friend class StructDescriptorDatabase;

 public:
  /**
   * Set of enumerated values.
   */
  using EnumValues = std::vector<std::pair<std::string, int64_t>>;

  StructFieldDescriptor(const StructDescriptor* parent, std::string_view name,
                        StructFieldType type, size_t size, size_t arraySize,
                        unsigned int bitWidth, EnumValues enumValues,
                        const StructDescriptor* structDesc,
                        const private_init&);

  /**
   * Gets the dynamic struct this field is contained in.
   *
   * @return struct descriptor
   */
  const StructDescriptor* GetParent() const { return m_parent; }

  /**
   * Gets the field name.
   *
   * @return field name
   */
  const std::string& GetName() const { return m_name; }

  /**
   * Gets the field type.
   *
   * @return field type
   */
  StructFieldType GetType() const { return m_type; }

  /**
   * Returns whether the field type is a signed integer.
   *
   * @return true if signed integer, false otherwise
   */
  bool IsInt() const {
    return m_type == StructFieldType::kInt8 ||
           m_type == StructFieldType::kInt16 ||
           m_type == StructFieldType::kInt32 ||
           m_type == StructFieldType::kInt64;
  }

  /**
   * Returns whether the field type is an unsigned integer.
   *
   * @return true if unsigned integer, false otherwise
   */
  bool IsUint() const {
    return m_type == StructFieldType::kUint8 ||
           m_type == StructFieldType::kUint16 ||
           m_type == StructFieldType::kUint32 ||
           m_type == StructFieldType::kUint64;
  }

  /**
   * Gets the underlying storage size of the field, in bytes.
   *
   * @return number of bytes
   */
  size_t GetSize() const { return m_size; }

  /**
   * Gets the storage offset of the field, in bytes.
   *
   * @return number of bytes from the start of the struct
   */
  size_t GetOffset() const { return m_offset; }

  /**
   * Gets the bit width of the field, in bits.
   *
   * @return number of bits
   */
  unsigned int GetBitWidth() const {
    return m_bitWidth == 0 ? m_size * 8 : m_bitWidth;
  }

  /**
   * Gets the bit mask for the field. The mask is always the least significant
   * bits (it is not shifted).
   *
   * @return bit mask
   */
  uint64_t GetBitMask() const { return m_bitMask; }

  /**
   * Gets the bit shift for the field (LSB=0).
   *
   * @return number of bits
   */
  unsigned int GetBitShift() const { return m_bitShift; }

  /**
   * Returns whether the field is an array.
   *
   * @return true if array
   */
  bool IsArray() const { return m_arraySize > 1; }

  /**
   * Gets the array size. Returns 1 if non-array.
   *
   * @return number of elements
   */
  size_t GetArraySize() const { return m_arraySize; }

  /**
   * Returns whether the field has enumerated values.
   *
   * @return true if there are enumerated values
   */
  bool HasEnum() const { return !m_enum.empty(); }

  /**
   * Gets the enumerated values.
   *
   * @return set of enumerated values
   */
  const EnumValues& GetEnumValues() { return m_enum; }

  /**
   * Gets the struct descriptor for a struct data type.
   *
   * @return struct descriptor; returns null for non-struct
   */
  const StructDescriptor* GetStruct() const { return m_struct; }

  /**
   * Gets the minimum unsigned integer value that can be stored in this field.
   *
   * @return minimum value
   */
  uint64_t GetUintMin() const { return 0; }

  /**
   * Gets the maximum unsigned integer value that can be stored in this field.
   *
   * @return maximum value
   */
  uint64_t GetUintMax() const { return m_bitMask; }

  /**
   * Gets the minimum signed integer value that can be stored in this field.
   *
   * @return minimum value
   */
  int64_t GetIntMin() const {
    return static_cast<int64_t>(-(m_bitMask >> 1)) - 1;
  }

  /**
   * Gets the maximum signed integer value that can be stored in this field.
   *
   * @return maximum value
   */
  int64_t GetIntMax() const { return m_bitMask >> 1; }

  /**
   * Returns whether the field is a bitfield.
   *
   * @return true if bitfield
   */
  bool IsBitField() const {
    return (m_bitShift != 0 || m_bitWidth != (m_size * 8)) &&
           m_struct == nullptr;
  }

 private:
  // note: constructor fills in everything except offset and shift
  const StructDescriptor* m_parent;
  std::string m_name;
  size_t m_size;
  size_t m_offset = 0;
  size_t m_arraySize;  // 1 for non-arrays
  EnumValues m_enum;
  const StructDescriptor* m_struct;  // nullptr for non-structs
  uint64_t m_bitMask;
  StructFieldType m_type;
  unsigned int m_bitWidth;
  unsigned int m_bitShift = 0;
};

/**
 * Raw struct dynamic struct descriptor.
 */
class StructDescriptor {
  struct private_init {};
  friend class StructDescriptorDatabase;

 public:
  StructDescriptor(std::string_view name, const private_init&) : m_name{name} {}

  /**
   * Gets the struct name.
   *
   * @return name
   */
  const std::string& GetName() const { return m_name; }

  /**
   * Gets the struct schema.
   *
   * @return schema
   */
  const std::string& GetSchema() const { return m_schema; }

  /**
   * Returns whether the struct is valid (e.g. the struct is fully defined and
   * field offsets computed).
   *
   * @return true if valid
   */
  bool IsValid() const { return m_valid; }

  /**
   * Returns the struct size, in bytes. Not valid unless IsValid() is true.
   *
   * @return size in bytes
   */
  size_t GetSize() const {
    assert(m_valid);
    return m_size;
  }

  /**
   * Gets a field descriptor by name. Note the field cannot be accessed until
   * the struct is valid.
   *
   * @param name field name
   * @return field descriptor, or nullptr if not found
   */
  const StructFieldDescriptor* FindFieldByName(std::string_view name) const;

  /**
   * Gets all field descriptors. Note fields cannot be accessed until the struct
   * is valid.
   *
   * @return field descriptors
   */
  const std::vector<StructFieldDescriptor>& GetFields() const {
    return m_fields;
  }

 private:
  bool CheckCircular(
      wpi::SmallVectorImpl<const StructDescriptor*>& stack) const;
  std::string CalculateOffsets(
      wpi::SmallVectorImpl<const StructDescriptor*>& stack);

  std::string m_name;
  std::string m_schema;
  std::vector<StructDescriptor*> m_references;
  std::vector<StructFieldDescriptor> m_fields;
  StringMap<size_t> m_fieldsByName;
  size_t m_size = 0;
  bool m_valid = false;
};

/**
 * Database of raw struct dynamic descriptors.
 */
class StructDescriptorDatabase {
 public:
  /**
   * Adds a structure schema to the database. If the struct references other
   * structs that have not yet been added, it will not be valid until those
   * structs are also added.
   *
   * @param[in] name structure name
   * @param[in] schema structure schema
   * @param[out] err detailed error, if nullptr is returned
   * @return Added struct, or nullptr on error
   */
  const StructDescriptor* Add(std::string_view name, std::string_view schema,
                              std::string* err);

  /**
   * Finds a structure in the database by name.
   *
   * @param name structure name
   * @return struct descriptor, or nullptr if not found
   */
  const StructDescriptor* Find(std::string_view name) const;

 private:
  StringMap<StructDescriptor> m_structs;
};

/**
 * Dynamic (run-time) read-only access to a serialized raw struct.
 */
class DynamicStruct {
 public:
  /**
   * Constructs a new dynamic struct. Note: the passed data is a span; no copy
   * is made, so it's necessary for the lifetime of the referenced data to be
   * longer than this object.
   *
   * @param desc struct descriptor
   * @param data serialized data
   */
  DynamicStruct(const StructDescriptor* desc, std::span<const uint8_t> data)
      : m_desc{desc}, m_data{data} {}

  /**
   * Gets the struct descriptor.
   *
   * @return struct descriptor
   */
  const StructDescriptor* GetDescriptor() const { return m_desc; }

  /**
   * Gets the serialized data.
   *
   * @return data
   */
  std::span<const uint8_t> GetData() const { return m_data; }

  /**
   * Gets a struct field descriptor by name.
   *
   * @param name field name
   * @return field descriptor, or nullptr if no field with that name exists
   */
  const StructFieldDescriptor* FindField(std::string_view name) const {
    return m_desc->FindFieldByName(name);
  }

  /**
   * Gets the value of a boolean field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   */
  bool GetBoolField(const StructFieldDescriptor* field,
                    size_t arrIndex = 0) const {
    assert(field->m_type == StructFieldType::kBool);
    return GetFieldImpl(field, arrIndex);
  }

  /**
   * Gets the value of a signed integer field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   */
  int64_t GetIntField(const StructFieldDescriptor* field,
                      size_t arrIndex = 0) const {
    assert(field->IsInt());
    uint64_t raw = GetFieldImpl(field, arrIndex);
    switch (field->m_size) {
      case 1:
        return static_cast<int8_t>(raw);
      case 2:
        return static_cast<int16_t>(raw);
      case 4:
        return static_cast<int32_t>(raw);
      default:
        return raw;
    }
  }

  /**
   * Gets the value of an unsigned integer field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   */
  uint64_t GetUintField(const StructFieldDescriptor* field,
                        size_t arrIndex = 0) const {
    assert(field->IsUint());
    return GetFieldImpl(field, arrIndex);
  }

  /**
   * Gets the value of a float field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   */
  float GetFloatField(const StructFieldDescriptor* field,
                      size_t arrIndex = 0) const {
    assert(field->m_type == StructFieldType::kFloat);
    return bit_cast<float>(
        static_cast<uint32_t>(GetFieldImpl(field, arrIndex)));
  }

  /**
   * Gets the value of a double field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   */
  double GetDoubleField(const StructFieldDescriptor* field,
                        size_t arrIndex = 0) const {
    assert(field->m_type == StructFieldType::kDouble);
    return bit_cast<double>(GetFieldImpl(field, arrIndex));
  }

  /**
   * Gets the value of a char or char array field.
   *
   * @param field field descriptor
   * @return field value
   */
  std::string_view GetStringField(const StructFieldDescriptor* field) const;

  /**
   * Gets the value of a struct field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   */
  DynamicStruct GetStructField(const StructFieldDescriptor* field,
                               size_t arrIndex = 0) const {
    assert(field->m_type == StructFieldType::kStruct);
    assert(field->m_parent == m_desc);
    assert(m_desc->IsValid());
    assert(arrIndex < field->m_arraySize);
    return DynamicStruct{field->m_struct,
                         m_data.subspan(field->m_offset +
                                        arrIndex * field->m_struct->GetSize())};
  }

 protected:
  const StructDescriptor* m_desc;

 private:
  uint64_t GetFieldImpl(const StructFieldDescriptor* field,
                        size_t arrIndex) const;

  std::span<const uint8_t> m_data;
};

/**
 * Dynamic (run-time) mutable access to a serialized raw struct.
 */
class MutableDynamicStruct : public DynamicStruct {
 public:
  /**
   * Constructs a new dynamic struct. Note: the passed data is a span; no copy
   * is made, so it's necessary for the lifetime of the referenced data to be
   * longer than this object.
   *
   * @param desc struct descriptor
   * @param data serialized data
   */
  MutableDynamicStruct(const StructDescriptor* desc, std::span<uint8_t> data)
      : DynamicStruct{desc, data}, m_data{data} {}

  /**
   * Gets the serialized data.
   *
   * @return data
   */
  std::span<uint8_t> GetData() { return m_data; }

  using DynamicStruct::GetData;

  /**
   * Overwrites the entire serialized struct by copying data from a span.
   *
   * @param data replacement data for the struct
   */
  void SetData(std::span<const uint8_t> data);

  /**
   * Sets the value of a boolean field.
   *
   * @param field field descriptor
   * @param value field value
   * @param arrIndex array index (must be less than field array size)
   */
  void SetBoolField(const StructFieldDescriptor* field, bool value,
                    size_t arrIndex = 0) {
    assert(field->m_type == StructFieldType::kBool);
    SetFieldImpl(field, value ? 1 : 0, arrIndex);
  }

  /**
   * Sets the value of a signed integer field.
   *
   * @param field field descriptor
   * @param value field value
   * @param arrIndex array index (must be less than field array size)
   */
  void SetIntField(const StructFieldDescriptor* field, int64_t value,
                   size_t arrIndex = 0) {
    assert(field->IsInt());
    SetFieldImpl(field, value, arrIndex);
  }

  /**
   * Sets the value of an unsigned integer field.
   *
   * @param field field descriptor
   * @param value field value
   * @param arrIndex array index (must be less than field array size)
   */
  void SetUintField(const StructFieldDescriptor* field, uint64_t value,
                    size_t arrIndex = 0) {
    assert(field->IsUint());
    SetFieldImpl(field, value, arrIndex);
  }

  /**
   * Sets the value of a float field.
   *
   * @param field field descriptor
   * @param value field value
   * @param arrIndex array index (must be less than field array size)
   */
  void SetFloatField(const StructFieldDescriptor* field, float value,
                     size_t arrIndex = 0) {
    assert(field->m_type == StructFieldType::kFloat);
    SetFieldImpl(field, bit_cast<uint32_t>(value), arrIndex);
  }

  /**
   * Sets the value of a double field.
   *
   * @param field field descriptor
   * @param value field value
   * @param arrIndex array index (must be less than field array size)
   */
  void SetDoubleField(const StructFieldDescriptor* field, double value,
                      size_t arrIndex = 0) {
    assert(field->m_type == StructFieldType::kDouble);
    SetFieldImpl(field, bit_cast<uint64_t>(value), arrIndex);
  }

  /**
   * Sets the value of a char or char array field.
   *
   * @param field field descriptor
   * @param value field value
   * @return true if the full value fit in the struct, false if truncated
   */
  bool SetStringField(const StructFieldDescriptor* field,
                      std::string_view value);

  /**
   * Sets the value of a struct field.
   *
   * @param field field descriptor
   * @param value field value
   * @param arrIndex array index (must be less than field array size)
   */
  void SetStructField(const StructFieldDescriptor* field,
                      const DynamicStruct& value, size_t arrIndex = 0);

  /**
   * Gets the value of a struct field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   */
  MutableDynamicStruct GetStructField(const StructFieldDescriptor* field,
                                      size_t arrIndex = 0) {
    assert(field->m_type == StructFieldType::kStruct);
    assert(field->m_parent == m_desc);
    assert(m_desc->IsValid());
    assert(arrIndex < field->m_arraySize);
    return MutableDynamicStruct{
        field->m_struct, m_data.subspan(field->m_offset +
                                        arrIndex * field->m_struct->GetSize())};
  }

  using DynamicStruct::GetStructField;

 private:
  void SetFieldImpl(const StructFieldDescriptor* field, uint64_t value,
                    size_t arrIndex);

  std::span<uint8_t> m_data;
};

namespace impl {
struct DSOData {
  explicit DSOData(size_t size) : m_dataStore(size) {}
  explicit DSOData(std::span<const uint8_t> data)
      : m_dataStore{data.begin(), data.end()} {}

  std::vector<uint8_t> m_dataStore;
};
}  // namespace impl

/**
 * Dynamic (run-time) mutable access to a serialized raw struct, with internal
 * data storage.
 */
class DynamicStructObject : private impl::DSOData, public MutableDynamicStruct {
  /**
   * Constructs a new dynamic struct object. The descriptor must be valid.
   *
   * @param desc struct descriptor
   */
  explicit DynamicStructObject(const StructDescriptor* desc)
      : DSOData{desc->GetSize()}, MutableDynamicStruct{desc, m_dataStore} {}

  /**
   * Constructs a new dynamic struct object. Makes a copy of the serialized
   * data so there are no lifetime constraints on the data parameter.
   *
   * @param desc struct descriptor
   * @param data serialized data
   */
  DynamicStructObject(const StructDescriptor* desc,
                      std::span<const uint8_t> data)
      : DSOData{data}, MutableDynamicStruct{desc, m_dataStore} {
    assert(data.size() >= desc->GetSize());
  }

  // can't be movable due to span references
  DynamicStructObject(DynamicStructObject&&) = delete;
  DynamicStructObject& operator=(DynamicStructObject&&) = delete;
};

}  // namespace wpi
