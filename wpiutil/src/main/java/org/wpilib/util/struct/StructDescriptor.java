// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;

/** Raw struct dynamic struct descriptor. */
public class StructDescriptor {
  StructDescriptor(String name) {
    m_name = name;
  }

  /**
   * Gets the struct name.
   *
   * @return name
   */
  public String getName() {
    return m_name;
  }

  /**
   * Gets the struct schema.
   *
   * @return schema
   */
  public String getSchema() {
    return m_schema;
  }

  /**
   * Returns whether the struct is valid (e.g. the struct is fully defined and field offsets
   * computed).
   *
   * @return true if valid
   */
  public boolean isValid() {
    return m_valid;
  }

  /**
   * Returns the struct size, in bytes. Not valid unless IsValid() is true.
   *
   * @return size in bytes
   * @throws IllegalStateException if descriptor is invalid
   */
  public int getSize() {
    if (!m_valid) {
      throw new IllegalStateException("descriptor is invalid");
    }
    return m_size;
  }

  /**
   * Gets a field descriptor by name. Note the field cannot be accessed until the struct is valid.
   *
   * @param name field name
   * @return field descriptor, or nullptr if not found
   */
  public StructFieldDescriptor findFieldByName(String name) {
    return m_fieldsByName.get(name);
  }

  /**
   * Gets all field descriptors. Note fields cannot be accessed until the struct is valid.
   *
   * @return field descriptors
   */
  public List<StructFieldDescriptor> getFields() {
    return m_fields;
  }

  boolean checkCircular(Stack<StructDescriptor> stack) {
    stack.push(this);
    for (StructDescriptor ref : m_references) {
      if (stack.contains(ref)) {
        return false;
      }
      if (!ref.checkCircular(stack)) {
        return false;
      }
    }
    stack.pop();
    return true;
  }

  void calculateOffsets(Stack<StructDescriptor> stack) {
    int offset = 0;
    int shift = 0;
    int prevBitfieldSize = 0;
    for (StructFieldDescriptor field : m_fields) {
      if (!field.isBitField()) {
        shift = 0; // reset shift on non-bitfield element
        offset += prevBitfieldSize; // finish bitfield if active
        prevBitfieldSize = 0; // previous is now not bitfield
        field.m_offset = offset;
        StructDescriptor struct = field.getStruct();
        if (struct != null) {
          if (!struct.isValid()) {
            m_valid = false;
            return;
          }
          field.m_size = struct.m_size;
        }
        offset += field.m_size * field.m_arraySize;
      } else {
        int bitWidth = field.getBitWidth();
        if (field.getType() == StructFieldType.kBool
            && prevBitfieldSize != 0
            && (shift + 1) <= (prevBitfieldSize * 8)) {
          // bool takes on size of preceding bitfield type (if it fits)
          field.m_size = prevBitfieldSize;
        } else if (field.m_size != prevBitfieldSize || (shift + bitWidth) > (field.m_size * 8)) {
          shift = 0;
          offset += prevBitfieldSize;
        }
        prevBitfieldSize = field.m_size;
        field.m_offset = offset;
        field.m_bitShift = shift;
        shift += bitWidth;
      }
    }

    // update struct size
    m_size = offset + prevBitfieldSize;
    m_valid = true;

    // now that we're valid, referring types may be too
    stack.push(this);
    for (StructDescriptor ref : m_references) {
      if (stack.contains(ref)) {
        throw new IllegalStateException(
            "internal error (inconsistent data): circular struct reference between "
                + m_name
                + " and "
                + ref.m_name);
      }
      ref.calculateOffsets(stack);
    }
    stack.pop();
  }

  private final String m_name;
  String m_schema;
  final Set<StructDescriptor> m_references = new HashSet<>();
  final List<StructFieldDescriptor> m_fields = new ArrayList<>();
  final Map<String, StructFieldDescriptor> m_fieldsByName = new HashMap<>();
  int m_size;
  boolean m_valid;
}
