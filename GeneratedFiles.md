# Maintaining Generated Files
WPILib extensively uses [metaprogramming](https://en.wikipedia.org/wiki/Metaprogramming#Code_generation) to generate code that would otherwise be tedious and error-prone to maintain. We use [Jinja](https://jinja.palletsprojects.com), a templating engine with a Python API, alongside JSON files that contain data to generate code. This document explains how to maintain these generated files and create new ones.

## File hierarchy
The Python script used to generate a subproject's files will always be located in the subproject's directory, e.g. wpilibc. It will always be called `generate_<thing>.py` where `<thing>` is the name for what you're generating.

The templates will be located under `subproject/src/generate/main`, and generated files will be located under `subproject/src/generated/main`.

If the generated file is for C++, the hierarchy should be symmetrical, so if a generated header is located under `subproject/src/generated/main/native/include/frc/header.h`, the template to generate it should be located under `subproject/src/generate/main/native/include/frc/template.h.jinja`. You should pretend like `subproject/src/generate/main` is just like `subproject/src/main`, in that the file hierarchy must make sense if the files weren't generated, e.g, headers that would go in `subproject/src/main/native/include/blah` should be in `subproject/src/generated/main/native/include/blah`.

If the generated file is for Java, templates should be located under `subproject/src/generate/main/java`, and the hierarchy for output files should reflect the declared package of the output Java files. For example, a Jinja template at `subproject/src/main/java/template.java.jinja` with the package `edu.wpi.first.wpilibj` would be used to generate Java files located at `subproject/src/generated/main/java/edu/wpi/first/wpilibj`

The JSON files live under `subproject/src/generate` since they apply to both languages. One unique case is JSON files that are used by multiple subprojects, currently only JSON files shared by wpilibc and wpilibj. In that specific case, the JSON files will always be located in wpilibj since Java is the most used language.

## Using code generation
If you've identified a set of files which are extremely similar, one file with lots of repetitive code, or both, you can create Jinja templates, a JSON file, and a Python script to automatically generate the code instead.

### Preparing files for codegen
Once you've identified the files you want to codegen, you will need to identify parts of code that are similar, and extract the data that's different. Code needs to go into your Jinja template, while data that will be used to fill in the template goes into a JSON file. Using game controllers as an example, they have lots of similar methods to read the value of a button, check if a button has been pressed since the last check, and check if a button has been released since the last check. Those methods are code that goes in a Jinja template, with the specific button replaced with a Jinja expression. The buttons, both the name and value, go into a JSON file.

### Writing a Python script
To maintain consistency with other Python scripts, copy an existing `generate_*.py` script. [generate_pwm_motor_controllers.py](./wpilibj/generate_pwm_motor_controllers.py) is a good start, since it's relatively basic. Modify the script to reference your templates and JSON file, modify the paths so the files end up in the right place, and rename the functions so they match what you're generating. An important part of the script is to give the files the correct name. Depending on files you're generating, this could be the name of the template itself (see [ntcore/generate_topics.py](./ntcore/generate_topics.py)) or it could be part of the data in the JSON file.

### (Re)Generating files and committing them
Once your Python script is complete, you can run `python generate_<thing>.py` to generate the files. Once you're finished with your files, commit these files to Git. If you regenerated the files and Git indicates the files have changed, but the diff doesn't show any changes, only the line endings have changed. If you expected changes to the generated code, you didn't correctly make changes. If you didn't expect changes, you can ignore this and discard the changes. Also ensure that you've marked the Python script as executable, since this is necessary for CI workflows to run your scripts. To add your script to the CI workflows, edit [.github/workflows/pregen_all.py](.github/workflows/pregen_all.py), and add your script alongside the rest of the scripts.
