// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.BlockTree;
import com.sun.source.tree.ClassTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.ExpressionTree;
import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.LambdaExpressionTree;
import com.sun.source.tree.LiteralTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.MethodTree;
import com.sun.source.tree.NewArrayTree;
import com.sun.source.tree.ParenthesizedTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreePath;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.HashSet;
import java.util.Set;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;

/**
 * Detects calls to {@code StateMachine.addState()} after a no-argument {@code
 * StateMachine.switchFromAny()} has been called on the same state machine object.
 */
public class AddStateAfterSwitchFromAnyDetector implements TaskListener {
  public static final String SUPPRESSION_KEY = "WPILib.AddStateAfterSwitchFromAny";
  public static final String ERROR_MESSAGE_FORMAT =
      "Cannot add new states to state machine `%s` after switchFromAny() "
          + "has been called with no arguments. "
          + "Move the addState() call to before all switchFromAny() calls, "
          + "or modify the switchFromAny() calls to explicitly list the states to transition from";

  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public AddStateAfterSwitchFromAnyDetector(JavacTask task) {
    m_task = task;
  }

  @Override
  public void finished(TaskEvent e) {
    if (e.getKind() == TaskEvent.Kind.ANALYZE && m_visitedCUs.add(e.getCompilationUnit())) {
      TypeElement smElement =
          m_task.getElements().getTypeElement("org.wpilib.command3.StateMachine");
      if (smElement == null) {
        // StateMachine is not on classpath
        return;
      }
      TypeMirror stateMachineType = smElement.asType();
      e.getCompilationUnit()
          .accept(new Scanner(e.getCompilationUnit(), stateMachineType), new HashSet<>());
    }
  }

  private final class Scanner extends TreeScanner<Void, Set<Element>> {
    private final CompilationUnitTree m_root;
    private final Trees m_trees;
    private final TypeMirror m_stateMachineType;

    Scanner(CompilationUnitTree compilationUnit, TypeMirror stateMachineType) {
      m_root = compilationUnit;
      m_trees = Trees.instance(m_task);
      m_stateMachineType = stateMachineType;
    }

    @Override
    public Void visitClass(ClassTree node, Set<Element> state) {
      return super.visitClass(node, new HashSet<>());
    }

    @Override
    public Void visitMethod(MethodTree node, Set<Element> state) {
      Set<Element> methodState = new HashSet<>();
      return super.visitMethod(node, methodState);
    }

    @Override
    public Void visitLambdaExpression(LambdaExpressionTree node, Set<Element> state) {
      Set<Element> lambdaState = new HashSet<>(state != null ? state : Set.of());
      return super.visitLambdaExpression(node, lambdaState);
    }

    @Override
    public Void visitBlock(BlockTree node, Set<Element> state) {
      Set<Element> workingState = state != null ? state : new HashSet<>();
      return super.visitBlock(node, workingState);
    }

    @Override
    public Void visitMethodInvocation(MethodInvocationTree node, Set<Element> state) {
      Set<Element> workingState = state != null ? state : new HashSet<>();
      super.visitMethodInvocation(node, workingState);

      TreePath path = m_trees.getPath(m_root, node);
      if (path == null) {
        return null;
      }

      Element el = m_trees.getElement(path);
      if (!(el instanceof ExecutableElement executableElement)) {
        return null;
      }

      if (isStateMachineMethod(executableElement, "switchFromAny")) {
        if (isNoArg(node)) {
          Element receiver = getReceiverElement(node);
          if (receiver != null) {
            workingState.add(receiver);
          }
        }
      } else if (isStateMachineMethod(executableElement, "addState")) {
        Element receiver = getReceiverElement(node);
        if (receiver != null
            && workingState.contains(receiver)
            && !Suppressions.hasSuppression(m_trees, path, SUPPRESSION_KEY)) {
          m_trees.printMessage(
              Diagnostic.Kind.ERROR, ERROR_MESSAGE_FORMAT.formatted(receiver), node, m_root);
        }
      }

      return null;
    }

    private boolean isStateMachineMethod(ExecutableElement method, String methodName) {
      if (!method.getSimpleName().contentEquals(methodName)) {
        return false;
      }
      Element enclosing = method.getEnclosingElement();
      if (enclosing instanceof TypeElement te) {
        return m_task.getTypes().isSameType(te.asType(), m_stateMachineType)
            || m_task.getTypes().isSubtype(te.asType(), m_stateMachineType);
      }
      return false;
    }

    private boolean isNoArg(MethodInvocationTree node) {
      if (node.getArguments().isEmpty()) {
        return true;
      }
      if (node.getArguments().size() == 1) {
        ExpressionTree arg = unwrap(node.getArguments().get(0));
        if (arg instanceof NewArrayTree nat && isEmptyArray(nat)) {
          return true;
        }
      }
      return false;
    }

    private boolean isEmptyArray(NewArrayTree nat) {
      return nat.getInitializers() != null && nat.getInitializers().isEmpty()
          || nat.getDimensions() != null
              && nat.getDimensions().size() == 1
              && unwrap(nat.getDimensions().get(0)) instanceof LiteralTree lit
              && Integer.valueOf(0).equals(lit.getValue());
    }

    private Element getReceiverElement(MethodInvocationTree mit) {
      ExpressionTree select = mit.getMethodSelect();
      if (select instanceof MemberSelectTree memberSelect) {
        ExpressionTree expr = unwrap(memberSelect.getExpression());
        TreePath exprPath = m_trees.getPath(m_root, expr);
        if (exprPath != null) {
          Element el = m_trees.getElement(exprPath);
          if (el != null) {
            return el;
          }
        }
      } else if (select instanceof IdentifierTree) {
        TreePath path = m_trees.getPath(m_root, mit);
        if (path != null) {
          Element el = m_trees.getElement(path);
          if (el != null) {
            return el.getEnclosingElement();
          }
        }
      }
      return null;
    }

    private ExpressionTree unwrap(ExpressionTree expr) {
      while (expr instanceof ParenthesizedTree p) {
        expr = p.getExpression();
      }
      return expr;
    }
  }
}
