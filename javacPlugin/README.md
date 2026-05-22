# javacPlugin

A javac plugin for use in WPILib and WPILib robot projects. Combined with [wpiannotations](../wpiannotations/README.md), this plugin analyzes source code to ensure the constraints laid out by the specified annotations are followed (to the extent static analysis allows) in order to reduce user mistakes.

## Design guidelines

This plugin should only be used for static analysis, not to enhance the Java language with syntax features. Adding additional syntax features is outside the scope of this plugin and can be confusing to beginners.
