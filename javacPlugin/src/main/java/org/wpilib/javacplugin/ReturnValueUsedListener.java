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
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;
import org.wpilib.annotation.NoDiscard;

/** Checks for usages of methods that require their return values to be used. */
public class ReturnValueUsedListener implements TaskListener {
  private final JavacTask m_task;
  private final Set<CompilationUnitTree> m_visitedCUs = new HashSet<>();

  public ReturnValueUsedListener(JavacTask task) {
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

      if (Suppressions.hasSuppression(m_trees, path, "NoDiscard")) {
        return;
      }

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

      // 2) Type-level @NoDiscard (classes + interfaces recursively)
      TypeElement targetType = null;
      if (method.getKind() == ElementKind.CONSTRUCTOR) {
        // For constructors, the "return type" is the enclosing type
        var enclosing = method.getEnclosingElement();
        if (enclosing instanceof TypeElement te) {
          targetType = te;
        }
      } else {
        var returnType = method.getReturnType();
        if (returnType instanceof DeclaredType dt && dt.asElement() instanceof TypeElement te) {
          targetType = te;
        }
      }
      if (targetType != null) {
        Set<TypeElement> seen = new HashSet<>();
        collectNoDiscardMessagesFromTypeHierarchy(targetType, seen, messages);
      }

      return messages;
    }

    /**
     * Searches for @NoDiscard on the provided type element, its superclasses, and all implemented
     * interfaces (recursively). Appends formatted messages to the provided list for every match.
     *
     * @param type The type element to search
     * @param seen A set of type elements that have already been searched
     * @param out The list to append messages to
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
}
