package edu.wpi.first.wpilibj;

public class RobotState {
	private static Interface impl;

	public static void SetImplementation(Interface i) {
		impl = i;
	}

	public static boolean isDisabled() {
		if (impl != null) {
			return impl.isDisabled();
		} else {
			return true;
		}
	}

	public static boolean isEnabled() {
		if (impl != null) {
			return impl.isEnabled();
		} else {
			return false;
		}
	}

	public static boolean isOperatorControl() {
		if (impl != null) {
			return impl.isOperatorControl();
		} else {
			return true;
		}
	}

	public static boolean isAutonomous() {
		if (impl != null) {
			return impl.isAutonomous();
		} else {
			return false;
		}
	}

	public static boolean isTest() {
		if (impl != null) {
			return impl.isTest();
		} else {
			return false;
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
