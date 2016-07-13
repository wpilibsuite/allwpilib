# Contributing to ntcore

So you want to contribute your changes back to ntcore. Great! We have a few contributing rules that will help you make sure your changes will be accepted into the project. Please remember to follow the rules written here, and behave with Gracious Professionalism.

- [General Contribution Rules](#general-contribution-rules)
- [What to Contribute](#what-to-contribute)
- [Coding Guidelines](#coding-guidelines)
- [Submitting Changes](#submitting-changes)
    - [Pull Request Format](#pull-request-format)
    - [Merge Process](#merge-process)
- [Licensing](#licensing)

## General Contribution Rules

- Everything in the library must work for the 3000+ teams that will be using it.
- We need to be able to maintain submitted changes, even if you are no longer working on the project.
- Excluding bug fixes, changes in one language generally need to have corresponding changes in other languages.
    - Substantial changes often need to have corresponding LabVIEW changes. To do this, we will work with NI on these large changes.
    - Protocol changes must be carefully considered. Please open issues for protocol enhancements so we can discuss first.
- Changes should have tests.
- Code should be well documented.
    - This often involves ScreenSteps. To add content to ScreenSteps, we will work with you to get the appropriate articles written.

## What to Contribute

- Bug reports and fixes
    - We will generally accept bug fixes without too much question. If they are only implemented for one language, we will implement them for any other necessary languages. Bug reports are also welcome, please submit them to our GitHub issue tracker.
- While we do welcome improvements to the API, there are a few important rules to consider:
    - Features must be added to both the C++ API and the Java API, with rare exceptions.
    - During competition season, we will not merge any new feature additions. We want to ensure that the API is stable during the season to help minimize issues for teams.
    - Ask about large changes before spending a bunch of time on them! You can create a new issue on our GitHub tracker for feature request/discussion and talk about it with us there.
    - Features that make it easier for teams with less experience to be more successful are more likely to be accepted.
    - Features in ntcore should be broadly applicable to all teams. Anything that is team specific should not be submitted.

## Coding Guidelines

ntcore uses Google style guides for both C++ and Java. Autoformatters are available for many popular editors at [https://github.com/google/styleguide]. An online version of the styleguide for [C++](https://google.github.io/styleguide/cppguide.html) and [Java](https://google.github.io/styleguide/javaguide.html) are also available.  Additionally, offline copies of the style guide can be found in the style guide directory of the repository.

While the library should be fully formatted according to the styles, additional elements of the style guide were not followed when the library was initially created. All new code should follow the guidelines. If you are looking for some easy ramp-up tasks, finding areas that don't follow the style guide and fixing them is very welcome.

## Submitting Changes

### Pull Request Format

Changes should be submitted as a Pull Request against the master branch of ntcore. For most changes, we ask that you squash your changes down to a single commit. For particularly large changes, multiple commits are ok, but assume one commit unless asked otherwise. No change will be merged unless it is up to date with the current master. We will also not merge any changes with merge commits in them; please rebase off of master before submitting a pull request. We do this to make sure that the git history isn't too cluttered.

### Merge Process

When you first submit changes, Travis-CI will attempt to run `./gradlew check` on your change. If this fails, you will need to fix any issues that it sees. Once Travis passes, we will begin the review process in more earnest. One or more WPILib team members will review your change. This will be a back-and-forth process with the WPILib team and the greater community. Once we are satisfied that your change is ready, we will allow our Jenkins instance to test it. This will run the full gamut of checks, including integration tests on actual hardware. Once all tests have passed and the team is satisfied, we will merge your change into the WPILib repository.

## Licensing

By contributing to ntcore, you agree that your code will be distributed with ntcore, and licensed under the license for the ntcore project. You should not contribute code that you do not have permission to relicense in this manner. This includes code that is licensed under the GPL that you do not have permission to relicense, as ntcore is not released under a copyleft license. Our license is the 3-clause BSD license, which you can find [here](license.txt).