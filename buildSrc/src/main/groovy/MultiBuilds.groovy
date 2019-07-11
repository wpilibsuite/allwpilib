
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
import edu.wpi.first.toolchain.ToolchainExtension
import org.gradle.model.Mutate;
import org.gradle.api.plugins.ExtensionContainer;
import org.gradle.nativeplatform.test.googletest.GoogleTestTestSuiteBinarySpec;
import org.gradle.model.RuleSource;
import org.gradle.model.Validate;
import org.gradle.nativeplatform.test.tasks.RunTestExecutable
import org.gradle.nativeplatform.NativeExecutableBinarySpec
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
import groovy.transform.CompileDynamic
import org.gradle.nativeplatform.BuildTypeContainer

@CompileStatic
class MultiBuilds implements Plugin<Project> {
  @CompileStatic
  public void apply(Project project) {

  }

  @CompileStatic
  static class Rules extends RuleSource {
    @CompileDynamic
    private static void setBuildableFalseDynamically(NativeBinarySpec binary) {
        binary.buildable = false
    }


    @Mutate
    @CompileStatic
    void disableReleaseGoogleTest(BinaryContainer binaries, ProjectLayout projectLayout) {
      def project = (Project) projectLayout.projectIdentifier
      if (project.hasProperty('testRelease')) {
        return
      }
      binaries.withType(GoogleTestTestSuiteBinarySpec) { oSpec ->
        GoogleTestTestSuiteBinarySpec spec = (GoogleTestTestSuiteBinarySpec) oSpec
        if (spec.buildType.name == 'release') {
          Rules.setBuildableFalseDynamically(spec)
        }
      }
    }
  }
}
