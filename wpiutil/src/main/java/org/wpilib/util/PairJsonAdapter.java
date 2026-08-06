// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import io.avaje.json.JsonAdapter;
import io.avaje.json.JsonReader;
import io.avaje.json.JsonWriter;
import io.avaje.jsonb.AdapterFactory;
import io.avaje.jsonb.CustomAdapter;
import io.avaje.jsonb.Jsonb;
import io.avaje.jsonb.Types;
import java.lang.reflect.Type;
import java.util.Collection;
import java.util.Optional;
import java.util.OptionalDouble;
import java.util.OptionalInt;
import java.util.OptionalLong;

/**
 * For automated Jsonb use.
 *
 * @hidden
 */
@CustomAdapter
public class PairJsonAdapter implements JsonAdapter<Pair<?, ?>> {
  private final JsonAdapter<Object> firstAdapter;
  private final JsonAdapter<Object> secondAdapter;

  public static final AdapterFactory FACTORY =
      (type, jsonb) -> {
        if (Types.isGenericTypeOf(type, Pair.class)) {
          Type[] args = Types.typeArguments(type);
          return new PairJsonAdapter(jsonb, args[0], args[1]).nullSafe();
        }
        return null;
      };

  public PairJsonAdapter(Jsonb jsonb, Type first, Type second) {
    firstAdapter = jsonb.adapter(first);
    secondAdapter = jsonb.adapter(second);
  }

  // Adapter for raw type, using Object for generic parameters
  public PairJsonAdapter(Jsonb jsonb) {
    this(jsonb, Object.class, Object.class);
  }

  @Override
  public Pair<?, ?> fromJson(JsonReader reader) {
    reader.beginArray();
    if (!reader.hasNextElement()) {
      throw new IllegalStateException("Missing first field of pair");
    }
    Object first = firstAdapter.fromJson(reader);
    if (!reader.hasNextElement()) {
      throw new IllegalStateException("Missing second field of pair");
    }
    Object second = secondAdapter.fromJson(reader);
    reader.endArray();
    return Pair.of(first, second);
  }

  @Override
  public void toJson(JsonWriter writer, Pair<?, ?> value) {
    writer.beginArray();
    if (writer.serializeNulls()
        && (writer.serializeEmpty()
            || !(value.getFirst() instanceof Collection
                || value.getSecond() instanceof Collection))) {
      // If nulls and collections are serialized, the default behavior works
      firstAdapter.toJson(writer, value.getFirst());
      secondAdapter.toJson(writer, value.getSecond());
    } else {
      boolean wasSerializingNulls = writer.serializeNulls();
      boolean wasSerializingEmpty = writer.serializeEmpty();
      // If nulls or collections are not serialized, override the flags to still put fields at the
      // correct index
      if (value.getFirst() == null || isEmpty(value.getFirst())) {
        writer.serializeNulls(true);
        writer.serializeEmpty(true);
      }
      firstAdapter.toJson(writer, value.getFirst());
      if (value.getSecond() == null || isEmpty(value.getSecond())) {
        writer.serializeNulls(true);
        writer.serializeEmpty(true);
      } else {
        writer.serializeNulls(wasSerializingNulls);
        writer.serializeEmpty(wasSerializingEmpty);
      }
      secondAdapter.toJson(writer, value.getSecond());
      writer.serializeNulls(wasSerializingNulls);
      writer.serializeEmpty(wasSerializingEmpty);
    }
    writer.endArray();
  }

  private static boolean isEmpty(Object value) {
    if (value instanceof Optional) {
      return ((Optional<?>) value).isEmpty();
    } else if (value instanceof OptionalInt) {
      return ((OptionalInt) value).isEmpty();
    } else if (value instanceof OptionalLong) {
      return ((OptionalLong) value).isEmpty();
    } else if (value instanceof OptionalDouble) {
      return ((OptionalDouble) value).isEmpty();
    } else if (value instanceof Collection) {
      return ((Collection<?>) value).isEmpty();
    } else {
      return false;
    }
  }
}
