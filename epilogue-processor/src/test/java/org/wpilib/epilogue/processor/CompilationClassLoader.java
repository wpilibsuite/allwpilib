// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import com.google.testing.compile.Compilation;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLStreamHandler;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;
import javax.tools.JavaFileObject;
import org.wpilib.epilogue.EpilogueService;

class CompilationClassLoader extends ClassLoader {
  private final Compilation m_compilation;

  @SuppressWarnings("PMD.UseProperClassLoader")
  CompilationClassLoader(Compilation compilation) {
    super(EpilogueService.class.getClassLoader());
    this.m_compilation = compilation;
  }

  @Override
  protected Class<?> findClass(String name) throws ClassNotFoundException {
    String classPath = "/" + name.replace('.', '/') + ".class";
    for (JavaFileObject file : m_compilation.generatedFiles()) {
      if (file.toUri().getPath().endsWith(classPath)) {
        try (InputStream is = file.openInputStream()) {
          byte[] bytes = is.readAllBytes();
          return defineClass(name, bytes, 0, bytes.length);
        } catch (IOException e) {
          throw new ClassNotFoundException(name, e);
        }
      }
    }
    return super.findClass(name);
  }

  @Override
  public InputStream getResourceAsStream(String name) {
    String path = name.startsWith("/") ? name : "/" + name;
    for (JavaFileObject file : m_compilation.generatedFiles()) {
      if (file.toUri().getPath().endsWith(path)) {
        try {
          return file.openInputStream();
        } catch (IOException e) {
          return null;
        }
      }
    }
    return super.getResourceAsStream(name);
  }

  @Override
  @SuppressWarnings("deprecation")
  protected Enumeration<URL> findResources(String name) throws IOException {
    List<URL> urls = new ArrayList<>();
    String path = name.startsWith("/") ? name : "/" + name;
    for (JavaFileObject file : m_compilation.generatedFiles()) {
      if (file.toUri().getPath().endsWith(path)) {
        urls.add(
            new URL(
                "mem",
                null,
                -1,
                path,
                new URLStreamHandler() {
                  @Override
                  protected URLConnection openConnection(URL u) {
                    return new URLConnection(u) {
                      @Override
                      public void connect() {}

                      @Override
                      public InputStream getInputStream() throws IOException {
                        return file.openInputStream();
                      }
                    };
                  }
                }));
      }
    }
    return Collections.enumeration(urls);
  }
}
