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
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;

/**
 * Detects any statements after a call to {@code coroutine.park()} and labels them as unreachable
 * code, similar to a {@code while (true)} statement.
 */
public class CodeAfterCoroutineParkDetector extends CoroutineBasedDetector {
  public CodeAfterCoroutineParkDetector(JavacTask task) {
    super(task);
  }

  @Override
  protected TreeScanner<?, ?> createScanner(CompilationUnitTree compilationUnit) {
    return new Scanner(compilationUnit);
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
              && m_task.getTypes().isSameType(m_coroutineType, ve.asType())) {
            parkInvocation = mit;
          }
        }
      }

      return super.visitBlock(node, param);
    }
  }
}
