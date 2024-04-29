package edu.wpi.first.wpilibj3.command.async;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;

public class MultiException extends RuntimeException {
  private static final String EXCEPTION_SEPARATOR =
      "\n\t______________________________________________________________________\n";

  /** The nested exceptions. These will be printed out in order of appearance. */
  private final List<Throwable> nested = new ArrayList<>();

  public MultiException() {
    super("Multiple exceptions");
  }

  public void add(Throwable throwable) {
    if (throwable instanceof MultiException other) {
      nested.addAll(other.nested);
    } else {
      nested.add(throwable);
    }
  }

  @Override
  public String getMessage() {
    if (nested.isEmpty()) {
      return "<no nested exceptions>";
    } else {
      StringBuilder sb = new StringBuilder();
      int n = nested.size();
      sb.append(n).append(n == 1 ? " nested exception:" : " nested exceptions:");

      for (Throwable t : nested) {
        sb.append(EXCEPTION_SEPARATOR).append("\n\t");
        var sw = new StringWriter();
        t.printStackTrace(new PrintWriter(sw));
        sb.append(sw.toString().replace("\n", "\n\t").trim());
      }

      sb.append(EXCEPTION_SEPARATOR);
      return sb.toString();
    }
  }
}
