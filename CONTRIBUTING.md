# Contributing to WPILib

So you want to contribute your changes back to WPILib. Great! We have a few contributing rules that will help you make sure your changes will be accepted into the project. Please remember to follow the rules in the [code of conduct](https://github.com/wpilibsuite/allwpilib/blob/main/CODE_OF_CONDUCT.md), and behave with Gracious Professionalism.

- [General Contribution Rules](#general-contribution-rules)
- [What to Contribute](#what-to-contribute)
- [Contribution Process](#contribution-process)
- [Coding Guidelines](#coding-guidelines)
- [Submitting Changes](#submitting-changes)
    - [Pull Request Format](#pull-request-format)
    - [Merge Process](#merge-process)
- [Licensing](#licensing)

## General Contribution Rules

- Everything in the library must work for the 4000+ teams that will be using it.
- We need to be able to maintain submitted changes, even if you are no longer working on the project.
- Tool suite changes must be generally useful to a broad range of teams
- Excluding bug fixes, changes in one language generally need to have corresponding changes in other languages.
    - Some features, such the addition of C++23 for WPILibC or Functional Interfaces for WPILibJ, are specific to that version of WPILib only. New language features added to C++ must be wrappable in Python for [RobotPy](https://github.com/robotpy).
    - Substantial changes often need to have corresponding LabVIEW changes. To do this, we will work with NI on these large changes.
- Changes should have tests.
- Code should be well documented.
    - This involves writing tutorials and/or usage guides for your submitted feature. These articles are then hosted on the [WPILib](https://docs.wpilib.org/) documentation website. See the [frc-docs repository](https://github.com/wpilibsuite/frc-docs) for more information.

## What to Contribute

- Bug reports and fixes
    - We will generally accept bug fixes without too much question. If they are only implemented for one language, we will implement them for any other necessary languages. Bug reports are also welcome, please submit them to our GitHub issue tracker.
- While we do welcome improvements to the API, there are a few important rules to consider:
    - Features must be added to Java (WPILibJ), C++ (WPILibC), with rare exceptions.
        - Most of Python (RobotPy) is created by wrapping WPILibC with pybind11 via robotpy-build. However, new features to the command framework should also be submitted to [robotpy-commands-v2](https://github.com/robotpy/robotpy-commands-v2) as the command framework is reimplemented in Python.
    - During competition season, we will not merge any new feature additions. We want to ensure that the API is stable during the season to help minimize issues for teams.
    - Ask about large changes before spending a bunch of time on them! See [Contribution Process](#contribution-process) for where to ask.
    - Features that make it easier for teams with less experience to be more successful are more likely to be accepted.
    - Features in WPILib should be broadly applicable to all teams. Anything that is team specific should not be submitted.
    - As a rule, we are happy with the general structure of WPILib. We are not interested in major rewrites of all of WPILib. We are open to talking about ideas, but backwards compatibility is very important for WPILib, so be sure to keep this in mind when proposing major changes.
    - Generally speaking, we do not accept code for specific sensors. We have to be able to test the sensor in hardware on the WPILib test bed. Additionally, hardware availability for teams is important. Therefore, as a general rule, the library only directly supports hardware that is in the Kit of Parts. If you are a company interested in getting a sensor into the Kit of Parts, please contact FIRST directly at frcparts@firstinspires.org.

## Contribution Process

Have an idea to make WPILib better? Here's some steps to go from idea to implementation:

1. (Optional) **Discuss it in the Discord.** The programming discussion channel in the [Unofficial FIRST Robotics Competition Discord Server](https://discord.com/invite/frc) is a popular choice for initial discussion about ideas because many WPILib developers are active there and the live messaging nature of the platform is well suited for initial discussion (particularly for smaller changes). Note that the unofficial Discord server is not a mandatory step in the development process and is not endorsed by FIRST®.
2. (Recommended) **Open a GitHub issue.** GitHub issues are another way to get initial feedback about an idea before working on an implementation. Compared to the unofficial Discord server, GitHub issues have much wider visibility and are better suited for serious discussions about major changes. Getting feedback about an idea (whether in the unofficial Discord server or in a GitHub issue) before working on the implementation is recommended to avoid working on a change that will be rejected, though some ideas are pretty safe.
3. (Rare) **Create a design document in GitHub.** Sometimes, a change is so large that a design document is necessary to fully flesh out the details (and get feedback on them) before starting on an implementation. This is done through a pull request that adds the design document (as a Markdown file) to the repository. This is extremely rare, and it is sometimes done concurrently with the implementation if the change doesn't need much debate but is large enough to require a design document.
4. (Mandatory) **Create a GitHub pull request.** This is how you implement the changes, and is the focus of most of the rest of this document.

## Coding Guidelines

WPILib uses modified Google style guides for both C++ and Java, which can be found in the [styleguide repository](https://github.com/wpilibsuite/styleguide). Autoformatters are available for many popular editors at https://github.com/google/styleguide. Running wpiformat is required for all contributions and is enforced by our continuous integration system.
While the library should be fully formatted according to the styles, additional elements of the style guide were not followed when the library was initially created. All new code should follow the guidelines. If you are looking for some easy ramp-up tasks, finding areas that don't follow the style guide and fixing them is very welcome.

### Math documentation

When writing math expressions in documentation, use https://www.unicodeit.net/ to convert LaTeX to a Unicode equivalent that's easier to read. Not all expressions will translate (e.g., superscripts of superscripts) so focus on making it readable by someone who isn't familiar with LaTeX. If content on multiple lines needs to be aligned in Doxygen/Javadoc comments (e.g., integration/summation limits, matrices packed with square brackets and superscripts for them), put them in @verbatim/@endverbatim blocks in Doxygen or `<pre>` tags in Javadoc so they render with monospace font.

The LaTeX to Unicode conversions can also be done locally via the unicodeit Python package. To install it, execute:
```bash
pip install --user unicodeit
```

Here's example usage:
```bash
$ python -m unicodeit.cli 'x_{k+1} = Ax_k + Bu_k'
xₖ₊₁ = Axₖ + Buₖ
```

On Linux, this process can be streamlined further by adding the following Bash function to your .bashrc (requires `wl-clipboard` on Wayland or `xclip` on X11):
```bash
# Converts LaTeX to Unicode, prints the result, and copies it to the clipboard
uc() {
  if [ $WAYLAND_DISPLAY ]; then
    python -m unicodeit.cli $@ | tee >(wl-copy -n)
  else
    python -m unicodeit.cli $@ | tee >(xclip -sel)
  fi
}
```

Here's example usage:
```bash
$ uc 'x_{k+1} = Ax_k + Bu_k'
xₖ₊₁ = Axₖ + Buₖ
```

## Submitting Changes

### Pull Request Format

Changes should be submitted as a Pull Request against the main branch of WPILib. For most changes, commits will be squashed upon merge. For particularly large changes, multiple commits are ok, but assume one commit unless asked otherwise. We may ask you to break a PR into multiple standalone PRs or commits for rebase within one PR to separate unrelated changes. No change will be merged unless it is up to date with the current main branch. We do this to make sure that the git history isn't too cluttered.

Particularly large and/or breaking changes should be targeted to the 2027 branch, which targets the [SystemCore Robot Controller](https://community.firstinspires.org/introducing-the-future-mobile-robot-controller). The intent is minimize changes for 2026, to allow development to focus on preparing for 2027.

### Merge Process

When you first submit changes, GitHub Actions will attempt to run `./gradlew check` on your change. If this fails, you will need to fix any issues that it sees. Once Actions passes, we will begin the review process in more earnest. One or more WPILib team members will review your change. This will be a back-and-forth process with the WPILib team and the greater community. Once we are satisfied that your change is ready, we will allow our hosted instance to test it. This will run the full gamut of checks, including integration tests on actual hardware. Once all tests have passed and the team is satisfied, we will merge your change into the WPILib repository.

## Licensing

By contributing to WPILib, you agree that your code will be distributed with WPILib, and licensed under the license for the WPILib project. You should not contribute code that you do not have permission to relicense in this manner. This includes code that is licensed under the GPL that you do not have permission to relicense, as WPILib is not released under a copyleft license. Our license is the 3-clause BSD license, which you can find [here](LICENSE.md).
