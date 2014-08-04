package edu.wpi.first.wpilibj;

/**
 * Support for high level usage reporting.
 *
 * @author alex
 */
public class HLUsageReporting {
	private static Interface impl;

	public static void SetImplementation(Interface i) {
		impl = i;
	}

	public static void reportScheduler() {
		if (impl != null) {
			impl.reportScheduler();
		}
	}

	public static void reportPIDController(int num) {
		if (impl != null) {
			impl.reportPIDController(num);
		}
	}

	public static void reportSmartDashboard() {
		if(impl != null) {
			impl.reportSmartDashboard();
		}
	}

	public interface Interface {
		void reportScheduler();
		void reportPIDController(int num);
		void reportSmartDashboard();
	}
}
