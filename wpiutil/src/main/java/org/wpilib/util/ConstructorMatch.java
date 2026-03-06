// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import java.lang.reflect.Constructor;
import java.util.List;
import java.util.Optional;

/**
 * Utility class to find the best matching constructor for a given set of parameter types. The
 * constructor must have parameter types that are assignable from the given parameter types, and the
 * parameter types must not be assignable to each other. If multiple constructors match, the one
 * with the most specific parameter types is chosen. If there is still a tie, the one with the most
 * specific first parameter type is chosen, then the second parameter type, and so on.
 *
 * @param <T> the type of the class to find the constructor for
 */
public class ConstructorMatch<T> {
  private final Constructor<T> m_constructor;
  private final List<Class<?>> m_parameterTypes;

  /**
   * Constructs a ConstructorMatch with the given constructor and parameter types. The parameter
   * types must not be assignable to each other.
   *
   * @param constructor the constructor to match
   * @param parameterTypes the parameter types for the constructor
   */
  public ConstructorMatch(Constructor<T> constructor, Class<?>... parameterTypes) {
    m_constructor = constructor;
    m_parameterTypes = List.of(parameterTypes);
    if (!isValidParameterPack(parameterTypes)) {
      throw new IllegalArgumentException("Parameter types must not be assignable to each other");
    }
  }

  private static boolean isValidParameterPack(Class<?>... types) {
    // Verify that all of the parameter types are not assignable to each other
    for (int i = 0; i < types.length; i++) {
      // Don't allow object parameters, as they would match any parameter type
      // and prevent more specific matches from being found
      if (types[i].equals(Object.class)) {
        return false;
      }

      for (int j = i + 1; j < types.length; j++) {
        if (types[i].isAssignableFrom(types[j]) || types[j].isAssignableFrom(types[i])) {
          return false;
        }
      }
    }
    return true;
  }

  /**
   * Creates a new instance of the constructor's class using the given arguments. The arguments must
   * match the parameter types of the constructor, and must not be assignable to each other. The
   * order of the arguments does not matter, as they will be matched to the parameter types.
   * Duplicate arguments are ignored, as the first match will match.
   *
   * @param args the arguments to pass to the constructor
   * @return a new instance of the constructor's class
   * @throws ReflectiveOperationException if the constructor cannot be invoked
   */
  public T newInstance(Object... args) throws ReflectiveOperationException {
    Object[] parameterArgs = new Object[m_parameterTypes.size()];
    // Find the incoming argument that matches each parameter type
    for (int i = 0; i < m_parameterTypes.size(); i++) {
      boolean found = false;
      for (Object arg : args) {
        if (m_parameterTypes.get(i).isAssignableFrom(arg.getClass())) {
          parameterArgs[i] = arg;
          found = true;
          break;
        }
      }
      if (!found) {
        throw new IllegalArgumentException(
            "No argument found for parameter type " + m_parameterTypes.get(i));
      }
    }
    return m_constructor.newInstance(parameterArgs);
  }

  /**
   * Finds the best matching constructor for the given class and parameter types. The constructor
   * must have parameter types that are assignable from the given parameter types, and the parameter
   * types must not be assignable to each other. If multiple constructors match, the one with the
   * most specific parameter types is chosen. If there is still a tie, the one with the most
   * specific first parameter type is chosen, then the second parameter type, and so on. The order
   * of the parameter types does not matter, as they will be matched to the constructor's parameter
   * types. Duplicate parameter types are ignored, as the first match will match.
   *
   * @param <T> the type of the class to find the constructor for
   * @param clazz the class to find the constructor for
   * @param parameterTypes the parameter types to match
   * @return an Optional containing the best matching ConstructorMatch, or empty if no match is
   *     found
   */
  public static <T> Optional<ConstructorMatch<T>> findBestConstructor(
      Class<T> clazz, Class<?>... parameterTypes) {
    if (!isValidParameterPack(parameterTypes)) {
      return Optional.empty();
    }
    Constructor<T> bestCtor = null;
    Class<?>[] bestParameterTypes = new Class<?>[parameterTypes.length];
    @SuppressWarnings("unchecked")
    Constructor<T>[] constructors = (Constructor<T>[]) clazz.getConstructors();
    for (Constructor<T> constructor : constructors) {
      Class<?>[] ctorParameterTypes = constructor.getParameterTypes();
      if (ctorParameterTypes.length != parameterTypes.length) {
        continue;
      }
      boolean matches = true;
      for (int i = 0; i < parameterTypes.length; i++) {
        // Don't allow object parameters, as they would match any parameter type and
        // prevent more specific matches from being found
        if (ctorParameterTypes[i].equals(Object.class)) {
          matches = false;
          break;
        }
        if (!ctorParameterTypes[i].isAssignableFrom(parameterTypes[i])) {
          matches = false;
          break;
        }
      }
      if (!matches) {
        continue;
      }
      boolean better = false;
      if (bestCtor == null) {
        better = true;
      } else {
        // Check if this constructor is more specific than the best one found so far
        // Order by parameter order so that if one constructor has a more specific
        // parameter type for the first parameter, it is preferred over a constructor
        // that has a more specific parameter type for the second parameter
        for (int i = 0; i < parameterTypes.length; i++) {
          if (ctorParameterTypes[i] != bestParameterTypes[i]) {
            if (bestParameterTypes[i].isAssignableFrom(ctorParameterTypes[i])) {
              better = true;
            }
            break;
          }
        }
      }
      if (better) {
        bestCtor = constructor;
        System.arraycopy(ctorParameterTypes, 0, bestParameterTypes, 0, parameterTypes.length);
      }
    }
    return bestCtor == null
        ? Optional.empty()
        : Optional.of(new ConstructorMatch<>(bestCtor, bestParameterTypes));
  }

  /**
   * Returns the constructor that was matched.
   *
   * @return the constructor that was matched
   */
  public Constructor<T> getConstructor() {
    return m_constructor;
  }

  /**
   * Returns the parameter types for the constructor.
   *
   * @return the parameter types for the constructor
   */
  public List<Class<?>> getParameterTypes() {
    return m_parameterTypes;
  }
}
