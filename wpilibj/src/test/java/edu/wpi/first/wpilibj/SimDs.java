/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.lang.annotation.Retention;
import java.lang.annotation.Target;
import java.lang.annotation.ElementType;
import java.lang.annotation.RetentionPolicy;

/**
 * {@code @SimDs} can be used to annotate a non-private field in a test class
 * or a parameter in a lifecycle method or test method of type
 * {@link DriverStationSim} or that should be resolved.
 *
 * <p>Please note that {@code @SimDs} is not supported on constructor
 * parameters. Please use field injection instead, by annotating a non-private
 * instance field with {@code @SimDs}.
 */
@Target({ ElementType.FIELD, ElementType.PARAMETER })
@Retention(RetentionPolicy.RUNTIME)
public @interface SimDs {
}
