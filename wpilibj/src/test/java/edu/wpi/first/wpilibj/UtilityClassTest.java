/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
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
  private final Class m_clazz;

  protected UtilityClassTest(Class clazz) {
    m_clazz = clazz;
  }

  @Test
  public void testSingleConstructor() {
    assertEquals("More than one constructor defined", 1,
        m_clazz.getDeclaredConstructors().length);
  }

  @Test
  public void testConstructorPrivate() {
    Constructor constructor = m_clazz.getDeclaredConstructors()[0];

    assertFalse("Constructor is not private", constructor.isAccessible());
  }

  @Test(expected = InvocationTargetException.class)
  public void testConstructorReflection() throws Throwable {
    Constructor constructor = m_clazz.getDeclaredConstructors()[0];
    constructor.setAccessible(true);
    constructor.newInstance();
  }

  @Test
  public void testPublicMethodsStatic() {
    List<String> failures = new ArrayList<>();
    for (Method method : m_clazz.getDeclaredMethods()) {
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
