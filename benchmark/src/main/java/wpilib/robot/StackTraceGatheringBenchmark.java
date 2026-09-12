package wpilib.robot;

public class StackTraceGatheringBenchmark {
    public static void a(boolean getStackTrace) {
        b(getStackTrace);
    }

    static void b(boolean getStackTrace) {
        c(getStackTrace);
    }

    static void c(boolean getStackTrace) {
        d(getStackTrace);
    }

    static void d(boolean getStackTrace) {
        e(getStackTrace);
    }

    static void e(boolean getStackTrace) {
        var throwable = new Throwable();
        if (getStackTrace) throwable.getStackTrace();
    }
}
