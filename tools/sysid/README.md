# SysId: System Identification for Robot Mechanisms

## Building and Running SysId

See [here](../README.md#Requirements) for build requirements.

Run the following in the monorepo root.
```bash
./gradlew sysid:run
```

## Troubleshooting

Use [AdvantageScope](https://docs.wpilib.org/en/stable/docs/software/dashboards/advantagescope.html) (shipped with the WPILib installer) to view .wpilog files for troubleshooting when SysId fails to generate plots.
