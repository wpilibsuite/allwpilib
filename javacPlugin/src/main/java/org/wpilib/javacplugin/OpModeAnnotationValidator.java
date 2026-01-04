// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.AnnotationTree;
import com.sun.source.tree.AssignmentTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.LiteralTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.HashSet;
import java.util.Set;
import javax.lang.model.element.Element;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;

/**
 * Validates opmode annotations {@code @Autonomous}, {@code @Teleop}, {@code @TestOpMode}.
 *
 * <p>Requirements:
 *
 * <ul>
 *   <li>Name must be <= 32 characters
 *   <li>Group must be <= 12 characters
 *   <li>Description must be <= 64 characters
 * </ul>
 */
public class OpModeAnnotationValidator implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  /** The maximum number of permitted characters for an opmode name. */
  private static final int NAME_MAX_LENGTH = 32;

  /** The maximum number of permitted characters for an opmode group name. */
  private static final int GROUP_MAX_LENGTH = 12;

  /** The maximum number of permitted characters for an opmode description. */
  private static final int DESCRIPTION_MAX_LENGTH = 64;

  public OpModeAnnotationValidator(JavacTask task) {
    m_task = task;
  }

  @Override
  public void finished(TaskEvent e) {
    // We override `finished` instead of `started` because we want to run after the
    // ANALYZE attribution phase has completed and assigned types to elements in the AST
    // Track the visited CUs to avoid re-processing the same CU multiple times when we call
    // `Trees.getElement()` on a tree path.
    if (e.getKind() == TaskEvent.Kind.ANALYZE && m_visitedCUs.add(e.getCompilationUnit())) {
      e.getCompilationUnit().accept(new Scanner(e.getCompilationUnit()), null);
    }
  }

  private final class Scanner extends TreeScanner<Void, Void> {
    private final CompilationUnitTree m_root;
    private final Trees m_trees;

    Scanner(CompilationUnitTree compilationUnit) {
      m_root = compilationUnit;
      m_trees = Trees.instance(m_task);
    }

    @Override
    public Void visitAnnotation(AnnotationTree node, Void unused) {
      // Identify if this is one of the target annotations
      TreePath annoTypePath = m_trees.getPath(m_root, node.getAnnotationType());
      Element annoElement = annoTypePath != null ? m_trees.getElement(annoTypePath) : null;

      if (!(annoElement instanceof TypeElement typeElem)) {
        return super.visitAnnotation(node, unused);
      }

      CharSequence qname = typeElem.getQualifiedName();
      boolean isAutonomous = "org.wpilib.opmode.Autonomous".contentEquals(qname);
      boolean isTeleop = "org.wpilib.opmode.Teleop".contentEquals(qname);
      boolean isTestOpMode = "org.wpilib.opmode.TestOpMode".contentEquals(qname);

      if (!(isAutonomous || isTeleop || isTestOpMode)) {
        return super.visitAnnotation(node, unused);
      }

      // Extract provided attributes (they are optional). Only check name, group, description
      ExpressionTree nameExpr = null;
      ExpressionTree groupExpr = null;
      ExpressionTree descriptionExpr = null;

      var args = node.getArguments();
      if (args != null) {
        for (ExpressionTree arg : args) {
          if (arg instanceof AssignmentTree assign) {
            String key = assign.getVariable().toString();
            switch (key) {
              case "name" -> nameExpr = assign.getExpression();
              case "group" -> groupExpr = assign.getExpression();
              case "description" -> descriptionExpr = assign.getExpression();
              default -> {
                // Not a field we're validating, ignore it
              }
            }
          }
        }
      }

      checkLength(typeElem.getSimpleName(), "name", NAME_MAX_LENGTH, nameExpr);
      checkLength(typeElem.getSimpleName(), "group", GROUP_MAX_LENGTH, groupExpr);
      checkLength(typeElem.getSimpleName(), "description", DESCRIPTION_MAX_LENGTH, descriptionExpr);

      return super.visitAnnotation(node, unused);
    }

    private void checkLength(
        CharSequence typeName, String fieldName, int max, ExpressionTree valueExpr) {
      String value = evaluateStringConstant(valueExpr);
      if (value == null || value.length() <= max) {
        // If not provided or not a constant expression we can evaluate, do nothing
        // If below the permitted maximum, leave it alone
        return;
      }

      m_trees.printMessage(
          Diagnostic.Kind.ERROR,
          "@%s opmode %s must be <= %d characters (was %d)"
              .formatted(typeName, fieldName, max, value.length()),
          valueExpr,
          m_root);
    }

    private String evaluateStringConstant(ExpressionTree expr) {
      if (expr == null) {
        return null;
      }

      // Direct literal
      if (expr instanceof LiteralTree lit && lit.getValue() instanceof String string) {
        return string;
      }

      // Reference to a compile-time constant variable
      TreePath path = m_trees.getPath(m_root, expr);
      if (path != null) {
        Element el = m_trees.getElement(path);
        if (el instanceof VariableElement var && var.getConstantValue() instanceof String string) {
          return string;
        }
      }
      return null;
    }
  }
}
