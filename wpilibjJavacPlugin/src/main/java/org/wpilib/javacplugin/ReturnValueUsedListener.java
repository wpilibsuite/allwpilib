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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;
import org.wpilib.annotation.NoDiscard;

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
        // If the parent node is an expression statement, then the value is ignored.
        // Otherwise, the value is used and we can ignore this site.
        return;
      }

      // Resolve the static type of the expression
      TypeMirror type = getType(node);
      if (type == null || type.getKind() == TypeKind.VOID) {
        // Skip void (e.g., void-returning methods)
        return;
      }

      // Special type checks (also applies to subtypes)
      m_specialTypes.forEach(
          (specialType, msg) -> {
            if (specialType == null) {
              // Not on classpath for this compilation unit
              return;
            }
            if (m_task.getTypes().isAssignable(type, specialType.asType())) {
              m_trees.printMessage(Diagnostic.Kind.ERROR, msg, node, m_root);
            }
          });

      // Check @NoDiscard on the invoked executable (method or constructor)
      var invoked = getInvokedExecutable(node);
      if (invoked != null) {
        List<String> messages = getNoDiscardMessages(invoked);
        for (String msg : messages) {
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

    /**
     * Collects all @NoDiscard messages applicable to the given executable: - The method/constructor
     * itself (if annotated) - The return type (if declared) including its superclasses and all
     * implemented interfaces Returns formatted diagnostics messages ready to print.
     */
    private List<String> getNoDiscardMessages(ExecutableElement method) {
      List<String> messages = new ArrayList<>();

      // 1) Method-level @NoDiscard
      var methodNoDiscard = method.getAnnotation(NoDiscard.class);
      if (methodNoDiscard != null) {
        String msg = methodNoDiscard.value();
        if (msg.isEmpty()) {
          messages.add("Result of @NoDiscard method is ignored");
        } else {
          messages.add(msg);
        }
      }

      // 2) Type-level @NoDiscard (classes and interfaces, recursively)
      var returnType = method.getReturnType();
      if (returnType instanceof DeclaredType dt && dt.asElement() instanceof TypeElement te) {
        Set<TypeElement> seen = new HashSet<>();
        collectNoDiscardMessagesFromTypeHierarchy(te, seen, messages);
      }

      return messages;
    }

    /**
     * Searches for @NoDiscard on the provided type element, its superclasses, and all implemented
     * interfaces (recursively). Appends formatted messages to the provided list for every match.
     */
    private void collectNoDiscardMessagesFromTypeHierarchy(
        TypeElement type, Set<TypeElement> seen, List<String> out) {
      if (type == null || !seen.add(type)) {
        return;
      }

      // Check this type directly
      var typeNoDiscard = type.getAnnotation(NoDiscard.class);
      if (typeNoDiscard != null) {
        String message = typeNoDiscard.value();
        if (message.isEmpty()) {
          out.add(
              "Result of method returning @NoDiscard type %s is ignored"
                  .formatted(type.getQualifiedName()));
        } else {
          out.add(message);
        }
      }

      // Check superclass chain
      var superMirror = type.getSuperclass();
      if (superMirror != null && superMirror.getKind() != TypeKind.NONE) {
        var superEl = m_task.getTypes().asElement(superMirror);
        if (superEl instanceof TypeElement ste) {
          collectNoDiscardMessagesFromTypeHierarchy(ste, seen, out);
        }
      }

      // Check all implemented interfaces (recursively)
      for (var iface : type.getInterfaces()) {
        var ifaceEl = m_task.getTypes().asElement(iface);
        if (ifaceEl instanceof TypeElement ite) {
          collectNoDiscardMessagesFromTypeHierarchy(ite, seen, out);
        }
      }
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
