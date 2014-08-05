package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.util.BaseSystemNotInitializedException;

public class RobotState {
	private static Interface impl;

	public static void SetImplementation(Interface i) {
		impl = i;
	}

	public static boolean isDisabled() {
		if (impl != null) {
			return impl.isDisabled();
		} else {
			throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
		}
	}

	public static boolean isEnabled() {
		if (impl != null) {
			return impl.isEnabled();
		} else {
			throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
		}
	}

	public static boolean isOperatorControl() {
		if (impl != null) {
			return impl.isOperatorControl();
		} else {
			throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
		}
	}

	public static boolean isAutonomous() {
		if (impl != null) {
			return impl.isAutonomous();
		} else {
			throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
		}
	}

	public static boolean isTest() {
		if (impl != null) {
			return impl.isTest();
		} else {
			throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
		}
	}

	interface Interface {
		boolean isDisabled();
		boolean isEnabled();
		boolean isOperatorControl();
		boolean isAutonomous();
		boolean isTest();
	}
}
