// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.AssignmentTree;
import com.sun.source.tree.BinaryTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.MethodTree;
import com.sun.source.tree.ParenthesizedTree;
import com.sun.source.tree.ReturnTree;
import com.sun.source.tree.Tree;
import com.sun.source.tree.Tree.Kind;
import com.sun.source.tree.TypeCastTree;
import com.sun.source.tree.VariableTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.HashSet;
import java.util.Set;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;

/**
 * Detects integer division operations in Java source code and marks them as errors.
 *
 * <pre>{@code
 * double x = 1 / 2; // -> error: integer division for a floating-point number
 * double x = (double) 1 / 2; // -> ok (casting an operand to a double)
 * double x = (double) (1 / 2); // -> error: integer division for a floating-point number
 * int x = 1 / 2; // -> ok (in an integer context)
 * }</pre>
 */
public class IntegerDivisionDetector implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public IntegerDivisionDetector(JavacTask task) {
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
    public Void visitBinary(BinaryTree node, Void unused) {
      if (node.getKind() != Kind.DIVIDE) {
        return super.visitBinary(node, unused);
      }

      TypeMirror type = m_trees.getTypeMirror(m_trees.getPath(m_root, node));
      if (!isIntegerType(type)) {
        return super.visitBinary(node, unused);
      }

      // It's integer division. Now check the context.
      if (isFloatContext(m_trees.getPath(m_root, node))) {
        if (Suppressions.hasSuppression(
            m_trees, m_trees.getPath(m_root, node), "IntegerDivision")) {
          return super.visitBinary(node, unused);
        }
        m_trees.printMessage(
            Diagnostic.Kind.ERROR, "integer division in a floating-point context", node, m_root);
      }

      return super.visitBinary(node, unused);
    }

    private boolean isIntegerType(TypeMirror type) {
      if (type == null) {
        return false;
      }

      return switch (type.getKind()) {
        case BYTE, CHAR, SHORT, INT, LONG -> true;
        default -> false;
      };
    }

    private boolean isFloatType(TypeMirror type) {
      if (type == null) {
        return false;
      }

      return switch (type.getKind()) {
        case FLOAT, DOUBLE -> true;
        default -> false;
      };
    }

    private boolean isFloatContext(TreePath path) {
      TreePath parentPath = path.getParentPath();
      while (parentPath != null && parentPath.getLeaf() instanceof ParenthesizedTree) {
        parentPath = parentPath.getParentPath();
      }
      if (parentPath == null) {
        return false;
      }
      Tree parent = parentPath.getLeaf();

      switch (parent) {
        case AssignmentTree assignment -> {
          if (getExpression(assignment.getExpression()) == path.getLeaf()) {
            return isFloatType(
                m_trees.getTypeMirror(m_trees.getPath(m_root, assignment.getVariable())));
          }
        }
        case VariableTree variable -> {
          if (variable.getInitializer() != null
              && getExpression(variable.getInitializer()) == path.getLeaf()) {
            return isFloatType(m_trees.getTypeMirror(m_trees.getPath(m_root, variable.getType())));
          }
        }
        case ReturnTree _ -> {
          TreePath p = parentPath;
          while (p != null && !(p.getLeaf() instanceof MethodTree)) {
            p = p.getParentPath();
          }
          if (p != null && p.getLeaf() instanceof MethodTree method) {
            return isFloatType(
                m_trees.getTypeMirror(m_trees.getPath(m_root, method.getReturnType())));
          }
        }
        case TypeCastTree cast -> {
          return isFloatType(m_trees.getTypeMirror(m_trees.getPath(m_root, cast.getType())));
        }
        case null, default -> {
          return false;
        }
      }
      return false;
    }

    private Tree getExpression(ExpressionTree tree) {
      while (tree instanceof ParenthesizedTree p) {
        tree = p.getExpression();
      }
      return tree;
    }
  }
}
