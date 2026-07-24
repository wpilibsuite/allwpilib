// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.LiteralTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.NewArrayTree;
import com.sun.source.tree.NewClassTree;
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
import javax.lang.model.type.ArrayType;
import javax.lang.model.type.DeclaredType;
import javax.tools.Diagnostic;

public class MatrixSizeListener implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public MatrixSizeListener(JavacTask task) {
    m_task = task;
  }

  @Override
  public void finished(TaskEvent e) {
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
      TreePath selectPath = m_trees.getPath(m_root, node.getMethodSelect());
      Element element = selectPath != null ? m_trees.getElement(selectPath) : null;

      if (element instanceof ExecutableElement method) {
        Element enclosing = method.getEnclosingElement();
        if (enclosing instanceof TypeElement typeElem
            && "org.wpilib.math.linalg.MatBuilder".equals(typeElem.getQualifiedName().toString())
            && "fill".equals(method.getSimpleName().toString())) {
          checkMatrixDef(node, node.getArguments(), true);
        }
      }
      return super.visitMethodInvocation(node, unused);
    }

    @Override
    public Void visitNewClass(NewClassTree node, Void unused) {
      TreePath path = m_trees.getPath(m_root, node);
      Element element = path != null ? m_trees.getElement(path) : null;
      if (element instanceof ExecutableElement constructor
          && constructor.getEnclosingElement() instanceof TypeElement typeElem
          && "org.wpilib.math.linalg.Matrix".equals(typeElem.getQualifiedName().toString())) {
        checkMatrixDef(node, node.getArguments(), false);
      }
      return super.visitNewClass(node, unused);
    }

    private void checkMatrixDef(
        ExpressionTree node, List<? extends ExpressionTree> args, boolean isMatBuilderMethod) {
      if (args.size() < 2) {
        return;
      }
      Integer rows = parseDimension(args.get(0));
      Integer cols = parseDimension(args.get(1));
      if (rows == null || cols == null) {
        return;
      }
      int expectedElements = rows * cols;

      Integer actualElements;
      if (args.size() == 3) {
        var thirdArg = args.get(2);
        var thirdArgPath = m_trees.getPath(m_root, thirdArg);
        actualElements =
            switch (m_trees.getTypeMirror(thirdArgPath)) {
              case null -> null;
              case ArrayType _ -> getArraySize(thirdArg);
              default -> 1;
            };
      } else if (args.size() > 3) {
        actualElements = args.size() - 2;
      } else if (!isMatBuilderMethod && args.size() == 2) {
        return;
      } else {
        actualElements = 0;
      }

      if (actualElements != null && actualElements != expectedElements) {
        m_trees.printMessage(
            Diagnostic.Kind.ERROR,
            String.format(
                "Invalid matrix data provided. Wanted %d x %d matrix, but got %d elements",
                rows, cols, actualElements),
            node,
            m_root);
      }
    }

    private Integer parseDimension(ExpressionTree expr) {
      if (expr == null) {
        return null;
      }

      // Checks for Nat.NX() calls
      if (expr instanceof MethodInvocationTree methodCall) {
        TreePath path = m_trees.getPath(m_root, methodCall.getMethodSelect());
        Element element = path != null ? m_trees.getElement(path) : null;
        if (element instanceof ExecutableElement method
            && method.getEnclosingElement() instanceof TypeElement typeElem
            && "org.wpilib.math.util.Nat".equals(typeElem.getQualifiedName().toString())) {
          String qualifiedName = method.getSimpleName().toString();
          if (qualifiedName.matches("^N\\d(\\d?)$")) {
            return Integer.parseInt(qualifiedName.substring(1));
          }
        }
      }

      // Checks for NX.instance calls (not recommended, but still valid java syntax)
      TreePath path = m_trees.getPath(m_root, expr);
      if (path != null
          && m_trees.getTypeMirror(path) instanceof DeclaredType type
          && type.asElement() instanceof TypeElement element) {
        String qualifiedName = element.getQualifiedName().toString();
        if (qualifiedName.startsWith("org.wpilib.math.numbers.N")) {
          String suffix = qualifiedName.substring("org.wpilib.math.numbers.N".length());
          if (suffix.matches("^\\d(\\d?)$")) {
            return Integer.parseInt(suffix);
          }
        }
      }

      return null;
    }

    private Integer getArraySize(ExpressionTree expr) {
      if (expr instanceof NewArrayTree arrayTree) {
        if (arrayTree.getInitializers() != null) {
          return arrayTree.getInitializers().size();
        }
        var dims = arrayTree.getDimensions();
        if (!dims.isEmpty()
            && dims.getFirst() instanceof LiteralTree tree
            && tree.getValue() instanceof Integer value) {
          return value;
        }
      }
      return null;
    }
  }
}
