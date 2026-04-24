# Contributing to WPILib

So you want to contribute your changes back to WPILib. Great! We have a few contributing rules that will help you make sure your changes will be accepted into the project. Please remember to follow the rules in the [code of conduct](https://github.com/wpilibsuite/allwpilib/blob/main/CODE_OF_CONDUCT.md), and behave with Gracious Professionalism.

- [General Contribution Rules](#general-contribution-rules)
- [What to Contribute](#what-to-contribute)
- [Design Philosophy](#design-philosophy)
- [Contribution Process](#contribution-process)
- [Coding Guidelines](#coding-guidelines)
- [Submitting Changes](#submitting-changes)
    - [Pull Request Format](#pull-request-format)
    - [Merge Process](#merge-process)
- [Licensing](#licensing)

## General Contribution Rules

- Everything in the library must work for the 14000+ teams that will be using it.
- We need to be able to maintain submitted changes, even if you are no longer working on the project.
- Tool suite changes must be generally useful to a broad range of teams
- Excluding bug fixes, changes in one language generally need to have corresponding changes in other languages.
    - Some features, such the addition of C++26 for WPILibC or Functional Interfaces for WPILibJ, are specific to that version of WPILib only. New language features added to C++ must be wrappable in Python for [RobotPy](https://github.com/robotpy).
- Changes should have tests.
- Code should be well documented.
    - This involves writing tutorials and/or usage guides for your submitted feature. These articles are then hosted on the [WPILib](https://docs.wpilib.org/) documentation website. See the [frc-docs repository](https://github.com/wpilibsuite/frc-docs) for more information.

## What to Contribute

- Bug reports and fixes
    - We will generally accept bug fixes without too much question. If they are only implemented for one language, we will implement them for any other necessary languages. Bug reports are also welcome, please submit them to our GitHub issue tracker.
- While we do welcome improvements to the API, there are a few important rules to consider:
    - Features must be added to Java (WPILibJ), C++ (WPILibC), and Python with rare exceptions.
        - Most of Python (RobotPy) is created by wrapping WPILibC with pybind11 via semiwrap. In general, new user-facing functions or classes should have the proper wrapper configs updated, typically located in a YAML file with the same name as the header. See the [in-repo RobotPy README](./README-RobotPy.md) for more info and how to partially auto-update the configs. However, the command framework is reimplemented in Python, and requires code to be ported instead of being wrapped via semiwrap.
    - During competition season, we will not merge any new feature additions or removals. We want to ensure that the API is stable during the season to help minimize issues for teams.
    - Ask about large changes before spending a bunch of time on them! See [Contribution Process](#contribution-process) for where to ask.
    - Features that make it easier for teams with less experience to be more successful are more likely to be accepted.
    - Features in WPILib should be broadly applicable to all teams. Anything that is team specific should not be submitted.
    - As a rule, we are happy with the general structure of WPILib. We are not interested in major rewrites of all of WPILib. We are open to talking about ideas, but backwards compatibility is very important for WPILib, so be sure to keep this in mind when proposing major changes.
    - While the library may contain support for specific sensors, these are typically items contained in the FIRST Robotics Competition Kit of Parts or commonly used hardware identified by FIRST or core WPILib Developers. If you think a certain sensor should be supported in WPILib, you may submit an issue justifying the reasons why it should be supported and approval will be determined by FIRST or core WPILib Developers. If you are a company interested in getting a sensor into the Kit of Parts, please contact FIRST directly at frcparts@firstinspires.org.

## Design Philosophy

WPILib's general design philosophy strays far away from the traditional Object-Oriented Programming architectures dominant in enterprise codebases. The general points to follow for WPILib are as follows:

- Prefer functions and composition over inheritance. Inheritance is rigid and often prevents evolution, as adding or removing methods from an inherited class risks breakage. For similar reasons, functional interfaces (`std::function` in C++) are preferred over actual interfaces.
- Avoid opaque black-boxes of functionality. Classes like RamseteCommand or HolonomicDriveController (both removed in 2027) are good examples of this. While they look like a good abstraction that helps beginners, the black-box nature means they are [difficult to debug](https://github.com/wpilibsuite/allwpilib/issues/3350) and it's impossible to instrument the internals to figure out what's going on, or they are extremely clunky to use compared to composing the individual components (thus defeating the point of abstracting it away; SwerveControllerCommand construction was [a huge pile of opaque arguments glued together](https://github.com/wpilibsuite/allwpilib/blob/v2026.2.2/wpilibjExamples/src/main/java/edu/wpi/first/wpilibj/examples/swervecontrollercommand/RobotContainer.java#L104-L114)). Composition is strongly preferred, with strong documentation and examples describing how to do that composition.
- Error at compile time, not runtime. Despite our best efforts, there will always be people who don't read stack traces (understandable for beginner programmers). Compile time errors show up in builds and in an IDE, which is much easier and faster for people to pinpoint and debug. Use language features to make invalid code impossible to build.
    - The Matrix class in Java is an example of this. While clunky due to Java's weak generics system, it enforces correct Matrix dimensions at compile time, with the MatBuilder factory method throwing if the array passed in is the wrong size, which leads to the next point:
- Try to only throw exceptions at code startup, and only for things that are obviously incorrect. Robots shouldn't quit, and it's a real "feels bad" moment when yours does, especially in a match. It's oftentimes better to have a robot continue running when it sees nonsensical state as opposed to outright crashing, since other components are often still functional. If you can't make invalid code a compile time error, throwing at the start of the robot program is the next best solution, but avoid throwing in functions likely to be called throughout a robot's runtime.
    - Sometimes the behavior of functions are just incorrect if invalid data is passed in, and throwing is one of the only options. This is a judgement call, but if there are no other options, throwing can be okay.
        - An alternative to throwing is logging an error, typically with [the Alerts framework](https://docs.wpilib.org/en/latest/docs/software/telemetry/persistent-alerts.html); this is a good choice for runtime errors. Also see https://github.com/wpilibsuite/allwpilib/issues/6766 for an example of not throwing exceptions, but simply logging an error on invalid data.
    - Note that hardware configuration issues such as a sensor not existing isn't necessarily obviously incorrect; it could be that the wrong port was specified, but it could also be unplugged due to external factors. Throwing just because it's unplugged can make for a "feels bad" moment, and should be avoided.

## Contribution Process

Have an idea to make WPILib better? Here's some steps to go from idea to implementation:

1. (Optional) **Discuss it in the Discord.** The programming discussion channel in the [Unofficial FIRST Robotics Competition Discord Server](https://discord.com/invite/frc) is a popular choice for initial discussion about ideas because many WPILib developers are active there and the live messaging nature of the platform is well suited for initial discussion (particularly for smaller changes). Note that the unofficial Discord server is not a mandatory step in the development process and is not endorsed by FIRST®.
2. (Recommended) **Open a GitHub issue.** GitHub issues are another way to get initial feedback about an idea before working on an implementation. Compared to the unofficial Discord server, GitHub issues have much wider visibility and are better suited for serious discussions about major changes. Getting feedback about an idea (whether in the unofficial Discord server or in a GitHub issue) before working on the implementation is recommended to avoid working on a change that will be rejected, though some ideas are pretty safe.
3. (Rare) **Create a design document in GitHub.** Sometimes, a change is so large that a design document is necessary to fully flesh out the details (and get feedback on them) before starting on an implementation. This is done through a pull request that adds the design document (as a Markdown file) to the repository. This is extremely rare, and it is sometimes done concurrently with the implementation if the change doesn't need much debate but is large enough to require a design document.
4. (Mandatory) **Create a GitHub pull request.** This is how you implement the changes, and is the focus of most of the rest of this document.

## Coding Guidelines

WPILib uses modified Google style guides for both C++ and Java, which can be found in the [styleguide repository](https://github.com/wpilibsuite/styleguide). Autoformatters are available for many popular editors at https://github.com/google/styleguide. Running wpiformat is required for all contributions and is enforced by our continuous integration system.
While the library should be fully formatted according to the styles, additional elements of the style guide were not followed when the library was initially created. All new code should follow the guidelines. If you are looking for some easy ramp-up tasks, finding areas that don't follow the style guide and fixing them is very welcome.

## Submitting Changes

### Pull Request Format

Changes should be submitted as a Pull Request against the main branch of WPILib. For most changes, commits will be squashed upon merge. For particularly large changes, multiple commits are ok, but assume one commit unless asked otherwise. We may ask you to break a PR into multiple standalone PRs or commits for rebase within one PR to separate unrelated changes. No change will be merged unless it is up to date with the current main branch. We do this to make sure that the git history isn't too cluttered.

Particularly large and/or breaking changes should be targeted to the 2027 branch, which targets the [Systemcore Robot Controller](https://community.firstinspires.org/introducing-the-future-mobile-robot-controller). The intent is minimize changes for 2026, to allow development to focus on preparing for 2027.

### Merge Process

When you first submit changes, GitHub Actions will attempt to run `./gradlew check` on your change. If this fails, you will need to fix any issues that it sees. Once Actions passes, we will begin the review process in more earnest. One or more WPILib team members will review your change. This will be a back-and-forth process with the WPILib team and the greater community. Once we are satisfied that your change is ready, we will allow our hosted instance to test it. This will run the full gamut of checks, including integration tests on actual hardware. Once all tests have passed and the team is satisfied, we will merge your change into the WPILib repository.

## Licensing

By contributing to WPILib, you agree that your code will be distributed with WPILib, and licensed under the license for the WPILib project. You should not contribute code that you do not have permission to relicense in this manner. This includes code that is licensed under the GPL that you do not have permission to relicense, as WPILib is not released under a copyleft license. Our license is the 3-clause BSD license, which you can find [here](LICENSE.md).
