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
 *   <li>Static initializer methods must accept exactly one parameter of the type that defines the
 *       static method (they cannot accept a parameter of a supertype or derived type).
 *   <li>Static initializer methods with multiple parameters of the initialized type must annotate
 *       one of them with {@link InitializedParam} to disambiguate for the compiler.
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

  /**
   * Marks a specific parameter in a static initializer method as being the initialized object. This
   * disambiguates situations where static initializer methods accept multiple arguments of the same
   * initialize-required type; for example:
   *
   * <pre>{@code
   * class Foo {
   *   @PostConstructionInitializer
   *   static void copy(Foo src, @InitializedParam Foo dst) {
   *     // ...
   *   }
   * }
   * }</pre>
   *
   * <p>Static initializer methods must have a parameter of the exact type that defines the static
   * method.
   *
   * <pre>{@code
   * interface I {
   *   @PostConstructionInitializer
   *   static void init(I object) { ... }
   * }
   *
   * class Foo implements I {
   *   @PostConstructionInitializer
   *   static void initFoo(Foo foo) { ... } // OK
   *
   *   @PostConstructionInitializer
   *   static void initI(I object) { ... } // ERROR: I is not Foo
   *
   *   @PostConstructionInitializer
   *   static void initOther(SomeOtherType o) { ... } // ERROR: SomeOtherType is not Foo
   * }
   * }</pre>
   */
  @Target(ElementType.PARAMETER)
  @Retention(RetentionPolicy.RUNTIME)
  @interface InitializedParam {}
}
