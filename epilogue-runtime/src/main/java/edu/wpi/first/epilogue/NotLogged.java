package edu.wpi.first.epilogue;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * A field or method annotated as {@code @NotLogged} will be ignored by Epilogue when determining
 * the data to log.
 */
@Target({ElementType.FIELD, ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
public @interface NotLogged {}
