package org.wpilib.util;

import java.lang.reflect.Constructor;
import java.util.List;
import java.util.Optional;

/**
 * Utility class to find the best matching constructor for a given set of
 * parameter types. The constructor must have parameter types that are
 * assignable from the given parameter types, and the parameter types must not
 * be assignable to each other. If multiple constructors match, the one with the
 * most specific parameter types is chosen. If there is still a tie, the one
 * with the most specific first parameter type is chosen, then the second
 * parameter type, and so on.
 */
public class ConstructorMatch<T> {
  private final Constructor<T> m_constructor;
  private final List<Class<?>> m_parameterTypes;

  public ConstructorMatch(Constructor<T> constructor, Class<?>... parameterTypes) {
    m_constructor = constructor;
    m_parameterTypes = List.of(parameterTypes);
    // Verify that all of the parameter types are not assignable to each other
    for (int i = 0; i < m_parameterTypes.size(); i++) {
      for (int j = i + 1; j < m_parameterTypes.size(); j++) {
        if (m_parameterTypes.get(i).isAssignableFrom(m_parameterTypes.get(j))
            || m_parameterTypes.get(j).isAssignableFrom(m_parameterTypes.get(i))) {
          throw new IllegalArgumentException(
              "Parameter types must not be assignable to each other");
        }
      }
    }
  }

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

  public static <T> Optional<ConstructorMatch<T>> findBestConstructor(Class<T> clazz, Class<?>... parameterTypes) {
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
    return bestCtor == null ? Optional.empty() : Optional.of(new ConstructorMatch<>(bestCtor, bestParameterTypes));
  }
}
