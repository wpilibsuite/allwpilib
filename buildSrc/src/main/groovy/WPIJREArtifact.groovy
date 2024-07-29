import groovy.transform.CompileStatic;
import javax.inject.Inject;

import org.gradle.api.Project;

import edu.wpi.first.deployutils.deploy.CommandDeployResult;
import edu.wpi.first.deployutils.deploy.artifact.MavenArtifact;
import edu.wpi.first.deployutils.deploy.context.DeployContext;
import edu.wpi.first.deployutils.deploy.target.RemoteTarget;
import edu.wpi.first.deployutils.PredicateWrapper;
import edu.wpi.first.deployutils.ActionWrapper;

@CompileStatic
public class WPIJREArtifact extends MavenArtifact {
    private final String configName;

    public String getConfigName() {
        return configName;
    }

    public boolean isCheckJreVersion() {
        return checkJreVersion;
    }

    public void setCheckJreVersion(boolean checkJreVersion) {
        this.checkJreVersion = checkJreVersion;
    }

    private boolean checkJreVersion = true;

    private final String artifactLocation = "edu.wpi.first.jdk:roborio-2024:17.0.9u7-1"

    @Inject
    public WPIJREArtifact(String name, RemoteTarget target) {
        super(name, target);
        String configName = name + "frcjre";
        this.configName = configName;
        Project project = target.getProject();
        getConfiguration().set(project.getConfigurations().create(configName));
        getDependency().set(project.getDependencies().add(configName, artifactLocation));

        setOnlyIf(new PredicateWrapper({ DeployContext ctx ->
            return jreMissing(ctx) || jreOutOfDate(ctx) || project.hasProperty("force-redeploy-jre");
        }));

        getDirectory().set("/tmp");
        getFilename().set("frcjre.ipk");

        getPostdeploy().add(new ActionWrapper({ DeployContext ctx ->
            ctx.getLogger().log("Installing JRE...");
            ctx.execute("opkg remove frc*-openjdk*; opkg install /tmp/frcjre.ipk; rm /tmp/frcjre.ipk");
            ctx.getLogger().log("JRE Deployed!");
        }));
    }

    private boolean jreMissing(DeployContext ctx) {
        return ctx.execute("if [[ -f \"/usr/local/frc/JRE/bin/java\" ]]; then echo OK; else echo MISSING; fi").getResult().contains("MISSING");
    }

    private boolean jreOutOfDate(DeployContext ctx) {
        if (!checkJreVersion) {
            return false;
        }
        String version = getDependency().get().getVersion();
        CommandDeployResult cmdResult = ctx.execute("opkg list-installed | grep openjdk");
        if (cmdResult.getExitCode() != 0) {
            ctx.getLogger().log("JRE not found");
            return false;
        }
        String result = cmdResult.getResult().trim();
        ctx.getLogger().log("Searching for JRE " + version);
        ctx.getLogger().log("Found JRE " + result);
        boolean matches = result.contains(version);
        ctx.getLogger().log(matches ? "JRE Is Correct Version" : "JRE is mismatched. Reinstalling");
        return !matches;
    }
}
