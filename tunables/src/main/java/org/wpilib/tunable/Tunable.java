// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.Objects;
import java.util.function.Consumer;
import java.util.function.Supplier;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

/**
 * A tunable value.
 *
 * @param <T> value class
 */
public abstract class Tunable<T> extends TunableBase implements Supplier<T>, Consumer<T> {
  private static final String UNSUPPORTED_TUNABLE_TYPE = "Unsupported tunable type: ";

  /**
   * A tunable value serialized with a {@link Struct}.
   *
   * @param <T> value type
   */
  public static class TunableStruct<T> extends Tunable<T> {
    TunableStruct(T initialValue, Struct<T> struct, TunableConfig config) {
      super(config, true);
      m_struct = struct;
      m_value = initialValue;
    }

    @Override
    public void set(T value) {
      m_value = value;
      markChanged();
    }

    @Override
    public T get() {
      return m_value;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_struct.getTypeClass();
    }

    /**
     * Gets the struct serializer.
     *
     * @return struct serializer
     */
    public Struct<T> getStruct() {
      return m_struct;
    }

    private final Struct<T> m_struct;
    private T m_value;
  }

  /**
   * A tunable array of values serialized with a {@link Struct}.
   *
   * @param <T> array element type
   */
  public static class TunableStructArray<T> extends Tunable<T[]> {
    TunableStructArray(T[] initialValue, Struct<T> struct, Class<T[]> cls, TunableConfig config) {
      super(config, true);
      m_struct = struct;
      m_cls = cls;
      m_value = initialValue == null ? null : initialValue.clone();
    }

    @Override
    public void set(T[] value) {
      m_value = value == null ? null : value.clone();
      markChanged();
    }

    @Override
    public T[] get() {
      return m_value == null ? null : m_value.clone();
    }

    @Override
    @SuppressWarnings("PMD.MethodReturnsInternalArray")
    public T[] mutate() {
      markChanged();
      return m_value;
    }

    @Override
    public Class<T[]> getTypeClass() {
      return m_cls;
    }

    /**
     * Gets the struct serializer.
     *
     * @return struct serializer
     */
    public Struct<T> getStruct() {
      return m_struct;
    }

    private final Struct<T> m_struct;
    private final Class<T[]> m_cls;
    private T[] m_value;
  }

  /**
   * A tunable value serialized with a {@link Protobuf}.
   *
   * @param <T> value type
   */
  public static class TunableProtobuf<T> extends Tunable<T> {
    TunableProtobuf(T initialValue, Protobuf<T, ?> proto, TunableConfig config) {
      super(config, true);
      m_proto = proto;
      m_value = initialValue;
    }

    @Override
    public void set(T value) {
      m_value = value;
      markChanged();
    }

    @Override
    public T get() {
      return m_value;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_proto.getTypeClass();
    }

    /**
     * Gets the protobuf serializer.
     *
     * @return protobuf serializer
     */
    public Protobuf<T, ?> getProtobuf() {
      return m_proto;
    }

    private final Protobuf<T, ?> m_proto;
    private T m_value;
  }

  private abstract static class BasicTunable<T> extends Tunable<T> {
    BasicTunable(Class<T> cls, TunableConfig config) {
      super(config, true);
      m_cls = cls;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_cls;
    }

    private final Class<T> m_cls;
  }

  /** A wrapper that exposes an inner tunable implementation. */
  @FunctionalInterface
  public interface CustomTunable {
    /**
     * Gets the inner tunable.
     *
     * @return inner tunable
     */
    TunableBase getInnerTunable();
  }

  @SuppressWarnings("unchecked")
  private static <T> Class<T> getValueClass(T value) {
    return (Class<T>) value.getClass();
  }

  private static boolean isSupportedBasicType(Class<?> cls) {
    return cls == Boolean.class
        || cls == Integer.class
        || cls == Long.class
        || cls == Float.class
        || cls == Double.class
        || cls == String.class
        || cls == byte[].class
        || cls == boolean[].class
        || cls == int[].class
        || cls == long[].class
        || cls == float[].class
        || cls == double[].class
        || cls == String[].class;
  }

  private static String getTypeName(Class<?> cls) {
    return cls == null ? "<null>" : cls.getName();
  }

  private static void warnIfUnsupportedBasicType(Class<?> cls) {
    if (!isSupportedBasicType(cls)) {
      TunableRegistry.reportWarning(
          UNSUPPORTED_TUNABLE_TYPE + getTypeName(cls) + "; it may not be published by backends");
    }
  }

  private static void warnIfUnpublishableTunable(TunableBase tunable) {
    if (tunable == null) {
      return;
    }
    if (tunable instanceof CustomTunable custom) {
      warnIfUnpublishableTunable(custom.getInnerTunable());
    } else if (tunable instanceof TunableBoolean
        || tunable instanceof TunableInt
        || tunable instanceof TunableLong
        || tunable instanceof TunableFloat
        || tunable instanceof TunableDouble
        || tunable instanceof TunableStruct<?>
        || tunable instanceof TunableStructArray<?>
        || tunable instanceof TunableProtobuf<?>) {
      return;
    } else if (tunable instanceof Tunable<?> generic) {
      warnIfUnsupportedBasicType(generic.getTypeClass());
    } else {
      TunableRegistry.reportWarning(UNSUPPORTED_TUNABLE_TYPE + tunable.getClass().getName());
    }
  }

  private static <T> Tunable<T> createBasic(T initialValue, TunableConfig config) {
    return createBasic(initialValue, getValueClass(initialValue), config);
  }

  private static <T> Tunable<T> createBasic(T initialValue, Class<T> cls, TunableConfig config) {
    warnIfUnsupportedBasicType(cls);
    return new BasicTunable<T>(cls, config) {
      @Override
      public void set(T value) {
        m_value = value;
        markChanged();
      }

      @Override
      public T get() {
        return m_value;
      }

      private T m_value = initialValue;
    };
  }

  private static <T> Struct<T> getStruct(Class<T> cls) {
    Struct<?> struct = getStructObject(cls);
    if (struct == null) {
      return null;
    }
    @SuppressWarnings("unchecked")
    Struct<T> typedStruct = (Struct<T>) struct;
    return typedStruct;
  }

  private static Struct<?> getStructObject(Class<?> cls) {
    // use introspection to get "struct" static variable
    Object obj;
    try {
      obj = cls.getField("struct").get(null);
    } catch (NoSuchFieldException e) {
      TunableRegistry.reportWarning("could not get struct field for " + cls.getName());
      return null;
    } catch (IllegalAccessException e) {
      TunableRegistry.reportWarning("could not access struct field for " + cls.getName());
      return null;
    }
    switch (obj) {
      case Struct<?> s when s.getTypeClass().equals(cls) -> {
        return s;
      }
      case Struct<?> s ->
          TunableRegistry.reportWarning(
              "type mismatch, expected '"
                  + s.getTypeClass().getName()
                  + "', got '"
                  + cls.getName()
                  + "'");
      default ->
          TunableRegistry.reportWarning(
              "struct field for " + cls.getName() + " is not of Struct<?> type");
    }
    return null;
  }

  @SuppressWarnings("unchecked")
  private static <T> Tunable<T> createStructArrayTunable(
      T initialValue, Class<T> cls, Struct<?> struct, TunableConfig config) {
    return (Tunable<T>)
        new TunableStructArray<>(
            (Object[]) initialValue, (Struct<Object>) struct, (Class<Object[]>) cls, config);
  }

  @SuppressWarnings("unchecked")
  private static <T> Tunable<T> createGetterSetterStructArrayTunable(
      Supplier<T> getter,
      Consumer<T> setter,
      Class<T> cls,
      Struct<?> struct,
      TunableConfig config) {
    return (Tunable<T>)
        new TunableStructArray<>(null, (Struct<Object>) struct, (Class<Object[]>) cls, config) {
          @Override
          public void set(Object[] value) {
            if (setter != null) {
              setter.accept((T) value);
            }
            markChanged();
          }

          @Override
          public Object[] get() {
            return (Object[]) getter.get();
          }

          @Override
          public Object[] mutate() {
            markChanged();
            return get();
          }
        };
  }

  private static <T> Protobuf<T, ?> getProtobuf(Class<T> cls) {
    // use introspection to get "proto" static variable
    Object obj;
    try {
      obj = cls.getField("proto").get(null);
    } catch (NoSuchFieldException e) {
      TunableRegistry.reportWarning("could not get proto field for " + cls.getName());
      return null;
    } catch (IllegalAccessException e) {
      TunableRegistry.reportWarning("could not access proto field for " + cls.getName());
      return null;
    }
    switch (obj) {
      case Protobuf<?, ?> s when s.getTypeClass().equals(cls) -> {
        @SuppressWarnings("unchecked")
        Protobuf<T, ?> proto = (Protobuf<T, ?>) s;
        return proto;
      }
      case Protobuf<?, ?> s ->
          TunableRegistry.reportWarning(
              "type mismatch, expected '"
                  + s.getTypeClass().getName()
                  + "', got '"
                  + cls.getName()
                  + "'");
      default ->
          TunableRegistry.reportWarning(
              "proto field for " + cls.getName() + " is not of Protobuf<?, ?> type");
    }
    return null;
  }

  private static <T> Tunable<T> createGetterSetterTunable(
      Supplier<T> getter, Consumer<T> setter, Struct<T> struct, TunableConfig config) {
    return new TunableStruct<T>(null, struct, config) {
      @Override
      public void set(T value) {
        if (setter != null) {
          setter.accept(value);
        }
        markChanged();
      }

      @Override
      public T get() {
        return getter.get();
      }
    };
  }

  private static <T> Tunable<T> createGetterSetterTunable(
      Supplier<T> getter, Consumer<T> setter, Protobuf<T, ?> proto, TunableConfig config) {
    return new TunableProtobuf<T>(null, proto, config) {
      @Override
      public void set(T value) {
        if (setter != null) {
          setter.accept(value);
        }
        markChanged();
      }

      @Override
      public T get() {
        return getter.get();
      }
    };
  }

  /**
   * Creates a tunable object with a null initial value.
   *
   * @param <T> data type
   * @param cls class of the tunable type
   * @return Tunable
   */
  public static <T> Tunable<T> createNull(Class<T> cls) {
    return createNullConfig(cls, null);
  }

  /**
   * Creates a tunable object with the given config and a null initial value.
   *
   * @param <T> data type
   * @param cls class of the tunable type
   * @param config tunable config
   * @return Tunable
   */
  public static <T> Tunable<T> createNullConfig(Class<T> cls, TunableConfig config) {
    return createBasic(null, cls, config);
  }

  /**
   * Creates a tunable object with the given initial value.
   *
   * @param <T> data type
   * @param initialValue the initial value
   * @return Tunable
   */
  public static <T> Tunable<T> create(T initialValue) {
    return createConfig(initialValue, null);
  }

  /**
   * Creates a tunable object with a specific Struct serializer.
   *
   * @param <T> data type
   * @param initialValue the initial value
   * @param struct struct serializer
   * @return Tunable
   */
  public static <T> Tunable<T> create(T initialValue, Struct<T> struct) {
    Objects.requireNonNull(struct);
    return createConfig(initialValue, struct, null);
  }

  /**
   * Creates a tunable object with a specific Protobuf serializer.
   *
   * @param <T> data type
   * @param initialValue the initial value
   * @param proto protobuf serializer
   * @return Tunable
   */
  public static <T> Tunable<T> create(T initialValue, Protobuf<T, ?> proto) {
    Objects.requireNonNull(proto);
    return createConfig(initialValue, proto, null);
  }

  /**
   * Creates a tunable object with the given getter and setter.
   *
   * <p>The getter and setter must not throw.
   *
   * @param <T> data type
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param cls class of the tunable type
   * @return Tunable
   */
  public static <T> Tunable<T> create(Supplier<T> getter, Consumer<T> setter, Class<T> cls) {
    return createConfig(getter, setter, cls, null);
  }

  /**
   * Creates a tunable object with the given config and initial value.
   *
   * @param <T> data type
   * @param initialValue the initial value
   * @param config tunable config
   * @return Tunable
   */
  public static <T> Tunable<T> createConfig(T initialValue, TunableConfig config) {
    Objects.requireNonNull(initialValue);
    Class<T> cls = getValueClass(initialValue);
    if (initialValue instanceof StructSerializable) {
      Struct<T> struct = getStruct(cls);
      if (struct != null) {
        return createConfig(initialValue, struct, config);
      }
    } else if (cls.isArray() && StructSerializable.class.isAssignableFrom(cls.getComponentType())) {
      Struct<?> struct = getStructObject(cls.getComponentType());
      if (struct != null) {
        return createStructArrayTunable(initialValue, cls, struct, config);
      }
    } else if (initialValue instanceof ProtobufSerializable) {
      Protobuf<T, ?> proto = getProtobuf(cls);
      if (proto != null) {
        return createConfig(initialValue, proto, config);
      }
    } else {
      // try other handlers
      var handler = TunableRegistry.getTypeHandler(initialValue);
      if (handler != null) {
        Tunable<T> tunable = handler.createTunable(initialValue, config);
        if (tunable != null) {
          warnIfUnpublishableTunable(tunable);
          return tunable;
        }
      }
    }
    return createBasic(initialValue, config);
  }

  /**
   * Creates a tunable object with a specific Struct serializer.
   *
   * @param <T> data type
   * @param initialValue the initial value (may be null)
   * @param struct struct serializer
   * @param config tunable config
   * @return Tunable
   */
  public static <T> Tunable<T> createConfig(
      T initialValue, Struct<T> struct, TunableConfig config) {
    return new TunableStruct<T>(initialValue, struct, config);
  }

  /**
   * Creates a tunable object with a specific Protobuf serializer.
   *
   * @param <T> data type
   * @param initialValue the initial value (may be null)
   * @param proto protobuf serializer
   * @param config tunable config
   * @return Tunable
   */
  public static <T> Tunable<T> createConfig(
      T initialValue, Protobuf<T, ?> proto, TunableConfig config) {
    return new TunableProtobuf<T>(initialValue, proto, config);
  }

  /**
   * Creates a tunable object with the given getter and setter and config.
   *
   * <p>The getter and setter must not throw.
   *
   * @param <T> data type
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param cls class of the tunable type
   * @param config tunable config
   * @return Tunable
   */
  public static <T> Tunable<T> createConfig(
      Supplier<T> getter, Consumer<T> setter, Class<T> cls, TunableConfig config) {
    if (config == null) {
      config = TunableConfig.of(TunableOption.ALWAYS_GET);
    } else if (config.getPolling() == TunableConfig.Polling.DEFAULT) {
      config = config.withPolling(TunableConfig.Polling.ALWAYS_GET);
    }
    if (cls != null && StructSerializable.class.isAssignableFrom(cls)) {
      Struct<T> struct = getStruct(cls);
      if (struct != null) {
        return createGetterSetterTunable(getter, setter, struct, config);
      }
    } else if (cls != null
        && cls.isArray()
        && StructSerializable.class.isAssignableFrom(cls.getComponentType())) {
      Struct<?> struct = getStructObject(cls.getComponentType());
      if (struct != null) {
        return createGetterSetterStructArrayTunable(getter, setter, cls, struct, config);
      }
    } else if (cls != null && ProtobufSerializable.class.isAssignableFrom(cls)) {
      Protobuf<T, ?> proto = getProtobuf(cls);
      if (proto != null) {
        return createGetterSetterTunable(getter, setter, proto, config);
      }
    }
    warnIfUnsupportedBasicType(cls);
    return new BasicTunable<T>(cls, config) {
      @Override
      public void set(T value) {
        if (setter != null) {
          setter.accept(value);
        }
        markChanged();
      }

      @Override
      public T get() {
        return getter.get();
      }
    };
  }

  /**
   * Constructs a tunable.
   *
   * @param config tunable config
   */
  protected Tunable(TunableConfig config) {
    super(config);
  }

  /**
   * Constructs a tunable.
   *
   * @param config tunable config
   * @param supportsChangeNotification whether the tunable notifies backends when it changes
   */
  protected Tunable(TunableConfig config, boolean supportsChangeNotification) {
    super(config, supportsChangeNotification);
  }

  /**
   * Sets the value.
   *
   * <p>Implementations must not throw.
   *
   * @param value value
   */
  public abstract void set(T value);

  /**
   * Gets the value.
   *
   * <p>Implementations must not throw.
   *
   * @return value
   */
  @Override
  public abstract T get();

  /**
   * Gets the value for in-place mutation and marks this tunable changed.
   *
   * <p>This is useful for mutable structured/protobuf values where modifying the object returned by
   * {@link #get()} would otherwise bypass {@link #set(Object)} and not notify backends. For
   * immutable values, prefer {@link #set(Object)}. For variable-length arrays, use {@link
   * #set(Object)} with a new array to change the length.
   *
   * @return mutable value
   */
  public T mutate() {
    markChanged();
    return get();
  }

  @Override
  public void accept(T value) {
    set(value);
  }

  /**
   * Gets the class of the tunable type. This is used for determining which type handler to use.
   *
   * @return class of the tunable type
   */
  public abstract Class<T> getTypeClass();
}
