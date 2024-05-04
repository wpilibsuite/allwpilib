package edu.wpi.first.epilogue;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Placed on a subclass of {@code ClassSpecificLogger}. Epilogue will detect it at compile time and
 * allow logging of data types compatible with the logger.
 *
 * <pre><code>
 *   {@literal @}CustomLoggerFor(VendorMotorType.class)
 *    class ExampleMotorLogger extends ClassSpecificLogger&lt;VendorMotorType&gt; { }
 * </code></pre>
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface CustomLoggerFor {
  /**
   * The class or classes of objects able to be logged with the annotated logger.
   *
   * @return the supported data types
   */
  Class<?>[] value();
}
