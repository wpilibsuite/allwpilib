package edu.wpi.first.util.struct;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.lang.reflect.RecordComponent;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Optional;

public final class ProceduralStructifier {
    @FunctionalInterface
    private interface Unpacker<T> {
        T unpack(ByteBuffer buffer);
    }

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

    private record PrimType<T>(
            String name,
            int size,
            Unpacker<T> unpacker,
            Packer<T> packer) {
    }

    /**
     * A map of primitive types to their schema types.
     */
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

    // Add boxed primitive types to the map
    static {
        addPrimType(Integer.class, int.class, "int32", Integer.BYTES, ByteBuffer::getInt, ByteBuffer::putInt);
        addPrimType(Double.class, double.class, "float64", Double.BYTES, ByteBuffer::getDouble, ByteBuffer::putDouble);
        addPrimType(Float.class, float.class, "float32", Float.BYTES, ByteBuffer::getFloat, ByteBuffer::putFloat);
        addPrimType(Boolean.class, boolean.class, "bool", Byte.BYTES, (buffer) -> buffer.get() != 0,
                (buffer, value) -> buffer.put((byte) (value ? 1 : 0)));
        addPrimType(Character.class, char.class, "char", Character.BYTES, ByteBuffer::getChar, ByteBuffer::putChar);
        addPrimType(Byte.class, byte.class, "uint8", Byte.BYTES, ByteBuffer::get, ByteBuffer::put);
        addPrimType(Short.class, short.class, "int16", Short.BYTES, ByteBuffer::getShort, ByteBuffer::putShort);
        addPrimType(Long.class, long.class, "int64", Long.BYTES, ByteBuffer::getLong, ByteBuffer::putLong);
    }

    /**
     * A map of types to their custom struct schemas.
     * 
     * This allows adding custom struct implementations for types that are not
     * supported by default. Think of vendor-specific.
     */
    private static final HashMap<Class<?>, Struct<?>> customStructTypeMap = new HashMap<>();

    /**
     * Add a custom struct to the structifier.
     * 
     * @param <T> The type the struct is for.
     * @param clazz The class of the type.
     * @param struct The struct to add.
     * @param override Whether to override an existing struct. An existing
     *                 struct could mean the type already has a {@code struct} field
     *                 and implemnts {@link StructSerializable} or that the type is
     *                 already in the custom struct map.
     */
    public static <T> void addCustomStruct(Class<T> clazz, Struct<T> struct, boolean override) {
        if (override) {
            customStructTypeMap.put(clazz, struct);
        } else if (!StructSerializable.class.isAssignableFrom(clazz)) {
            customStructTypeMap.putIfAbsent(clazz, struct);
        }
    }

    /**
     * Returns a {@link Struct} for the given {@link StructSerializable} marked class.
     * Due to the non-contractual nature of the marker this can fail.
     * If the {@code struct} field could not be accessed for any reason, an empty
     * {@link Optional} is returned.
     * 
     * @param <T> The type of the class.
     * @param clazz The class object to extract the struct from.
     * @return An optional containing the struct if it could be extracted.
     */
    @SuppressWarnings("unchecked")
    public static <T extends StructSerializable> Optional<Struct<T>> extractClassStruct(Class<T> clazz) {
        try {
            var possibleField = Optional.ofNullable(clazz.getDeclaredField("struct"));
            return possibleField.flatMap(field -> {
                if (Struct.class.isAssignableFrom(field.getType())) {
                    try {
                        return Optional.ofNullable((Struct<T>) field.get(null));
                    } catch (IllegalAccessException e) {
                        return Optional.empty();
                    }
                } else {
                    return Optional.empty();
                }
            });
        } catch (NoSuchFieldException e) {
            return Optional.empty();
        }
    }

    /**
     * Returns a {@link Struct} for the given class.
     * This does not do compile time checking that the class is a {@link StructSerializable}.
     * Whenever possible it is reccomended to use {@link #extractClassStruct(Class)}.
     * 
     * @param clazz The class object to extract the struct from.
     * @return An optional containing the struct if it could be extracted.
     */
    @SuppressWarnings("unchecked")
    public static Optional<Struct<?>> extractClassStructDynamic(Class<?> clazz) {
        if (StructSerializable.class.isAssignableFrom(clazz)) {
            return extractClassStruct((Class<? extends StructSerializable>) clazz)
                    .map(struct -> (Struct<?>) struct);
        } else {
            return Optional.empty();
        }
    }

    /**
     * A utility for building schema syntax in a procedural manner.
     */
    public static class SchemaBuilder {
        /**
         * A utility for building enum fields in a procedural manner.
         */
        public static final class EnumFieldBuilder {
            private final StringBuilder enumFieldBuilder = new StringBuilder();
            private final String fieldName;
            private int greatestValue = 0;
            private int lowestValue = 0;
            private boolean firstVariant = true;

            public EnumFieldBuilder(String fieldName) {
                this.fieldName = fieldName;
                enumFieldBuilder.append("enum {");
            }

            /**
             * Adds a variant to the enum field.
             * 
             * @param name The name of the variant.
             * @param value The value of the variant.
             * @return The builder for chaining.
             */
            public EnumFieldBuilder addVariant(String name, int value) {
                if (!firstVariant) {
                    enumFieldBuilder.append(",");
                }
                firstVariant = false;
                enumFieldBuilder.append(name).append("=").append(value);
                greatestValue = Math.max(greatestValue, value);
                lowestValue = Math.min(lowestValue, value);
                return this;
            }

            /**
             * Builds the enum field.
             * If this object is being used with {@link SchemaBuilder#addEnumField}
             * then {@link #build()} does not have to be called by the user.
             * 
             * @return The built enum field.
             */
            public String build() {
                enumFieldBuilder.append("} ")
                        .append("int8 ")
                        .append(fieldName)
                        .append(';');
                return enumFieldBuilder.toString();
            }
        }

        private final StringBuilder schemaBuilder = new StringBuilder();

        /**
         * Adds a field to the schema.
         * 
         * @param name The name of the field.
         * @param type The type of the field.
         * @return The builder for chaining.
         */
        public SchemaBuilder addField(String name, String type) {
            schemaBuilder.append(type).append(' ').append(name).append(';');
            return this;
        }

        /**
         * Adds an inline enum field to the schema.
         * 
         * @param enumFieldBuilder The builder for the enum field.
         * @return The builder for chaining.
         */
        public SchemaBuilder addEnumField(EnumFieldBuilder enumFieldBuilder) {
            schemaBuilder.append(enumFieldBuilder.build());
            return this;
        }

        /**
         * Builds the schema.
         * 
         * @return The built schema.
         */
        public String build() {
            return schemaBuilder.toString();
        }
    }

    /**
     * A struct that was procedurally generated from a record.
     */
    public interface ProcRecordStruct<T extends Record> extends Struct<T> {
        /**
         * Generates a no-op struct for the given record class.
         * 
         * @param <R> The type of the record.
         * @param recordClass The class of the record.
         * @return The no-op struct.
         */
        public static <R extends Record> ProcRecordStruct<R> noopStruct(Class<R> recordClass) {
            return new ProcRecordStruct<>() {
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
                    return "";
                }

                @Override
                public int getSize() {
                    return 0;
                }

                @Override
                public void pack(ByteBuffer buffer, R value) {
                }

                @Override
                public R unpack(ByteBuffer buffer) {
                    return null;
                }

                @Override
                public boolean isImmutable() {
                    return true;
                }
            };
        }

        /**
         * Generates a struct for the given record class.
         * 
         * @param <R> The type of the record.
         * @param recordClass The class of the record.
         * @return The generated struct.
         */
        @SuppressWarnings("unchecked")
        public static <R extends Record> ProcRecordStruct<R> generate(Class<R> recordClass) {
            final RecordComponent[] components = recordClass.getRecordComponents();
            final SchemaBuilder schemaBuilder = new SchemaBuilder();
            final ArrayList<Struct<?>> nestedStructs = new ArrayList<>();
            final ArrayList<Unpacker<?>> unpackers = new ArrayList<>();
            final ArrayList<Packer<?>> packers = new ArrayList<>();

            int size = 0;
            boolean failed = false;

            for (RecordComponent component : components) {
                final Class<?> type = component.getType();
                final String name = component.getName();

                if (primitiveTypeMap.containsKey(type)) {
                    PrimType<?> primType = primitiveTypeMap.get(type);
                    schemaBuilder.addField(name, primType.name);
                    size += primType.size;
                    unpackers.add(primType.unpacker);
                    packers.add(primType.packer);
                } else if (customStructTypeMap.containsKey(type)) {
                    Struct<?> struct = customStructTypeMap.get(type);
                    schemaBuilder.addField(name, struct.getTypeName());
                    size += struct.getSize();
                    nestedStructs.add(struct);
                    nestedStructs.addAll(List.of(struct.getNested()));
                    unpackers.add(struct::unpack);
                    packers.add(Packer.fromStruct(struct));
                } else if (StructSerializable.class.isAssignableFrom(type)) {
                    var optStruct = extractClassStructDynamic(type);
                    if (optStruct.isPresent()) {
                        Struct<?> struct = optStruct.get();
                        schemaBuilder.addField(name, struct.getTypeName());
                        size += struct.getSize();
                        nestedStructs.add(struct);
                        nestedStructs.addAll(List.of(struct.getNested()));
                        unpackers.add(struct::unpack);
                        packers.add(Packer.fromStruct(struct));
                    } else {
                        System.err.println(
                                "Could not structify record component: "
                                        + recordClass.getSimpleName()
                                        + "#"
                                        + name
                                        + "\n Could not extract struct from marked class: "
                                        + type.getName());
                        failed = true;
                    }
                } else {
                    System.err.println(
                            "Could not structify record component: "
                                    + recordClass.getSimpleName()
                                    + "#"
                                    + name);
                    failed = true;
                }
            }

            if (failed) {
                return noopStruct(recordClass);
            }

            final int frozenSize = size;
            ProcRecordStruct<R> retStruct = new ProcRecordStruct<>() {
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
                    return schemaBuilder.build();
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
                            packer.pack(buffer, components[i].getAccessor().invoke(value));
                        } catch (IllegalAccessException e) {
                            System.err.println(
                                    "Could not pack record component: " + recordClass.getSimpleName()
                                            + "#" + components[i].getName()
                                            + "\n    " + "Illegal Access, Make sure the record is public");
                        } catch (Exception e) {
                            System.err.println(
                                    "Could not pack record component: " + recordClass.getSimpleName()
                                            + "#" + components[i].getName()
                                            + "\n    " + e.getMessage());
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
                        for (int i = 0; i < components.length; i++) {
                            args[i] = unpackers.get(i).unpack(buffer);
                        }
                        return (R) recordClass.getConstructor().newInstance(args);
                    } catch (Exception e) {
                        System.err.println("Could not unpack record: " + recordClass.getSimpleName());
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

            addCustomStruct(recordClass, retStruct, !failed);

            return retStruct;
        }
    }

    /**
     * A struct that was procedurally generated from an enum.
     */
    public interface ProcEnumStruct<T extends Enum<T>> extends Struct<T> {
        /**
         * Generates a no-op struct for the given enum class.
         * This struct will publish no data and will not be able to unpack data.
         * This is useful for when a struct could not be generated.
         * 
         * @param <E> The type of the enum.
         * @param enumClass The class of the enum.
         * @return The no-op struct.
         */
        public static <E extends Enum<E>> ProcEnumStruct<E> noopStruct(Class<E> enumClass) {
            return new ProcEnumStruct<>() {
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
                    return "";
                }

                @Override
                public int getSize() {
                    return 0;
                }

                @Override
                public void pack(ByteBuffer buffer, E value) {
                }

                @Override
                public E unpack(ByteBuffer buffer) {
                    return null;
                }

                @Override
                public boolean isImmutable() {
                    return true;
                }
            };
        }

        /**
         * Generates a struct for the given enum class.
         * 
         * @param <E> The type of the enum.
         * @param enumClass The class of the enum.
         * @return The generated struct.
         */
        @SuppressWarnings("unchecked")
        public static <E extends Enum<E>> ProcEnumStruct<E> generate(Class<E> enumClass) {
            final E[] enumVariants = enumClass.getEnumConstants();
            final Field[] allEnumFields = enumClass.getDeclaredFields();
            final SchemaBuilder schemaBuilder = new SchemaBuilder();
            final SchemaBuilder.EnumFieldBuilder enumFieldBuilder = new SchemaBuilder.EnumFieldBuilder("variant");
            final HashMap<Integer, E> enumMap = new HashMap<>();
            final ArrayList<Packer<?>> packers = new ArrayList<>();

            if (enumVariants == null || enumVariants.length == 0) {
                System.err.println(
                        "Could not structify enum: " + enumClass.getSimpleName()
                        + "\n    " + "Enum has no constants");
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

            final List<Field> enumFields = List.of(allEnumFields)
                .stream()
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
                } else if (customStructTypeMap.containsKey(type)) {
                    Struct<?> struct = customStructTypeMap.get(type);
                    schemaBuilder.addField(name, struct.getTypeName());
                    size += struct.getSize();
                    packers.add(Packer.fromStruct(struct));
                } else if (StructSerializable.class.isAssignableFrom(type)) {
                    var optStruct = extractClassStructDynamic(type);
                    if (optStruct.isPresent()) {
                        Struct<?> struct = optStruct.get();
                        schemaBuilder.addField(name, struct.getTypeName());
                        size += struct.getSize();
                        packers.add(Packer.fromStruct(struct));
                    } else {
                        System.err.println(
                                "Could not structify enum field: "
                                        + enumClass.getSimpleName()
                                        + "#"
                                        + name
                                        + "\n Could not extract struct from marked class: "
                                        + type.getName());
                        failed = true;
                    }
                } else {
                    System.err.println(
                            "Could not structify enum field: "
                                    + enumClass.getSimpleName()
                                    + "#"
                                    + name);
                    failed = true;
                }
            }

            if (failed) {
                return noopStruct(enumClass);
            }

            final int frozenSize = size;
            ProcEnumStruct<E> retStruct = new ProcEnumStruct<>() {
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
                    return schemaBuilder.build();
                }

                @Override
                public int getSize() {
                    return frozenSize;
                }

                @Override
                public void pack(ByteBuffer buffer, E value) {
                    boolean failed = false;
                    int startingPosition = buffer.position();
                    for (int i = 0; i < enumFields.size(); i++) {
                        Packer<Object> packer = (Packer<Object>) packers.get(i);
                        Field field = enumFields.get(i);
                        if (field.isEnumConstant()) {
                            continue;
                        }
                        try {
                            packer.pack(buffer, field.get(value));
                        } catch (IllegalAccessException e) {
                            System.err.println(
                                    "Could not pack enum field: " + enumClass.getSimpleName()
                                            + "#" + field.getName()
                                            + "\n    " + "Illegal Access, Make sure the enum is public");
                        } catch (Exception e) {
                            System.err.println(
                                    "Could not pack enum field: " + enumClass.getSimpleName()
                                            + "#" + field.getName()
                                            + "\n    " + e.getMessage());
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
                public E unpack(ByteBuffer buffer) {
                    try {
                        int ordinal = buffer.getInt();
                        buffer.get(new byte[frozenSize - 1]);
                        return enumMap.get(ordinal);
                    } catch (Exception e) {
                        System.err.println(
                                "Could not unpack enum: " + enumClass.getSimpleName());
                        return null;
                    }
                }

                @Override
                public boolean isImmutable() {
                    return true;
                }
            };

            addCustomStruct(enumClass, retStruct, !failed);

            return retStruct;
        }
    }
}
