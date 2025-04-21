// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import java.util.Optional;

/**
 * A utility class for fetching the assigned struct of existing classes. These are usually public,
 * static, and final properties with the Struct type.
 */
public final class StructFetcher {
  private StructFetcher() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Returns a {@link Struct} for the given {@link StructSerializable} marked class. Due to the
   * non-contractual nature of the marker this can fail. If the {@code struct} field could not be
   * accessed for any reason, an empty {@link Optional} is returned.
   *
   * @param <T> The type of the class.
   * @param clazz The class object to extract the struct from.
   * @return An optional containing the struct if it could be extracted.
   */
  @SuppressWarnings("unchecked")
  public static <T extends StructSerializable> Optional<Struct<T>> fetchStruct(
      Class<? extends T> clazz) {
    try {
      var possibleField = Optional.ofNullable(clazz.getDeclaredField("struct"));
      return possibleField.flatMap(
          field -> {
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
   * Returns a {@link Struct} for the given class. This does not do compile time checking that the
   * class is a {@link StructSerializable}. Whenever possible it is reccomended to use {@link
   * #fetchStruct(Class)}.
   *
   * @param clazz The class object to extract the struct from.
   * @return An optional containing the struct if it could be extracted.
   */
  @SuppressWarnings("unchecked")
  public static Optional<Struct<?>> fetchStructDynamic(Class<?> clazz) {
    if (StructSerializable.class.isAssignableFrom(clazz)) {
      return fetchStruct((Class<? extends StructSerializable>) clazz).map(struct -> struct);
    } else {
      return Optional.empty();
    }
  }
}
