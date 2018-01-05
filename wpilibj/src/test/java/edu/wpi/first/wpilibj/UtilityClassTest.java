package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.fail;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.junit.Test;

@SuppressWarnings("PMD.AbstractClassWithoutAbstractMethod")
public abstract class UtilityClassTest {

  private final Class clazz;

  UtilityClassTest(Class clazz) {
    this.clazz = clazz;
  }

  @Test
  public void testConstructorPrivate() {
    Constructor constructor = clazz.getDeclaredConstructors()[0];

    assertFalse(constructor.isAccessible());
  }

  @Test(expected = InvocationTargetException.class)
  public void testConstructorReflection() throws Throwable {
    Constructor constructor = clazz.getDeclaredConstructors()[0];
    constructor.setAccessible(true);
    constructor.newInstance();
  }

  @Test
  public void testPublicMethodsStatic() {
    List<String> failures = new ArrayList<>();
    for (Method method : clazz.getDeclaredMethods()) {
      int modifiers = method.getModifiers();
      if (Modifier.isPublic(modifiers) && !Modifier.isStatic(modifiers)) {
        failures.add(method.toString());
      }
    }

    if (!failures.isEmpty()) {
      fail("Found public methods that are not static: "
          + Arrays.toString(failures.toArray()));
    }
  }
}
