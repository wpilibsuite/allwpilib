# DataLog Tool

Run DataLog Tool with one or more datalog filenames to open them on startup:

```text
datalogtool "C:\Robot Logs\first.wpilog" "C:\Robot Logs\second.wpilog"
```

On Windows, you can also drag files onto the application icon or a shortcut to
open them. Running without filenames opens the tool normally.

Use `--save-dir <directory>` to select the directory for the tool's settings:

```text
datalogtool --save-dir "C:\DataLog Settings" "C:\Robot Logs\first.wpilog"
```

The settings directory was previously a positional argument; it now requires
`--save-dir`. Use `--` to stop option parsing if a filename matches an option.
