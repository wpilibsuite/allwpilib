package org.wpilib.javacplugin;

import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.Tree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import javax.tools.Diagnostic;

/**
 * Base class for scanners in this plugin. Helpers are provided for error reporting
 *
 * @param <R> the return type of this visitor's methods. Use {@link Void} for visitors that do not
 *     need to return results.
 * @param <P> the type of the additional parameter to this visitor's methods. Use {@code Void} for
 *     visitors that do not need an additional parameter.
 */
abstract class WPILibTreeScanner<R, P> extends TreeScanner<R, P> {
  public static final String WPILIB_MESSAGE_PREFIX = "[WPILib]";

  protected final CompilationUnitTree m_root;
  protected final JavacTask m_task;
  protected final Trees m_trees;

  WPILibTreeScanner(CompilationUnitTree tree, JavacTask task) {
    m_root = tree;
    m_task = task;
    m_trees = Trees.instance(m_task);
  }

  protected void printError(CharSequence message, Tree tree, CharSequence suppression) {
    m_trees.printMessage(
        Diagnostic.Kind.ERROR, buildFullMessage(message, suppression), tree, m_root);
  }

  private static String buildFullMessage(CharSequence message, CharSequence suppression) {
    StringBuilder builder =
        new StringBuilder(86)
            .append(WPILIB_MESSAGE_PREFIX)
            .append(' ')
            .append(normalizeMessage(message));

    if (suppression != null) {
      builder
          .append(" If this is intentional, the error may be silenced with @SuppressWarnings(\"")
          .append(suppression)
          .append("\")");
    }

    return builder.toString();
  }

  /**
   * Normalizes the message by appending a period if the message does not already end with a
   * punctuation mark.
   *
   * @param input The message to normalize.
   * @return The normalized message.
   */
  private static CharSequence normalizeMessage(CharSequence input) {
    if (input.isEmpty()) {
      return input;
    }

    return switch (input.charAt(input.length() - 1)) {
      case '.', '!', '?' -> input;
      default -> input + ".";
    };
  }
}
