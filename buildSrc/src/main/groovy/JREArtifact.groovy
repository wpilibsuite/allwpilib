import groovy.transform.CompileStatic
import javax.inject.Inject
import edu.wpi.first.deployutils.deploy.artifact.MavenArtifact
import edu.wpi.first.deployutils.deploy.context.DeployContext
import org.gradle.api.Project
import edu.wpi.first.deployutils.ActionWrapper
import edu.wpi.first.deployutils.deploy.target.RemoteTarget
import edu.wpi.first.deployutils.PredicateWrapper

import java.util.function.Function

@CompileStatic
public class FRCJREArtifact extends MavenArtifact {
    private final String configName;

    public String getConfigName() {
        return configName;
    }

    @Inject
    public FRCJREArtifact(String name, RemoteTarget target) {
        super(name, target);
        String configName = name + "frcjre";
        this.configName = configName;
        Project project = target.getProject();
        getConfiguration().set(project.getConfigurations().create(configName));
        getDependency().set(project.getDependencies().add(configName, "edu.wpi.first.jdk:roborio-2021:11.0.9u11-1"));

        setOnlyIf(new PredicateWrapper({ DeployContext ctx ->
            return jreMissing(ctx) || project.hasProperty("force-redeploy-jre");
        }));

        getDirectory().set("/tmp");
        getFilename().set("frcjre.ipk");

        getPostdeploy().add(new ActionWrapper({ DeployContext ctx ->
            ctx.getLogger().log("Installing JRE...");
            ctx.execute("opkg remove frc2020-openjdk*; opkg install /tmp/frcjre.ipk; rm /tmp/frcjre.ipk");
            ctx.getLogger().log("JRE Deployed!");
        }));
    }

    private boolean jreMissing(DeployContext ctx) {
        return ctx.execute("if [[ -f \"/usr/local/frc/JRE/bin/java\" ]]; then echo OK; else echo MISSING; fi").getResult().contains("MISSING");
    }


}
