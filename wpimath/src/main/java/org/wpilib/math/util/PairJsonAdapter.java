// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.util;

import io.avaje.json.JsonAdapter;
import io.avaje.json.JsonReader;
import io.avaje.json.JsonWriter;
import io.avaje.jsonb.AdapterFactory;
import io.avaje.jsonb.CustomAdapter;
import io.avaje.jsonb.Jsonb;
import io.avaje.jsonb.Types;
import java.lang.reflect.Type;

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
          return new PairJsonAdapter(jsonb, args[0], args[1]);
        }
        return null;
      };

  public PairJsonAdapter(Jsonb jsonb, Type first, Type second) {
    firstAdapter = jsonb.adapter(first);
    secondAdapter = jsonb.adapter(second);
  }

  @Override
  public Pair<?, ?> fromJson(JsonReader reader) {
    reader.beginArray();
    Object first = reader.hasNextElement() ? firstAdapter.fromJson(reader) : null;
    Object second = reader.hasNextElement() ? secondAdapter.fromJson(reader) : null;
    reader.endArray();
    return Pair.of(first, second);
  }

  @Override
  public void toJson(JsonWriter writer, Pair<?, ?> value) {
    writer.beginArray();
    firstAdapter.toJson(writer, value.getFirst());
    secondAdapter.toJson(writer, value.getSecond());
    writer.endArray();
  }
}
