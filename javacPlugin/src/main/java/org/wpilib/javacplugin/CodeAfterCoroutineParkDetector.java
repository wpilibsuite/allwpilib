// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.BlockTree;
import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.EmptyStatementTree;
import com.sun.source.tree.ExpressionStatementTree;
import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.MemberSelectTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.StatementTree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.HashSet;
import java.util.Set;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;

/**
 * Detects any statements after a call to {@code coroutine.park()} and labels them as unreachable
 * code, similar to a {@code while (true)} statement.
 */
public class CodeAfterCoroutineParkDetector implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public CodeAfterCoroutineParkDetector(JavacTask task) {
    m_task = task;
  }

  @Override
  public void finished(TaskEvent taskEvent) {
    CompilationUnitTree compilationUnit = taskEvent.getCompilationUnit();
    if (taskEvent.getKind() == TaskEvent.Kind.ANALYZE && m_visitedCUs.add(compilationUnit)) {
      TypeMirror coroutineType = getCoroutineType();
      if (coroutineType == null) {
        // Not using the commands library; nothing to scan for
        return;
      }

      compilationUnit.accept(new Scanner(compilationUnit), null);
    }
  }

  private TypeMirror getCoroutineType() {
    var te = m_task.getElements().getTypeElement("org.wpilib.commands3.Coroutine");
    return te != null ? te.asType() : null;
  }

  private final class Scanner extends TreeScanner<Void, Void> {
    private final CompilationUnitTree m_root;
    private final Trees m_trees = Trees.instance(m_task);

    Scanner(CompilationUnitTree compilationUnit) {
      m_root = compilationUnit;
    }

    @Override
    public Void visitBlock(BlockTree node, Void param) {
      MethodInvocationTree parkInvocation = null;
      for (StatementTree statement : node.getStatements()) {
        if (statement instanceof EmptyStatementTree) {
          // skip empty statements; someone could have just added an extra semicolon by accident
          continue;
        }

        if (parkInvocation != null) {
          m_trees.printMessage(
              Diagnostic.Kind.ERROR,
              "Unreachable statement: `" + parkInvocation + "` will never exit",
              statement,
              m_root);
          break;
        }

        if (statement instanceof ExpressionStatementTree est
            && est.getExpression() instanceof MethodInvocationTree mit
            && mit.getMethodSelect() instanceof MemberSelectTree ms
            && ms.getIdentifier().contentEquals("park")
            && ms.getExpression() instanceof IdentifierTree id) {
          var idPath = m_trees.getPath(m_root, id);
          var identifierElement = m_trees.getElement(idPath);
          if (identifierElement instanceof VariableElement ve
              && m_task.getTypes().isSameType(getCoroutineType(), ve.asType())) {
            parkInvocation = mit;
          }
        }
      }

      return super.visitBlock(node, param);
    }
  }
}
