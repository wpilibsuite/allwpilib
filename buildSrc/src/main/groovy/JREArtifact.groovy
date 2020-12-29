import groovy.transform.CompileStatic
import javax.inject.Inject
import jaci.gradle.deploy.artifact.MavenArtifact
import jaci.gradle.deploy.context.DeployContext
import org.gradle.api.Project
import jaci.gradle.ActionWrapper

import java.util.function.Function

@CompileStatic
class JREArtifact extends MavenArtifact {
    Function<DeployContext, Boolean> buildRequiresJre = (Function<DeployContext, Boolean>){ true }

    void setJreDependency(String dep) {
      dependency = project.dependencies.add(configuration(), dep)
    }

    @Inject
    JREArtifact(String name, Project project) {
        super(name, project)
        configuration = project.configurations.create(configuration())

        onlyIf = { DeployContext ctx ->
            (buildRequiresJre.apply(ctx) && jreMissing(ctx)) || project.hasProperty("force-redeploy-jre")
        }

        predeploy << new ActionWrapper({ DeployContext ctx ->
            ctx.logger.log('Deploying RoboRIO JRE (this will take a while)...')
        })

        directory = '/tmp'
        filename = 'frcjre.ipk'

        postdeploy << new ActionWrapper({ DeployContext ctx ->
            ctx.logger.log('Installing JRE...')
            ctx.execute('opkg remove frc2020-openjdk*; opkg install /tmp/frcjre.ipk; rm /tmp/frcjre.ipk')
            ctx.logger.log('JRE Deployed!')
        })
    }

    String configuration() {
        return name + 'frcjre'
    }

    boolean jreMissing(DeployContext ctx) {
        return ctx.execute('if [[ -f "/usr/local/frc/JRE/bin/java" ]]; then echo OK; else echo MISSING; fi').result.contains("MISSING")
    }
}
