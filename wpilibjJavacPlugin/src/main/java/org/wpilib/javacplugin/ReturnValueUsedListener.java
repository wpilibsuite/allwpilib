// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.tree.CompilationUnitTree;
import com.sun.source.tree.MethodInvocationTree;
import com.sun.source.tree.NewClassTree;
import com.sun.source.tree.Tree;
import com.sun.source.util.JavacTask;
import com.sun.source.util.TaskEvent;
import com.sun.source.util.TaskListener;
import com.sun.source.util.TreeScanner;
import com.sun.source.util.Trees;
import java.util.HashMap;
import java.util.Map;
import javax.lang.model.element.AnnotationValue;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;

/**
 * Checks for usages of methods that require their return values to be used. Some return types are
 * special cased, like Command; however, any method annotated with @NoDiscard will also be checked.
 * Special cased return types are useful for user code to get the benefits for free, without having
 * to manually opt into them.
 */
public class ReturnValueUsedListener implements TaskListener {
  private final JavacTask m_task;

  public ReturnValueUsedListener(JavacTask task) {
    m_task = task;
  }

  private class Scanner extends TreeScanner<Void, Void> {
    private final CompilationUnitTree m_root;
    private final Trees m_trees;
    private final Map<TypeElement, String> m_specialTypes;

    private static final String kNoDiscardFqn = "org.wpilib.annotation.NoDiscard";
    private static final String kCommands2CommandFqn = "edu.wpi.first.wpilibj2.command.Command";
    private static final String kCommands3CommandFqn = "org.wpilib.commands3.Command";

    private static final String kCommandMsg = "Return value of Command-returning method is ignored";

    Scanner(CompilationUnitTree compilationUnit) {
      m_root = compilationUnit;
      m_trees = Trees.instance(m_task);

      // Can't use Map.of because it prohibits null keys
      m_specialTypes = new HashMap<>();
      m_specialTypes.put(m_task.getElements().getTypeElement(kCommands2CommandFqn), kCommandMsg);
      m_specialTypes.put(m_task.getElements().getTypeElement(kCommands3CommandFqn), kCommandMsg);
    }

    @Override
    public Void visitMethodInvocation(MethodInvocationTree node, Void unused) {
      checkIgnoredExpression(node);
      return super.visitMethodInvocation(node, unused);
    }

    @Override
    public Void visitNewClass(NewClassTree node, Void unused) {
      checkIgnoredExpression(node);
      return super.visitNewClass(node, unused);
    }

    /**
     * Common logic for both method invocations and constructor calls when they appear as
     * stand-alone expression statements (i.e., their result is ignored).
     */
    private void checkIgnoredExpression(Tree node) {
      var path = m_trees.getPath(m_root, node);
      var parentPath = (path == null) ? null : path.getParentPath();
      if (parentPath == null || parentPath.getLeaf().getKind() != Tree.Kind.EXPRESSION_STATEMENT) {
        // If the parent node is an expression statement, then the return value is ignored.
        // Otherwise, the return value is used and we can ignore this site.
        return;
      }

      // Resolve the static type of the expression
      TypeMirror returnType = getType(node);
      if (returnType == null || returnType.getKind() == TypeKind.VOID) {
        // Skip void since there's nothing to return.
        // Maybe this should be a compiler error, but it might be bad library code and we shouldn't
        // have it break users.
        return;
      }

      m_specialTypes.forEach(
          (type, msg) -> {
            if (type == null) {
              // This type is not on the classpath in this compilation unit (i.e., users are not
              // using it); skip.
              return;
            }

            // Also applies to subtypes
            boolean isSpecial = m_task.getTypes().isAssignable(returnType, type.asType());
            if (isSpecial) {
              m_trees.printMessage(Diagnostic.Kind.ERROR, msg, node, m_root);
            }
          });

      // Check @NoDiscard on the invoked executable (method or constructor)
      var invoked = getInvokedExecutable(node);
      if (invoked != null) {
        String msg = getNoDiscardMessage(invoked);
        if (msg != null) {
          m_trees.printMessage(Diagnostic.Kind.ERROR, msg, node, m_root);
        }
      }
    }

    private TypeMirror getType(Tree node) {
      var path = m_trees.getPath(m_root, node);
      if (path == null) {
        return null;
      }
      // Requires running after ANALYZE attribution has completed for this CU.
      return m_trees.getTypeMirror(path);
    }

    private ExecutableElement getInvokedExecutable(Tree node) {
      var path = m_trees.getPath(m_root, node);
      if (path == null) {
        return null;
      }
      var el = m_trees.getElement(path);
      return (el instanceof ExecutableElement ee) ? ee : null;
    }

    private String getNoDiscardMessage(ExecutableElement method) {
      for (var mirror : method.getAnnotationMirrors()) {
        var annoType = mirror.getAnnotationType();
        if (annoType == null) {
          continue;
        }

        if (kNoDiscardFqn.equals(annoType.toString())) {
          return mirror.getElementValues().entrySet().stream()
              .filter(e -> e.getKey().getSimpleName().contentEquals("value"))
              .findFirst()
              .map(Map.Entry::getValue)
              .map(AnnotationValue::getValue)
              .map(
                  msg -> {
                    if ("".equals(msg)) {
                      return null;
                    } else {
                      return "Result of @NoDiscard method is ignored: " + msg;
                    }
                  })
              .orElse("Result of @NoDiscard method is ignored");
        }
      }

      return null;
    }
  }

  @Override
  public void finished(TaskEvent e) {
    // We override `finished` instead of `started` because we want to run after the
    // ANALYZE attribution phase has completed and assigned types to elements in the AST
    if (e.getKind() == TaskEvent.Kind.ANALYZE) {
      e.getCompilationUnit().accept(new Scanner(e.getCompilationUnit()), null);
    }
  }
}
