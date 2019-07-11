/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.io.File;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.nio.file.Path;
import java.util.function.Predicate;

import org.junit.jupiter.api.extension.BeforeAllCallback;
import org.junit.jupiter.api.extension.BeforeEachCallback;
import org.junit.jupiter.api.extension.ExtensionConfigurationException;
import org.junit.jupiter.api.extension.ExtensionContext;
import org.junit.jupiter.api.extension.ExtensionContext.Namespace;
import org.junit.jupiter.api.extension.ParameterContext;
import org.junit.jupiter.api.extension.ParameterResolutionException;
import org.junit.jupiter.api.extension.ParameterResolver;
import org.junit.platform.commons.util.ExceptionUtils;
import org.junit.platform.commons.util.ReflectionUtils;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.sim.DriverStationSim;

import static org.junit.platform.commons.util.AnnotationUtils.findAnnotatedFields;
import static org.junit.platform.commons.util.ReflectionUtils.isPrivate;
import static org.junit.platform.commons.util.ReflectionUtils.makeAccessible;

public final class MockHardwareExtension implements BeforeAllCallback, BeforeEachCallback,
    ParameterResolver {
  private static final Namespace NAMESPACE = Namespace.create(MockHardwareExtension.class);
  private static final String KEY = "wpilib.simds";

  /**
   * Perform field injection for non-private, {@code static} fields (i.e.,
   * class fields) of type {@link Path} or {@link File} that are annotated with
   * {@link TempDir @TempDir}.
   */
  @Override
  public void beforeAll(ExtensionContext context) throws Exception {
    injectFields(context, null, ReflectionUtils::isStatic);
  }

  /**
   * Perform field injection for non-private, non-static fields (i.e.,
   * instance fields) of type {@link Path} or {@link File} that are annotated
   * with {@link TempDir @TempDir}.
   */
  @Override
  public void beforeEach(ExtensionContext context) throws Exception {
    injectFields(context, context.getRequiredTestInstance(), ReflectionUtils::isNotStatic);
  }

  /**
   * Determine if the {@link Parameter} in the supplied {@link ParameterContext}
   * is annotated with {@link SimDs @SimDs}.
   */
  @Override
  public boolean supportsParameter(ParameterContext parameterContext,
      ExtensionContext extensionContext) {
    boolean annotated = parameterContext.isAnnotated(SimDs.class);
    if (annotated && parameterContext.getDeclaringExecutable() instanceof Constructor) {
      throw new ParameterResolutionException(
        "@SimDs is not supported on constructor parameters. Please use field injection instead.");
    }
    return annotated;
  }

  /**
   * Resolve the current SimDs for the {@link Parameter} in the
   * supplied {@link ParameterContext}.
   */
  @Override
  public Object resolveParameter(ParameterContext parameterContext,
      ExtensionContext extensionContext) {
    Class<?> parameterType = parameterContext.getParameter().getType();
    assertSupportedType("parameter", parameterType);
    return getSimDs(extensionContext);
  }

  private void injectFields(ExtensionContext context, Object testInstance,
      Predicate<Field> predicate) {
    findAnnotatedFields(context.getRequiredTestClass(), SimDs.class, predicate).forEach(field -> {
      assertValidFieldCandidate(field);
      try {
        makeAccessible(field).set(testInstance, getSimDs(context));
      } catch (IllegalArgumentException | IllegalAccessException ex) {
        ExceptionUtils.throwAsUncheckedException(ex);
      }
    });
  }

  private void assertValidFieldCandidate(Field field) {
    assertSupportedType("field", field.getType());
    if (isPrivate(field)) {
      throw new ExtensionConfigurationException("@SimDs field ["
        + field
        + "] must not be private.");
    }
  }

  private void assertSupportedType(String target, Class<?> type) {
    if (type != DriverStationSim.class) {
      throw new ExtensionConfigurationException("Can only resolve @SimDs " + target + " of type "
          + DriverStationSim.class.getName() + " but was: " + type.getName());
    }
  }

  private DriverStationSim getSimDs(ExtensionContext extensionContext) {
    return extensionContext.getStore(NAMESPACE)
        .getOrComputeIfAbsent(KEY, key -> initializeHardware(), DriverStationSim.class);
  }

  private DriverStationSim initializeHardware() {
    HAL.initialize(500, 0);

    DriverStationSim dsSim = new DriverStationSim();
    dsSim.setDsAttached(true);
    dsSim.setAutonomous(false);
    dsSim.setEnabled(true);
    dsSim.setTest(true);

    return dsSim;
  }
}
