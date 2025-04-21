// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Modifier;
import java.lang.reflect.RecordComponent;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/** A utility class for procedurally generating {@link Struct}s from records and enums. */
public final class StructGenerator {
  private StructGenerator() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * A functional interface representing a method that retrives a value from a {@link ByteBuffer}.
   */
  @FunctionalInterface
  private interface Unpacker<T> {
    T unpack(ByteBuffer buffer);
  }

  /** A functional interface representing a method that packs a value into a {@link ByteBuffer}. */
  @FunctionalInterface
  private interface Packer<T> {
    ByteBuffer pack(ByteBuffer buffer, T value);

    static <T> Packer<T> fromStruct(Struct<T> struct) {
      return (buffer, value) -> {
        struct.pack(buffer, value);
        return buffer;
      };
    }
  }

  private record PrimType<T>(String name, int size, Unpacker<T> unpacker, Packer<T> packer) {}

  /** A map of primitive types to their schema types. */
  private static final HashMap<Class<?>, PrimType<?>> primitiveTypeMap = new HashMap<>();

  private static <T> void addPrimType(
      Class<T> boxedClass,
      Class<T> primitiveClass,
      String name,
      int size,
      Unpacker<T> unpacker,
      Packer<T> packer) {
    PrimType<T> primType = new PrimType<>(name, size, unpacker, packer);
    primitiveTypeMap.put(boxedClass, primType);
    primitiveTypeMap.put(primitiveClass, primType);
  }

  // Add primitive types to the map
  static {
    addPrimType(
        Integer.class, int.class, "int32", Integer.BYTES, ByteBuffer::getInt, ByteBuffer::putInt);
    addPrimType(
        Double.class,
        double.class,
        "float64",
        Double.BYTES,
        ByteBuffer::getDouble,
        ByteBuffer::putDouble);
    addPrimType(
        Float.class,
        float.class,
        "float32",
        Float.BYTES,
        ByteBuffer::getFloat,
        ByteBuffer::putFloat);
    addPrimType(
        Boolean.class,
        boolean.class,
        "bool",
        Byte.BYTES,
        buffer -> buffer.get() != 0,
        (buffer, value) -> buffer.put((byte) (value ? 1 : 0)));
    addPrimType(
        Character.class,
        char.class,
        "char",
        Character.BYTES,
        ByteBuffer::getChar,
        ByteBuffer::putChar);
    addPrimType(Byte.class, byte.class, "uint8", Byte.BYTES, ByteBuffer::get, ByteBuffer::put);
    addPrimType(
        Short.class, short.class, "int16", Short.BYTES, ByteBuffer::getShort, ByteBuffer::putShort);
    addPrimType(
        Long.class, long.class, "int64", Long.BYTES, ByteBuffer::getLong, ByteBuffer::putLong);
  }

  /**
   * A map of types to their custom struct schemas.
   *
   * <p>This allows adding custom struct implementations for types that are not supported by
   * default. Think of vendor-specific.
   */
  private static final HashMap<Class<?>, Struct<?>> customStructTypeMap = new HashMap<>();

  /**
   * Add a custom struct to the structifier.
   *
   * @param <T> The type the struct is for.
   * @param clazz The class of the type.
   * @param struct The struct to add.
   * @param override Whether to override an existing struct. An existing struct could mean the type
   *     already has a {@code struct} field and implemnts {@link StructSerializable} or that the
   *     type is already in the custom struct map.
   */
  public static <T> void addCustomStruct(Class<T> clazz, Struct<T> struct, boolean override) {
    if (override) {
      customStructTypeMap.put(clazz, struct);
    } else if (!StructSerializable.class.isAssignableFrom(clazz)) {
      customStructTypeMap.putIfAbsent(clazz, struct);
    }
  }

  /** A utility for building schema syntax in a procedural manner. */
  @SuppressWarnings("PMD.AvoidStringBufferField")
  public static class SchemaBuilder {
    /** A utility for building enum fields in a procedural manner. */
    public static final class EnumFieldBuilder {
      private final StringBuilder m_builder = new StringBuilder();
      private final String m_fieldName;
      private boolean m_firstVariant = true;

      /**
       * Creates a new enum field builder.
       *
       * @param fieldName The name of the field.
       */
      public EnumFieldBuilder(String fieldName) {
        this.m_fieldName = fieldName;
        m_builder.append("enum {");
      }

      /**
       * Adds a variant to the enum field.
       *
       * @param name The name of the variant.
       * @param value The value of the variant.
       * @return The builder for chaining.
       */
      public EnumFieldBuilder addVariant(String name, int value) {
        if (!m_firstVariant) {
          m_builder.append(',');
        }
        m_firstVariant = false;
        m_builder.append(name).append('=').append(value);
        return this;
      }

      /**
       * Builds the enum field. If this object is being used with {@link SchemaBuilder#addEnumField}
       * then {@link #build()} does not have to be called by the user.
       *
       * @return The built enum field.
       */
      public String build() {
        m_builder.append("} int8 ").append(m_fieldName).append(';');
        return m_builder.toString();
      }
    }

    /** Creates a new schema builder. */
    public SchemaBuilder() {}

    private final StringBuilder m_builder = new StringBuilder();

    /**
     * Adds a field to the schema.
     *
     * @param name The name of the field.
     * @param type The type of the field.
     * @return The builder for chaining.
     */
    public SchemaBuilder addField(String name, String type) {
      m_builder.append(type).append(' ').append(name).append(';');
      return this;
    }

    /**
     * Adds an inline enum field to the schema.
     *
     * @param enumFieldBuilder The builder for the enum field.
     * @return The builder for chaining.
     */
    public SchemaBuilder addEnumField(EnumFieldBuilder enumFieldBuilder) {
      m_builder.append(enumFieldBuilder.build());
      return this;
    }

    /**
     * Builds the schema.
     *
     * @return The built schema.
     */
    public String build() {
      return m_builder.toString();
    }
  }

  private static <T> Struct<T> noopStruct(Class<T> cls) {
    return new Struct<>() {
      @Override
      public Class<T> getTypeClass() {
        return cls;
      }

      @Override
      public String getTypeName() {
        return cls.getSimpleName();
      }

      @Override
      public String getSchema() {
        return "";
      }

      @Override
      public int getSize() {
        return 0;
      }

      @Override
      public void pack(ByteBuffer buffer, T value) {}

      @Override
      public T unpack(ByteBuffer buffer) {
        return null;
      }

      @Override
      public boolean isImmutable() {
        return true;
      }
    };
  }

  /**
   * Generates a {@link Struct} for the given {@link Record} class. If a {@link Struct} cannot be
   * generated from the {@link Record}, the errors encountered will be printed and a no-op {@link
   * Struct} will be returned.
   *
   * @param <R> The type of the record.
   * @param recordClass The class of the record.
   * @return The generated struct.
   */
  @SuppressWarnings({"unchecked", "PMD.AvoidAccessibilityAlteration"})
  public static <R extends Record> Struct<R> genRecord(final Class<R> recordClass) {
    final RecordComponent[] components = recordClass.getRecordComponents();
    final SchemaBuilder schemaBuilder = new SchemaBuilder();
    final ArrayList<Struct<?>> nestedStructs = new ArrayList<>();
    final ArrayList<Unpacker<?>> unpackers = new ArrayList<>();
    final ArrayList<Packer<?>> packers = new ArrayList<>();

    int size = 0;
    boolean failed = false;

    for (final RecordComponent component : components) {
      final Class<?> type = component.getType();
      final String name = component.getName();
      component.getAccessor().setAccessible(true);

      if (primitiveTypeMap.containsKey(type)) {
        PrimType<?> primType = primitiveTypeMap.get(type);
        schemaBuilder.addField(name, primType.name);
        size += primType.size;
        unpackers.add(primType.unpacker);
        packers.add(primType.packer);
      } else {
        Struct<?> struct;
        if (customStructTypeMap.containsKey(type)) {
          struct = customStructTypeMap.get(type);
        } else if (StructSerializable.class.isAssignableFrom(type)) {
          var optStruct = StructFetcher.fetchStructDynamic(type);
          if (optStruct.isPresent()) {
            struct = optStruct.get();
          } else {
            System.err.println(
                "Could not structify record component: "
                    + recordClass.getSimpleName()
                    + "#"
                    + name
                    + "\n    Could not extract struct from marked class: "
                    + type.getName());
            failed = true;
            continue;
          }
        } else {
          System.err.println(
              "Could not structify record component: " + recordClass.getSimpleName() + "#" + name);
          failed = true;
          continue;
        }
        schemaBuilder.addField(name, struct.getTypeName());
        size += struct.getSize();
        nestedStructs.add(struct);
        nestedStructs.addAll(List.of(struct.getNested()));
        unpackers.add(struct::unpack);
        packers.add(Packer.fromStruct(struct));
      }
    }

    if (failed) {
      return noopStruct(recordClass);
    }

    final int frozenSize = size;
    final String schema = schemaBuilder.build();
    return new Struct<>() {
      @Override
      public Class<R> getTypeClass() {
        return recordClass;
      }

      @Override
      public String getTypeName() {
        return recordClass.getSimpleName();
      }

      @Override
      public String getSchema() {
        return schema;
      }

      @Override
      public int getSize() {
        return frozenSize;
      }

      @Override
      public void pack(ByteBuffer buffer, R value) {
        boolean failed = false;
        int startingPosition = buffer.position();
        for (int i = 0; i < components.length; i++) {
          Packer<Object> packer = (Packer<Object>) packers.get(i);
          try {
            Object componentValue = components[i].getAccessor().invoke(value);
            if (componentValue == null) {
              throw new IllegalArgumentException("Component is null");
            }
            packer.pack(buffer, componentValue);
          } catch (IllegalAccessException
              | IllegalArgumentException
              | InvocationTargetException e) {
            System.err.println(
                "Could not pack record component: "
                    + recordClass.getSimpleName()
                    + "#"
                    + components[i].getName()
                    + "\n    "
                    + e.getMessage());
            failed = true;
            break;
          }
        }
        if (failed) {
          buffer.position(startingPosition);
          for (int i = 0; i < frozenSize; i++) {
            buffer.put((byte) 0);
          }
        }
      }

      @Override
      public R unpack(ByteBuffer buffer) {
        try {
          Object[] args = new Object[components.length];
          Class<?>[] argTypes = new Class<?>[components.length];
          for (int i = 0; i < components.length; i++) {
            args[i] = unpackers.get(i).unpack(buffer);
            argTypes[i] = components[i].getType();
          }
          return recordClass.getConstructor(argTypes).newInstance(args);
        } catch (InstantiationException
            | IllegalAccessException
            | InvocationTargetException
            | NoSuchMethodException
            | SecurityException e) {
          System.err.println(
              "Could not unpack record: "
                  + recordClass.getSimpleName()
                  + "\n    "
                  + e.getMessage());
          return null;
        }
      }

      @Override
      public Struct<?>[] getNested() {
        return nestedStructs.toArray(new Struct<?>[0]);
      }

      @Override
      public boolean isImmutable() {
        return true;
      }
    };
  }

  /**
   * Generates a {@link Struct} for the given {@link Enum} class. If a {@link Struct} cannot be
   * generated from the {@link Enum}, the errors encountered will be printed and a no-op {@link
   * Struct} will be returned.
   *
   * @param <E> The type of the enum.
   * @param enumClass The class of the enum.
   * @return The generated struct.
   */
  @SuppressWarnings({"unchecked", "PMD.AvoidAccessibilityAlteration"})
  public static <E extends Enum<E>> Struct<E> genEnum(Class<E> enumClass) {
    final E[] enumVariants = enumClass.getEnumConstants();
    final Field[] allEnumFields = enumClass.getDeclaredFields();
    final SchemaBuilder schemaBuilder = new SchemaBuilder();
    final SchemaBuilder.EnumFieldBuilder enumFieldBuilder =
        new SchemaBuilder.EnumFieldBuilder("variant");
    final HashMap<Integer, E> enumMap = new HashMap<>();
    final ArrayList<Packer<?>> packers = new ArrayList<>();

    if (enumVariants == null || enumVariants.length == 0) {
      System.err.println(
          "Could not structify enum: "
              + enumClass.getSimpleName()
              + "\n    "
              + "Enum has no constants");
      return noopStruct(enumClass);
    }

    int size = 0;
    boolean failed = false;

    for (final E constant : enumVariants) {
      final String name = constant.name();
      final int ordinal = constant.ordinal();

      enumFieldBuilder.addVariant(name, ordinal);
      enumMap.put(ordinal, constant);
    }
    schemaBuilder.addEnumField(enumFieldBuilder);
    size += 1;

    final List<Field> enumFields =
        List.of(allEnumFields).stream()
            .filter(f -> !f.isEnumConstant() && !Modifier.isStatic(f.getModifiers()))
            .toList();

    for (final Field field : enumFields) {
      final Class<?> type = field.getType();
      final String name = field.getName();
      field.setAccessible(true);

      if (primitiveTypeMap.containsKey(type)) {
        PrimType<?> primType = primitiveTypeMap.get(type);
        schemaBuilder.addField(name, primType.name);
        size += primType.size;
        packers.add(primType.packer);
      } else {
        Struct<?> struct;
        if (customStructTypeMap.containsKey(type)) {
          struct = customStructTypeMap.get(type);
        } else if (StructSerializable.class.isAssignableFrom(type)) {
          var optStruct = StructFetcher.fetchStructDynamic(type);
          if (optStruct.isPresent()) {
            struct = optStruct.get();
          } else {
            System.err.println(
                "Could not structify record component: "
                    + enumClass.getSimpleName()
                    + "#"
                    + name
                    + "\n    Could not extract struct from marked class: "
                    + type.getName());
            failed = true;
            continue;
          }
        } else {
          System.err.println(
              "Could not structify record component: " + enumClass.getSimpleName() + "#" + name);
          failed = true;
          continue;
        }
        schemaBuilder.addField(name, struct.getTypeName());
        size += struct.getSize();
        packers.add(Packer.fromStruct(struct));
      }
    }

    if (failed) {
      return noopStruct(enumClass);
    }

    final int frozenSize = size;
    final String schema = schemaBuilder.build();
    return new Struct<>() {
      @Override
      public Class<E> getTypeClass() {
        return enumClass;
      }

      @Override
      public String getTypeName() {
        return enumClass.getSimpleName();
      }

      @Override
      public String getSchema() {
        return schema;
      }

      @Override
      public int getSize() {
        return frozenSize;
      }

      @Override
      public void pack(ByteBuffer buffer, E value) {
        boolean failed = false;
        int startingPosition = buffer.position();
        buffer.put((byte) value.ordinal());
        for (int i = 0; i < enumFields.size(); i++) {
          Packer<Object> packer = (Packer<Object>) packers.get(i);
          Field field = enumFields.get(i);
          try {
            Object fieldValue = field.get(value);
            if (fieldValue == null) {
              throw new IllegalArgumentException("Field is null");
            }
            packer.pack(buffer, fieldValue);
          } catch (IllegalArgumentException | IllegalAccessException e) {
            System.err.println(
                "Could not pack enum field: "
                    + enumClass.getSimpleName()
                    + "#"
                    + field.getName()
                    + "\n    "
                    + e.getMessage());
            failed = true;
            break;
          }
        }
        if (failed) {
          buffer.position(startingPosition);
          for (int i = 0; i < frozenSize; i++) {
            buffer.put((byte) 0);
          }
        }
      }

      final byte[] m_spongeBuffer = new byte[frozenSize - 1];

      @Override
      public E unpack(ByteBuffer buffer) {
        int ordinal = buffer.get();
        buffer.get(m_spongeBuffer);
        return enumMap.getOrDefault(ordinal, null);
      }

      @Override
      public boolean isImmutable() {
        return true;
      }
    };
  }
}
