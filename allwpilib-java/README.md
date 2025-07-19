# allwpilib-java

This is an umbrella project that pulls in all the Java projects as dependencies. The only "code" published is a module-info file that lists all the Java projects as transitive requirements. This allows user programs to have a single `import module wpilib;` directive to import _all_ top-level types from every WPILib module
