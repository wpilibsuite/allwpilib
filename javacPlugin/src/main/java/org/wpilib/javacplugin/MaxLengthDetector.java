// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.AnnotationTree;
import com.sun.source.tree.AssignmentTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.LiteralTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.Tree.Kind;
import com.sun.source.tree.UnaryTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;
import org.wpilib.annotation.MaxLength;

public class MaxLengthDetector implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public MaxLengthDetector(JavacTask task) {
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
    public Void visitMethodInvocation(MethodInvocationTree node, Void unused) {
      // Find the invoked method to determine parameter annotations
      TreePath selectPath = m_trees.getPath(m_root, node.getMethodSelect());
      Element element = selectPath != null ? m_trees.getElement(selectPath) : null;
      List<? extends ExpressionTree> args = node.getArguments();

      if (!(element instanceof ExecutableElement method)) {
        return super.visitMethodInvocation(node, unused);
      }

      List<? extends VariableElement> params = method.getParameters();

      for (int i = 0; i < args.size(); i++) {
        ExpressionTree argument = args.get(i);

        if (!(argument instanceof LiteralTree literal)) {
          continue;
        }

        if (!(literal.getValue() instanceof String string)) {
          continue;
        }

        // Determine which parameter this argument maps to (accounting for varargs)
        int paramIndex = i;
        if (!params.isEmpty() && method.isVarArgs()) {
          paramIndex = Math.min(i, params.size() - 1);
        }

        if (paramIndex >= params.size()) {
          continue;
        }

        VariableElement param = params.get(paramIndex);
        var maxLength = param.getAnnotation(MaxLength.class);
        if (maxLength == null || string.length() <= maxLength.value()) {
          continue;
        }

        m_trees.printMessage(
            Diagnostic.Kind.ERROR,
            ("String literal exceeds maximum length: \"%s\""
                    + " (%d characters) is longer than %d character%s")
                .formatted(
                    string, string.length(), maxLength.value(), maxLength.value() == 1 ? "" : "s"),
            literal,
            m_root);
      }

      return super.visitMethodInvocation(node, unused);
    }

    // Checks for @MaxLength annotations with invalid configurations (zero or negative lengths)
    @Override
    public Void visitAnnotation(AnnotationTree node, Void unused) {
      // Validate usages of @MaxLength annotation: value must be >= 1
      TreePath annoTypePath = m_trees.getPath(m_root, node.getAnnotationType());
      Element annoElement = annoTypePath != null ? m_trees.getElement(annoTypePath) : null;

      if (!(annoElement instanceof TypeElement typeElem)
          || !"org.wpilib.annotation.MaxLength".contentEquals(typeElem.getQualifiedName())) {
        return super.visitAnnotation(node, unused);
      }

      // Extract the annotation's single parameter "value"
      ExpressionTree valueExpr = null;
      List<? extends ExpressionTree> args = node.getArguments();
      if (args != null && !args.isEmpty()) {
        for (ExpressionTree arg : args) {
          if (arg instanceof AssignmentTree assign) {
            if ("value".equals(assign.getVariable().toString())) {
              valueExpr = assign.getExpression();
              break;
            }
          } else {
            // Single unnamed argument form: @MaxLength(5)
            valueExpr = arg;
            break;
          }
        }
      }

      Integer constValue = evaluateIntConstant(valueExpr);
      if (constValue != null && constValue < 1) {
        m_trees.printMessage(
            Diagnostic.Kind.ERROR,
            "@MaxLength value must be >= 1 (was " + constValue + ")",
            valueExpr,
            m_root);
      }

      return super.visitAnnotation(node, unused);
    }

    private Integer evaluateIntConstant(ExpressionTree expr) {
      return switch (expr) {
        case null -> null;

        // Literal like 0, 1, etc.
        case LiteralTree lit when lit.getValue() instanceof Integer i -> {
          yield i;
        }

        // Handle unary minus of an int literal, e.g., -1
        case UnaryTree unary
            when unary.getKind() == Kind.UNARY_MINUS
                && unary.getExpression() instanceof LiteralTree literal
                && literal.getValue() instanceof Integer i -> {
          yield -i;
        }

        default -> {
          // Handle references to compile-time constants (static final int)
          TreePath path = m_trees.getPath(m_root, expr);
          if (path != null
              && m_trees.getElement(path) instanceof VariableElement var
              && var.getConstantValue() instanceof Integer i) {
            yield i;
          }

          yield null;
        }
      };
    }
  }
}
