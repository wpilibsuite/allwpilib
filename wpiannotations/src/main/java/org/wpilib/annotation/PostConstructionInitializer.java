// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a method as a post-construction initializer. The WPILib compiler plugin will check for uses
 * of methods with this annotation and report a compiler error if the method is not called after the
 * object is constructed.
 *
 * <p>Limitations of this annotation:
 *
 * <ul>
 *   <li>Annotated methods cannot be static
 *   <li>Annotated methods must have at least the same access modifiers as the class they are in
 *       (e.g., annotated method in a public class must be public; annotated methods in a protected
 *       class must be protected or public)
 *   <li>Initializer methods must be called on the variable directly. They cannot be detected if
 *       called indirectly (e.g., on an object returned by a method)
 *       <pre>{@code
 * // This is OK
 * Foo foo = new Foo();
 * foo.init();
 *
 * // This is not OK
 * Box box = new Box(new Foo());
 * box.getFoo().init();
 *
 * }</pre>
 * </ul>
 *
 * <p>Errors reported by the compiler plugin may be suppressed by annotating the offending method
 * with {@code SuppressWarnings("PostConstructionInitializer")} or {@code
 * SuppressWarnings(PostConstructionInitializer.SUPPRESSION_KEY)}. This is intended to be used in
 * tests to allow runtime error handling code to be tested, but may also be used to suppress
 * spurious warnings in production code.
 */
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface PostConstructionInitializer {
  /**
   * The string key to use in {@link SuppressWarnings} annotations to suppress compiler error
   * messages related to this annotation.
   */
  String SUPPRESSION_KEY = "PostConstructionInitializer";
}
