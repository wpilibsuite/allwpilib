// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.util.TreePath;
import com.sun.source.util.Trees;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/** Utility class for checking for compiler error and warning suppressions. */
public final class Suppressions {
  private Suppressions() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Checks if the given tree has a suppression annotation for at least one of the given keys. The
   * "all" suppression key is automatically included.
   *
   * @param trees The tree utils to use to loop up elements.
   * @param tree The tree to the node to check for suppressions.
   * @param suppressionKeys The suppression keys to check for.
   * @return True if the tree has a suppression annotation for at least one of the given keys or
   *     "all". False otherwise
   */
  public static boolean hasSuppression(Trees trees, TreePath tree, String... suppressionKeys) {
    TreePath currentPath = tree;
    Set<String> suppressionKeysSet = new HashSet<>(List.of(suppressionKeys));
    suppressionKeysSet.add("all");

    while (currentPath != null) {
      var element = trees.getElement(currentPath);
      currentPath = currentPath.getParentPath();

      if (element == null) {
        continue;
      }

      var suppression = element.getAnnotation(SuppressWarnings.class);
      if (suppression == null) {
        continue;
      }

      for (String key : suppression.value()) {
        if (suppressionKeysSet.contains(key)) {
          return true;
        }
      }
    }

    // No suppression annotations found on the element or any parent element.
    return false;
  }
}
