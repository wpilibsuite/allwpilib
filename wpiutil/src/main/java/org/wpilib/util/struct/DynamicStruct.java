// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ReadOnlyBufferException;
import java.nio.charset.StandardCharsets;

/** Dynamic (run-time) access to a serialized raw struct. */
public final class DynamicStruct {
  private DynamicStruct(StructDescriptor desc, ByteBuffer data) {
    m_desc = desc;
    m_data = data.order(ByteOrder.LITTLE_ENDIAN);
  }

  /**
   * Constructs a new dynamic struct object with internal storage. The descriptor must be valid. The
   * internal storage is allocated using ByteBuffer.allocate().
   *
   * @param desc struct descriptor
   * @return dynamic struct object
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public static DynamicStruct allocate(StructDescriptor desc) {
    return new DynamicStruct(desc, ByteBuffer.allocate(desc.getSize()));
  }

  /**
   * Constructs a new dynamic struct object with internal storage. The descriptor must be valid. The
   * internal storage is allocated using ByteBuffer.allocateDirect().
   *
   * @param desc struct descriptor
   * @return dynamic struct object
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public static DynamicStruct allocateDirect(StructDescriptor desc) {
    return new DynamicStruct(desc, ByteBuffer.allocateDirect(desc.getSize()));
  }

  /**
   * Constructs a new dynamic struct object. Note: the passed data buffer is not copied.
   * Modifications to the passed buffer will be reflected in the struct and vice-versa.
   *
   * @param desc struct descriptor
   * @param data byte buffer containing serialized data starting at current position
   * @return dynamic struct object
   */
  public static DynamicStruct wrap(StructDescriptor desc, ByteBuffer data) {
    return new DynamicStruct(desc, data.slice());
  }

  /**
   * Gets the struct descriptor.
   *
   * @return struct descriptor
   */
  public StructDescriptor getDescriptor() {
    return m_desc;
  }

  /**
   * Gets the serialized backing data buffer.
   *
   * @return data buffer
   */
  public ByteBuffer getBuffer() {
    return m_data.duplicate().position(0);
  }

  /**
   * Overwrites the entire serialized struct by copying data from a byte array.
   *
   * @param data replacement data for the struct
   * @throws BufferUnderflowException if data is smaller than the struct size
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public void setData(byte[] data) {
    if (data.length < m_desc.getSize()) {
      throw new BufferUnderflowException();
    }
    m_data.position(0).put(data);
  }

  /**
   * Overwrites the entire serialized struct by copying data from a byte buffer.
   *
   * @param data replacement data for the struct; copy starts from current position
   * @throws BufferUnderflowException if remaining data is smaller than the struct size
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public void setData(ByteBuffer data) {
    if (data.remaining() < m_desc.getSize()) {
      throw new BufferUnderflowException();
    }
    int oldLimit = data.limit();
    m_data.position(0).put(data.limit(m_desc.getSize()));
    data.limit(oldLimit);
  }

  /**
   * Gets a struct field descriptor by name.
   *
   * @param name field name
   * @return field descriptor, or null if no field with that name exists
   */
  public StructFieldDescriptor findField(String name) {
    return m_desc.findFieldByName(name);
  }

  /**
   * Gets the value of a boolean field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return boolean field value
   * @throws UnsupportedOperationException if field is not bool type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   */
  public boolean getBoolField(StructFieldDescriptor field, int arrIndex) {
    if (field.getType() != StructFieldType.kBool) {
      throw new UnsupportedOperationException("field is not bool type");
    }
    return getFieldImpl(field, arrIndex) != 0;
  }

  /**
   * Gets the value of a boolean field.
   *
   * @param field field descriptor
   * @return boolean field value
   * @throws UnsupportedOperationException if field is not bool type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public boolean getBoolField(StructFieldDescriptor field) {
    return getBoolField(field, 0);
  }

  /**
   * Sets the value of a boolean field.
   *
   * @param field field descriptor
   * @param value boolean value
   * @param arrIndex array index (must be less than field array size)
   * @throws UnsupportedOperationException if field is not bool type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setBoolField(StructFieldDescriptor field, boolean value, int arrIndex) {
    if (field.getType() != StructFieldType.kBool) {
      throw new UnsupportedOperationException("field is not bool type");
    }
    setFieldImpl(field, value ? 1 : 0, arrIndex);
  }

  /**
   * Sets the value of a boolean field.
   *
   * @param field field descriptor
   * @param value boolean value
   * @throws UnsupportedOperationException if field is not bool type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setBoolField(StructFieldDescriptor field, boolean value) {
    setBoolField(field, value, 0);
  }

  /**
   * Gets the value of an integer field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return integer field value
   * @throws UnsupportedOperationException if field is not integer type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   */
  public long getIntField(StructFieldDescriptor field, int arrIndex) {
    if (!field.isInt() && !field.isUint()) {
      throw new UnsupportedOperationException("field is not integer type");
    }
    return getFieldImpl(field, arrIndex);
  }

  /**
   * Gets the value of an integer field.
   *
   * @param field field descriptor
   * @return integer field value
   * @throws UnsupportedOperationException if field is not integer type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public long getIntField(StructFieldDescriptor field) {
    return getIntField(field, 0);
  }

  /**
   * Sets the value of an integer field.
   *
   * @param field field descriptor
   * @param value integer value
   * @param arrIndex array index (must be less than field array size)
   * @throws UnsupportedOperationException if field is not integer type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setIntField(StructFieldDescriptor field, long value, int arrIndex) {
    if (!field.isInt() && !field.isUint()) {
      throw new UnsupportedOperationException("field is not integer type");
    }
    setFieldImpl(field, value, arrIndex);
  }

  /**
   * Sets the value of an integer field.
   *
   * @param field field descriptor
   * @param value integer value
   * @throws UnsupportedOperationException if field is not integer type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setIntField(StructFieldDescriptor field, long value) {
    setIntField(field, value, 0);
  }

  /**
   * Gets the value of a float field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return float field value
   * @throws UnsupportedOperationException if field is not float type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   */
  public float getFloatField(StructFieldDescriptor field, int arrIndex) {
    if (field.getType() != StructFieldType.kFloat) {
      throw new UnsupportedOperationException("field is not float type");
    }
    return Float.intBitsToFloat((int) getFieldImpl(field, arrIndex));
  }

  /**
   * Gets the value of a float field.
   *
   * @param field field descriptor
   * @return float field value
   * @throws UnsupportedOperationException if field is not float type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public float getFloatField(StructFieldDescriptor field) {
    return getFloatField(field, 0);
  }

  /**
   * Sets the value of a float field.
   *
   * @param field field descriptor
   * @param value float value
   * @param arrIndex array index (must be less than field array size)
   * @throws UnsupportedOperationException if field is not float type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setFloatField(StructFieldDescriptor field, float value, int arrIndex) {
    if (field.getType() != StructFieldType.kFloat) {
      throw new UnsupportedOperationException("field is not float type");
    }
    setFieldImpl(field, Float.floatToIntBits(value), arrIndex);
  }

  /**
   * Sets the value of a float field.
   *
   * @param field field descriptor
   * @param value float value
   * @throws UnsupportedOperationException if field is not float type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setFloatField(StructFieldDescriptor field, float value) {
    setFloatField(field, value, 0);
  }

  /**
   * Gets the value of a double field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return double field value
   * @throws UnsupportedOperationException if field is not double type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   */
  public double getDoubleField(StructFieldDescriptor field, int arrIndex) {
    if (field.getType() != StructFieldType.kDouble) {
      throw new UnsupportedOperationException("field is not double type");
    }
    return Double.longBitsToDouble(getFieldImpl(field, arrIndex));
  }

  /**
   * Gets the value of a double field.
   *
   * @param field field descriptor
   * @return double field value
   * @throws UnsupportedOperationException if field is not double type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public double getDoubleField(StructFieldDescriptor field) {
    return getDoubleField(field, 0);
  }

  /**
   * Sets the value of a double field.
   *
   * @param field field descriptor
   * @param value double value
   * @param arrIndex array index (must be less than field array size)
   * @throws UnsupportedOperationException if field is not double type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setDoubleField(StructFieldDescriptor field, double value, int arrIndex) {
    if (field.getType() != StructFieldType.kDouble) {
      throw new UnsupportedOperationException("field is not double type");
    }
    setFieldImpl(field, Double.doubleToLongBits(value), arrIndex);
  }

  /**
   * Sets the value of a double field.
   *
   * @param field field descriptor
   * @param value double value
   * @throws UnsupportedOperationException if field is not double type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setDoubleField(StructFieldDescriptor field, double value) {
    setDoubleField(field, value, 0);
  }

  /**
   * Gets the value of a character or character array field.
   *
   * @param field field descriptor
   * @return field value
   * @throws UnsupportedOperationException if field is not char type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   */
  @SuppressWarnings({"PMD.CollapsibleIfStatements", "PMD.AvoidDeeplyNestedIfStmts"})
  public String getStringField(StructFieldDescriptor field) {
    if (field.getType() != StructFieldType.kChar) {
      throw new UnsupportedOperationException("field is not char type");
    }
    if (!field.getParent().equals(m_desc)) {
      throw new IllegalArgumentException("field is not part of this struct");
    }
    if (!m_desc.isValid()) {
      throw new IllegalStateException("struct descriptor is not valid");
    }
    byte[] bytes = new byte[field.m_arraySize];
    m_data.position(field.m_offset).get(bytes, 0, field.m_arraySize);
    // Find last non zero character
    int stringLength = bytes.length;
    for (; stringLength > 0; stringLength--) {
      if (bytes[stringLength - 1] != 0) {
        break;
      }
    }
    // If string is all zeroes, its empty and return an empty string.
    if (stringLength == 0) {
      return "";
    }
    // Check if the end of the string is in the middle of a continuation byte or
    // not.
    if ((bytes[stringLength - 1] & 0x80) != 0) {
      // This is a UTF8 continuation byte. Make sure its valid.
      // Walk back until initial byte is found
      int utf8StartByte = stringLength;
      for (; utf8StartByte > 0; utf8StartByte--) {
        if ((bytes[utf8StartByte - 1] & 0x40) != 0) {
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
      byte checkByte = bytes[utf8StartByte];
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

    return new String(bytes, 0, stringLength, StandardCharsets.UTF_8);
  }

  /**
   * Sets the value of a character or character array field.
   *
   * @param field field descriptor
   * @param value field value
   * @return true if the full value fit in the struct, false if truncated
   * @throws UnsupportedOperationException if field is not char type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public boolean setStringField(StructFieldDescriptor field, String value) {
    if (field.getType() != StructFieldType.kChar) {
      throw new UnsupportedOperationException("field is not char type");
    }
    if (!field.getParent().equals(m_desc)) {
      throw new IllegalArgumentException("field is not part of this struct");
    }
    if (!m_desc.isValid()) {
      throw new IllegalStateException("struct descriptor is not valid");
    }
    ByteBuffer bb = StandardCharsets.UTF_8.encode(value);
    int len = Math.min(bb.remaining(), field.m_arraySize);
    boolean copiedFull = len == bb.remaining();
    m_data.position(field.m_offset).put(bb.limit(len));
    for (int i = len; i < field.m_arraySize; i++) {
      m_data.put((byte) 0);
    }
    return copiedFull;
  }

  /**
   * Gets the value of a struct field.
   *
   * @param field field descriptor
   * @param arrIndex array index (must be less than field array size)
   * @return field value
   * @throws UnsupportedOperationException if field is not of struct type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   */
  public DynamicStruct getStructField(StructFieldDescriptor field, int arrIndex) {
    if (field.getType() != StructFieldType.kStruct) {
      throw new UnsupportedOperationException("field is not struct type");
    }
    if (!field.getParent().equals(m_desc)) {
      throw new IllegalArgumentException("field is not part of this struct");
    }
    if (!m_desc.isValid()) {
      throw new IllegalStateException("struct descriptor is not valid");
    }
    if (arrIndex < 0 || arrIndex >= field.m_arraySize) {
      throw new ArrayIndexOutOfBoundsException(
          "arrIndex (" + arrIndex + ") is larger than array size (" + field.m_arraySize + ")");
    }
    StructDescriptor struct = field.getStruct();
    return wrap(struct, m_data.position(field.m_offset + arrIndex * struct.m_size));
  }

  /**
   * Gets the value of a struct field.
   *
   * @param field field descriptor
   * @return field value
   * @throws UnsupportedOperationException if field is not of struct type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   */
  public DynamicStruct getStructField(StructFieldDescriptor field) {
    return getStructField(field, 0);
  }

  /**
   * Sets the value of a struct field.
   *
   * @param field field descriptor
   * @param value struct value
   * @param arrIndex array index (must be less than field array size)
   * @throws UnsupportedOperationException if field is not struct type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ArrayIndexOutOfBoundsException if array index is out of bounds
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setStructField(StructFieldDescriptor field, DynamicStruct value, int arrIndex) {
    if (field.getType() != StructFieldType.kStruct) {
      throw new UnsupportedOperationException("field is not struct type");
    }
    if (!field.getParent().equals(m_desc)) {
      throw new IllegalArgumentException("field is not part of this struct");
    }
    if (!m_desc.isValid()) {
      throw new IllegalStateException("struct descriptor is not valid");
    }
    StructDescriptor struct = field.getStruct();
    if (!value.getDescriptor().equals(struct)) {
      throw new IllegalArgumentException("value's struct type does not match field struct type");
    }
    if (!value.getDescriptor().isValid()) {
      throw new IllegalStateException("value's struct descriptor is not valid");
    }
    if (arrIndex < 0 || arrIndex >= field.m_arraySize) {
      throw new ArrayIndexOutOfBoundsException(
          "arrIndex (" + arrIndex + ") is larger than array size (" + field.m_arraySize + ")");
    }
    m_data
        .position(field.m_offset + arrIndex * struct.m_size)
        .put(value.m_data.position(0).limit(value.getDescriptor().getSize()));
  }

  /**
   * Sets the value of a struct field.
   *
   * @param field field descriptor
   * @param value struct value
   * @throws UnsupportedOperationException if field is not struct type
   * @throws IllegalArgumentException if field is not a member of this struct
   * @throws IllegalStateException if struct descriptor is invalid
   * @throws ReadOnlyBufferException if the underlying buffer is read-only
   */
  public void setStructField(StructFieldDescriptor field, DynamicStruct value) {
    setStructField(field, value, 0);
  }

  private long getFieldImpl(StructFieldDescriptor field, int arrIndex) {
    if (!field.getParent().equals(m_desc)) {
      throw new IllegalArgumentException("field is not part of this struct");
    }
    if (!m_desc.isValid()) {
      throw new IllegalStateException("struct descriptor is not valid");
    }
    if (arrIndex < 0 || arrIndex >= field.m_arraySize) {
      throw new ArrayIndexOutOfBoundsException(
          "arrIndex (" + arrIndex + ") is larger than array size (" + field.m_arraySize + ")");
    }

    long val =
        switch (field.m_size) {
          case 1 -> m_data.get(field.m_offset + arrIndex);
          case 2 -> m_data.getShort(field.m_offset + arrIndex * 2);
          case 4 -> m_data.getInt(field.m_offset + arrIndex * 4);
          case 8 -> m_data.getLong(field.m_offset + arrIndex * 8);
          default -> throw new IllegalStateException("invalid field size");
        };

    if (field.isUint() || field.getType() == StructFieldType.kBool) {
      // for unsigned fields, we can simply logical shift and mask
      return (val >>> field.m_bitShift) & field.getBitMask();
    } else {
      // to get sign extension, shift so the sign bit within the bitfield goes to the long's sign
      // bit (also clearing all higher bits), then shift back down (also clearing all lower bits);
      // since upper and lower bits are cleared with the shifts, the bitmask is unnecessary
      return (val << (64 - field.m_bitShift - field.getBitWidth())) >> (64 - field.getBitWidth());
    }
  }

  private void setFieldImpl(StructFieldDescriptor field, long value, int arrIndex) {
    if (!field.getParent().equals(m_desc)) {
      throw new IllegalArgumentException("field is not part of this struct");
    }
    if (!m_desc.isValid()) {
      throw new IllegalStateException("struct descriptor is not valid");
    }
    if (arrIndex < 0 || arrIndex >= field.m_arraySize) {
      throw new ArrayIndexOutOfBoundsException(
          "arrIndex (" + arrIndex + ") is larger than array size (" + field.m_arraySize + ")");
    }

    // common case is no bit shift and no masking
    if (!field.isBitField()) {
      switch (field.m_size) {
        case 1 -> m_data.put(field.m_offset + arrIndex, (byte) value);
        case 2 -> m_data.putShort(field.m_offset + arrIndex * 2, (short) value);
        case 4 -> m_data.putInt(field.m_offset + arrIndex * 4, (int) value);
        case 8 -> m_data.putLong(field.m_offset + arrIndex * 8, value);
        default -> throw new IllegalStateException("invalid field size");
      }
      return;
    }

    // handle bit shifting and masking into current value
    switch (field.m_size) {
      case 1 -> {
        byte val = m_data.get(field.m_offset + arrIndex);
        val &= (byte) ~(field.getBitMask() << field.m_bitShift);
        val |= (byte) ((value & field.getBitMask()) << field.m_bitShift);
        m_data.put(field.m_offset + arrIndex, val);
      }
      case 2 -> {
        short val = m_data.getShort(field.m_offset + arrIndex * 2);
        val &= (short) ~(field.getBitMask() << field.m_bitShift);
        val |= (short) ((value & field.getBitMask()) << field.m_bitShift);
        m_data.putShort(field.m_offset + arrIndex * 2, val);
      }
      case 4 -> {
        int val = m_data.getInt(field.m_offset + arrIndex * 4);
        val &= (int) ~(field.getBitMask() << field.m_bitShift);
        val |= (int) ((value & field.getBitMask()) << field.m_bitShift);
        m_data.putInt(field.m_offset + arrIndex * 4, val);
      }
      case 8 -> {
        long val = m_data.getLong(field.m_offset + arrIndex * 8);
        val &= ~(field.getBitMask() << field.m_bitShift);
        val |= (value & field.getBitMask()) << field.m_bitShift;
        m_data.putLong(field.m_offset + arrIndex * 8, val);
      }
      default -> throw new IllegalStateException("invalid field size");
    }
  }

  private final StructDescriptor m_desc;
  private final ByteBuffer m_data;
}
