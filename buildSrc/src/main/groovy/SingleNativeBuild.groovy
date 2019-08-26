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
import org.gradle.platform.base.BinarySpec;
import org.gradle.platform.base.ComponentSpec;
import org.gradle.platform.base.ComponentSpecContainer;
import org.gradle.platform.base.BinaryContainer;
import org.gradle.platform.base.ComponentType;
import org.gradle.platform.base.TypeBuilder;
import org.gradle.nativeplatform.tasks.ObjectFilesToBinary;
import groovy.transform.CompileStatic;
import edu.wpi.first.nativeutils.tasks.ExportsGenerationTask

@CompileStatic
class SingleNativeBuild implements Plugin<Project> {
    @CompileStatic
    public void apply(Project project) {

    }

    @CompileStatic
    static class Rules extends RuleSource {
        @Mutate
        @CompileStatic
        void removeMacSystemIncludes(ModelMap<Task> tasks, BinaryContainer binaries) {
            binaries.each {
                if (!(it instanceof NativeBinarySpec)) {
                    return
                }
                NativeBinarySpec nativeBin = (NativeBinarySpec)it
                if (nativeBin.targetPlatform.operatingSystem.isMacOsX()) {
                    nativeBin.tasks.withType(AbstractNativeSourceCompileTask) { AbstractNativeSourceCompileTask compileTask->
                        compileTask.getSystemIncludes().setFrom()
                    }
                }
            }
        }

        @Mutate
        @CompileStatic
        void setupSingleNativeBuild(ModelMap<Task> tasks, ComponentSpecContainer components, BinaryContainer binaryContainer, ProjectLayout projectLayout) {
            Project project = (Project) projectLayout.projectIdentifier;

            def nativeName = project.extensions.extraProperties.get('nativeName')

            NativeLibrarySpec base = null
            def subs = []
            components.each { component ->
                if (component.name == "${nativeName}Base") {
                    base = (NativeLibrarySpec) component
                } else if (component.name == "${nativeName}" || component.name == "${nativeName}JNI") {
                    subs << component
                }
            }
            subs.each {
                ((NativeLibrarySpec) it).binaries.each { oBinary ->
                    if (oBinary.buildable == false) {
                        return
                    }
                    NativeBinarySpec binary = (NativeBinarySpec) oBinary
                    NativeBinarySpec baseBin = null
                    base.binaries.each { oTmpBaseBin ->
                        if (oTmpBaseBin.buildable == false) {
                            return
                        }
                        def tmpBaseBin = (NativeBinarySpec) oTmpBaseBin
                        if (tmpBaseBin.targetPlatform.name == binary.targetPlatform.name &&
                                tmpBaseBin.buildType == binary.buildType) {
                            baseBin = tmpBaseBin
                        }
                    }
                    baseBin.tasks.withType(AbstractNativeSourceCompileTask) { oCompileTask ->
                        def compileTask = (AbstractNativeSourceCompileTask) oCompileTask
                        if (binary instanceof SharedLibraryBinarySpec) {
                            def sBinary = (SharedLibraryBinarySpec) binary
                            ObjectFilesToBinary link = (ObjectFilesToBinary) sBinary.tasks.link
                            ExportsGenerationTask exportsTask = binary.tasks.withType(ExportsGenerationTask)[0]
                            if (exportsTask != null) {
                                exportsTask.dependsOn compileTask
                            }
                            link.dependsOn compileTask
                            link.inputs.dir compileTask.objectFileDir
                            def tree = project.fileTree(compileTask.objectFileDir)
                            tree.include '**/*.o'
                            tree.include '**/*.obj'
                            link.source tree
                        } else if (binary instanceof StaticLibraryBinarySpec) {
                            def sBinary = (StaticLibraryBinarySpec) binary
                            ObjectFilesToBinary assemble = (ObjectFilesToBinary) sBinary.tasks.createStaticLib
                            assemble.dependsOn compileTask
                            assemble.inputs.dir compileTask.objectFileDir
                            def tree = project.fileTree(compileTask.objectFileDir)
                            tree.include '**/*.o'
                            tree.include '**/*.obj'
                            assemble.source tree
                        }
                    }
                }
            }
        }
    }
}
