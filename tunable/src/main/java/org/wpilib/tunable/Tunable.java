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
  public static class TunableStruct<T> extends Tunable<T> {
    TunableStruct(T initialValue, Struct<T> struct, TunableConfig config) {
      super(config);
      m_struct = struct;
      m_value = initialValue;
    }

    @Override
    public void set(T value) {
      m_value = value;
      m_changed = true;
    }

    @Override
    public T get() {
      return m_value;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_struct.getTypeClass();
    }

    public Struct<T> getStruct() {
      return m_struct;
    }

    private final Struct<T> m_struct;
    private T m_value;
  }

  public static class TunableProtobuf<T> extends Tunable<T> {
    TunableProtobuf(T initialValue, Protobuf<T, ?> proto, TunableConfig config) {
      super(config);
      m_proto = proto;
      m_value = initialValue;
    }

    @Override
    public void set(T value) {
      m_value = value;
      m_changed = true;
    }

    @Override
    public T get() {
      return m_value;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_proto.getTypeClass();
    }

    public Protobuf<T, ?> getProtobuf() {
      return m_proto;
    }

    private final Protobuf<T, ?> m_proto;
    private T m_value;
  }

  private abstract static class BasicTunable<T> extends Tunable<T> {
    BasicTunable(Class<T> cls, TunableConfig config) {
      super(config);
      m_cls = cls;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_cls;
    }

    private final Class<T> m_cls;
  }

  @FunctionalInterface
  public interface CustomTunable {
    TunableBase getInnerTunable();
  }

  private static <T> Tunable<T> createBasic(T initialValue, TunableConfig config) {
    @SuppressWarnings("unchecked")
    Class<T> cls = (Class<T>) initialValue.getClass();
    return createBasic(initialValue, cls, config);
  }

  private static <T> Tunable<T> createBasic(T initialValue, Class<T> cls, TunableConfig config) {
    return new BasicTunable<T>(cls, config) {
      @Override
      public void set(T value) {
        m_value = value;
        m_changed = true;
      }

      @Override
      public T get() {
        return m_value;
      }

      private T m_value = initialValue;
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
    if (initialValue instanceof StructSerializable v) {
      // use introspection to get "struct" static variable
      Object obj;
      try {
        obj = v.getClass().getField("struct").get(null);
      } catch (NoSuchFieldException e) {
        TunableRegistry.reportWarning("could not get struct field for " + v.getClass().getName());
        return createBasic(initialValue, config);
      } catch (IllegalAccessException e) {
        TunableRegistry.reportWarning(
            "could not access struct field for " + v.getClass().getName());
        return createBasic(initialValue, config);
      }
      if (obj instanceof Struct<?> s) {
        if (s.getTypeClass().equals(initialValue.getClass())) {
          @SuppressWarnings("unchecked")
          Struct<T> s2 = (Struct<T>) s;
          return createConfig(initialValue, s2, config);
        } else {
          TunableRegistry.reportWarning(
              "type mismatch, expected '"
                  + s.getTypeClass().getName()
                  + "', got '"
                  + initialValue.getClass().getName()
                  + "'");
        }
      } else {
        TunableRegistry.reportWarning(
            "struct field for " + v.getClass().getName() + " is not of Struct<?> type");
      }
    } else if (initialValue instanceof ProtobufSerializable v) {
      // use introspection to get "proto" static variable
      Object obj;
      try {
        obj = v.getClass().getField("proto").get(null);
      } catch (NoSuchFieldException e) {
        TunableRegistry.reportWarning("could not get proto field for " + v.getClass().getName());
        return createBasic(initialValue, config);
      } catch (IllegalAccessException e) {
        TunableRegistry.reportWarning("could not access proto field for " + v.getClass().getName());
        return createBasic(initialValue, config);
      }
      if (obj instanceof Protobuf<?, ?> s) {
        if (s.getTypeClass().equals(initialValue.getClass())) {
          @SuppressWarnings("unchecked")
          Protobuf<T, ?> s2 = (Protobuf<T, ?>) s;
          return createConfig(initialValue, s2, config);
        } else {
          TunableRegistry.reportWarning(
              "type mismatch, expected '"
                  + s.getTypeClass().getName()
                  + "', got '"
                  + initialValue.getClass().getName()
                  + "'");
        }
      } else {
        TunableRegistry.reportWarning(
            "proto field for " + v.getClass().getName() + " is not of Protobuf<?, ?> type");
      }
    } else {
      // try other handlers
      var handler = TunableRegistry.getTypeHandler(initialValue);
      if (handler != null) {
        Tunable<T> tunable = handler.createTunable(initialValue, config);
        if (tunable != null) {
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
   * @return Tunable
   */
  public static <T> Tunable<T> createConfig(
      T initialValue, Protobuf<T, ?> proto, TunableConfig config) {
    return new TunableProtobuf<T>(initialValue, proto, config);
  }

  /**
   * Creates a tunable object with the given getter and setter and config.
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
    } else {
      config = config.withAlwaysGet(true);
    }
    return new BasicTunable<T>(cls, config) {
      @Override
      public void set(T value) {
        if (setter != null) {
          setter.accept(value);
        }
        m_changed = true;
      }

      @Override
      public T get() {
        return getter.get();
      }
    };
  }

  protected Tunable(TunableConfig config) {
    super(config);
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  public abstract void set(T value);

  /**
   * Gets the value.
   *
   * @return value
   */
  @Override
  public abstract T get();

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
