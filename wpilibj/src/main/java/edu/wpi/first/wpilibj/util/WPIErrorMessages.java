package edu.wpi.first.wpilibj.util;

import static java.util.Objects.requireNonNull;

public class NullChecks {
  public static void requireNonNullParam(Object obj, String name) {
    requireNonNull(obj, "Parameter " + name + " was null when it should not have been!  "
        + "Check the stacktrace to find the responsible method call - usually, it is the first "
        + "line of user-written code indicated in the stacktrace.  Make sure all objects "
        + "passed to the method in question were properly initialized - note that this may "
        + "not be obvious if it is being called under dynamically-changing conditions!  Please "
        + "do not seek online technical assistance (on Chief Delphi or elsewhere) without doing this,"
        + " first!");
  }
}
