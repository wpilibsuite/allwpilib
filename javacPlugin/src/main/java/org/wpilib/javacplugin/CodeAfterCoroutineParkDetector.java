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
import javax.lang.model.element.VariableElement;

/**
 * Detects any statements after a call to {@code coroutine.park()} and labels them as unreachable
 * code, similar to a {@code while (true)} statement.
 */
public class CodeAfterCoroutineParkDetector extends CoroutineBasedDetector {
  public static final String SUPPRESSION_KEY = "WPILib.CodeAfterCoroutinePark";

  public CodeAfterCoroutineParkDetector(JavacTask task) {
    super(task);
  }

  @Override
  protected TreeScanner<?, ?> createScanner(CompilationUnitTree compilationUnit) {
    return new Scanner(compilationUnit);
  }

  private final class Scanner extends WPILibTreeScanner<Void, Void> {
    Scanner(CompilationUnitTree compilationUnit) {
      super(compilationUnit, CodeAfterCoroutineParkDetector.this.m_task);
    }

    @Override
    public Void visitBlock(BlockTree node, Void param) {
      var path = m_trees.getPath(m_root, node);
      if (Suppressions.hasSuppression(m_trees, path, SUPPRESSION_KEY)) {
        // Error is suppressed for this block, don't bother checking
        return super.visitBlock(node, param);
      }

      MethodInvocationTree parkInvocation = null;
      for (StatementTree statement : node.getStatements()) {
        if (statement instanceof EmptyStatementTree) {
          // skip empty statements; someone could have just added an extra semicolon by accident
          continue;
        }

        if (parkInvocation != null) {
          printError(
              "Unreachable statement: `" + parkInvocation + "` will never exit.",
              statement,
              SUPPRESSION_KEY);
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
