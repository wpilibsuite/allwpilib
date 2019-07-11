import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.lang.reflect.Field;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;

import org.gradle.api.GradleException;
import org.gradle.api.Plugin;
import org.gradle.api.Project;
import org.gradle.api.Task;
import org.gradle.api.file.FileTree;
import org.gradle.api.tasks.compile.JavaCompile;
import org.gradle.language.base.internal.ProjectLayout;
import org.gradle.language.base.plugins.ComponentModelBasePlugin;
import org.gradle.language.nativeplatform.tasks.AbstractNativeSourceCompileTask;
import org.gradle.model.ModelMap;
import org.gradle.model.Mutate;
import org.gradle.model.RuleSource;
import org.gradle.model.Validate;
import org.gradle.nativeplatform.NativeBinarySpec;
import org.gradle.nativeplatform.NativeComponentSpec;
import org.gradle.nativeplatform.NativeLibrarySpec;
import org.gradle.nativeplatform.SharedLibraryBinarySpec;
import org.gradle.nativeplatform.StaticLibraryBinarySpec;
import org.gradle.nativeplatform.platform.internal.NativePlatformInternal;
import org.gradle.nativeplatform.toolchain.NativeToolChain;
import org.gradle.nativeplatform.toolchain.NativeToolChainRegistry;
import org.gradle.nativeplatform.toolchain.internal.PlatformToolProvider;
import org.gradle.nativeplatform.toolchain.internal.ToolType;
import org.gradle.nativeplatform.toolchain.internal.gcc.AbstractGccCompatibleToolChain;
import org.gradle.nativeplatform.toolchain.internal.msvcpp.VisualCppToolChain;
import org.gradle.nativeplatform.toolchain.internal.tools.ToolRegistry;
import org.gradle.nativeplatform.test.googletest.GoogleTestTestSuiteBinarySpec;
import org.gradle.platform.base.BinarySpec;
import org.gradle.platform.base.ComponentSpec;
import org.gradle.platform.base.ComponentSpecContainer;
import org.gradle.platform.base.ComponentType;
import org.gradle.platform.base.TypeBuilder;
import org.gradle.nativeplatform.test.tasks.RunTestExecutable;
import org.gradle.platform.base.BinaryContainer;
import groovy.transform.CompileStatic;

@CompileStatic
class ExtraTasks implements Plugin<Project> {
    @CompileStatic
    public void apply(Project project) {

    }

    @CompileStatic
    static class Rules extends RuleSource {
        @Mutate
        @CompileStatic
        void createNativeCompileTask(ModelMap<Task> tasks, BinaryContainer binaries) {
            tasks.create('compileCpp', Task) { oTask ->
                def task = (Task) oTask
                task.group = 'build'
                task.description = 'Uber task to compile all native code for this project'
                binaries.each { binary ->
                    if (binary instanceof NativeBinarySpec && binary.buildable) {
                        binary.tasks.withType(AbstractNativeSourceCompileTask) { compileTask ->
                            task.dependsOn compileTask
                        }
                    }
                }
            }
        }

        @Mutate
        @CompileStatic
        void createNativeTestTask(ModelMap<Task> tasks, BinaryContainer binaries) {
            tasks.create('testCpp', Task) { oTask ->
                def task = (Task) oTask
                task.group = 'build'
                task.description = 'Uber task to run all native tests for project'
                binaries.each { binary ->
                    if (binary instanceof GoogleTestTestSuiteBinarySpec && binary.buildable) {
                        binary.tasks.withType(RunTestExecutable) { testTask ->
                            task.dependsOn testTask
                        }
                    }
                }
            }
        }
    }
}
