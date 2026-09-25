# **WPILib Governance Structure**

## **Background**

WPILib was originally developed by Brad Miller for the 2004-2005 IFI Robot Controllers used in *FIRST* Robotics Competition (FRC) as a library to abstract some of the low level detail of programming robots to allow students to tackle higher level challenges. The library was officially adopted by *FIRST* as a base provided to all teams programming in C++ starting with the NI cRIO control system in 2009\. While the source of the library was available in this era, development was still largely the product of Brad, WPI students, NI and *FIRST*. External contribution started to pick up in the roboRIO era, circa 2015, and accelerated to the state today where the vast majority of WPILib contribution comes from community developers.

This document aims to formalize some of the top level project governance processes without overly burdening the project with additional procedure or bureaucracy. The overall vision is to keep WPILib as an open source project driven by a robust community of developers, with top level guidance from *FIRST* and WPI to keep the project aligned with both the programs which serve as the primary use and with an overall view of the direction of robotics education.

## **Steering Council**

### **Purpose**

The Steering Council will be the ultimate authoritative decision-making body in the WPILib project for the purpose of:

* Setting broad project direction including long-term roadmaps, approving proposed Major changes each year, etc.
* Managing membership of the Core Developers
* Approving changes to core project guidelines such as Code of Conduct, Governance, Mission, and Privacy Policy
* Resolving any disputes


The goal is for the Steering Council to formally exercise these powers as little as is feasible, preferring instead to build consensus within the Core Developers and community whenever possible. When exercising these powers formally, the Steering Council should aim to do so by consensus, but may establish direction via majority vote if consensus cannot be reached.

### **Composition**

The Steering Council is composed of 3 members representing the diverse groups with substantial interest in the development of WPILib:

FIRST – 1 individual representing the interests of FIRST in WPILib through its use as the core programming library in the *FIRST* Robotics Competition and *FIRST* Tech Challenge (FTC) programs.

WPI – 1 individual representing the interests of Worcester Polytechnic University (WPI) which drove initial library development, continues to be reflected in the library name, and supplies a number of resources to support project activities.

WPILib Developers – 1 individual representing the community of WPILib developers.

The FIRST and WPI representatives will be selected by their respective organizations and will have no defined term limits. The WPILib Developer member will be selected via nomination and vote of the WPILib Core Developers prior to the FIRST Championship each year and will serve a one year term running through the end of the next FIRST Championship. If more than 3 candidates are nominated, a run-off of the top 3 candidates from the initial vote will be held to determine the representative. If a WPILib Developer representative voluntarily resigns prior to the end of the term, the same process is followed to select a representative for the remainder of the term. There is no limit to the number of consecutive terms a WPILib Developer representative may serve.

### **Current Membership**

The current membership of the Steering Council is as follows:

- FIRST Representative – Kevin O’Connor
- WPI Representative – Amanda Bessette
- WPILib Developer Rep – Peter Johnson

## ***FIRST*** **Control System Advisory Group**

This group will meet approximately weekly to track status updates of various hardware, software and rules development related to the control systems used in FRC and FTC and provide advisory opinions to the Steering Council. Its membership will consist of roughly 10-15 total individuals including the WPILib Steering Council above along with additional representatives from FIRST, WPILib, and additional technology partners. In recognition of the broader hardware and software ecosystem that WPILib exists in, it is expected that several members of this Advisory Council may not be active contributors to the WPILib codebase.

## **Core Developers**

WPILib Core Developers are the group of individuals doing most of the day-to-day work on WPILib development as well as developers of 3rd party projects which may be bundled with WPILib. This group of individuals will have access to the WPILib Slack workspace and may have review or commit permissions on one or more WPILib repositories either directly or as part of one or more individual area/project teams. Overall membership of this group, as well as membership of any area/project teams and specific permissions on any WPILib repositories is overseen by the Steering Council. Generally Core Developers must be over 18 though exceptions may be made in extraordinary circumstances. Members may be added to this group for reasons including but not limited to:

* Contribution to WPILib projects
* “Sponsored” developers such as *FIRST* interns, WPI student developers, etc.
* Contribution to 3rd party projects which are being incorporated into or distributed with WPILib
* Invitation by the Steering Council in order to increase developer capacity in a specific area of need or proposed strategic target

In order to maintain project stability and security, a small amount of “real world” vetting of Core Developers will be done by the Steering Council:

* Real name known by the Steering Council
* Youth Protection Policy Requirements compliant within *FIRST* Dashboard.
* Validation that they are who they say they are within the community. This may be via direct interaction with Steering Council or other Core Developers or indirectly via other known and trusted community members (e.g. Steering Council knows and trusts Mentor Susan who is in the same region as this candidate and validates they are who they say)

Core Developers meet \~weekly to discuss project status, current blocking items, etc.

## **WPILib Slack**

The WPILib Slack workspace is the primary internal coordination resource for WPILib development. This is a closed Slack Workspace whose membership consists of the Core Developers identified above along with members from other ecosystem partners who need to closely coordinate with WPILib (e.g. companies and third parties producing vendor libraries for use with WPILib). While this is a closed workspace, members will not generally be removed simply for becoming inactive as Core Developers. Members may be removed after becoming inactive in the workspace for greater than 1 year, or “for cause” if their continued participation in the workspace is deemed to be disruptive or detrimental to WPILib by the Steering Council.

## **Community Interaction**

WPILib Core Developers are allowed and encouraged to interact with the broader community of users and developers through any number of unofficial avenues including:

- In person interaction with teams via mentoring and/or event volunteering
- Forums such as the FTC Forum and Chief Delphi
- Chat platforms such as various Slack and Discord spaces

Core Developers should recognize that their interaction in these spaces may reflect on WPILib as a whole and should conduct themselves accordingly.

Official communication with developers is done through the WPILib Github Organization and its various projects including the use of Issues, Discussions and comments on Pull Requests.

Most information available to WPILib Core Developers via the WPILib Slack is not considered private and is fine to share with appropriate context if helpful when discussing particular features, decisions, etc. (though developers should avoid individual attribution of opinions such as “some people wanted to do X but John was strongly opposed”). Occasionally private or pre-release information may be available to Core Developers (e.g. early access to Usage Data, access to private repositories or documentation, etc.), data providers will attempt to clearly identify this information when shared and Core Developers should seek approval from the data source before discussing any such information publicly (or from the Steering Council if the source is a private repository in the WPILib Suite organization).

## **Development Process**

This section aims to provide general guidelines on the development process for WPILib and its various projects without introducing any unnecessary bureaucratic burden on processes that have generally been working well for many years without formal guidelines.

While there is no clear hard delineation between these categories, generally:

- Bug Fixes: fix unintended behavior of the library and cause little to no breakage to user code.
- Minor Changes: add API methods or classes that complement existing methods and classes in the same project and do not cause significant breakage to user code or change to user experience.
- Significant Changes: either affect a large number of files or lines of code in an internal way, or may cause significant breaking changes to user code scoped to one small area of the library consisting of several classes or less, or narrowly scoped changes to user experience (e.g. reorganize one subsection of documentation, or change one set of buttons or workflow in a particular application).
- Major changes: include things such as substantial new features, large re-writes of existing functionality, addition or removal of tools, or other changes with large implications on user code or experience.

### **Bug Fixes**

Bug fixes may be made by any developer, approved by any Core Developer with appropriate permissions on the associated project, and have no proposed minimum “dwell time” for feedback.

### **Minor Changes**

Minor changes may be made by any developer, approved by any Core Developer with appropriate permissions on the associated project and generally should remain open as a PR for a minimum of 24 hours to allow for any interested individuals to object with any concerns.

### **Significant Changes**

Significant changes proposed by non-Core Developers are recommended to solicit feedback via Issue or Discussion prior to performing any significant amount of work in order to ensure the proposed changes are aligned with the desired project direction. Significant changes, regardless of source, are encouraged to be submitted as Draft PRs in order to solicit feedback as early in the process as possible. Significant changes are encouraged to remain open for a minimum of:

- 24 hours after being marked as ready if feedback has been provided on the Draft and appears to be in consensus
- 72 hours if no feedback has yet been provided or if the feedback does not appear to be in consensus.

### **Major Changes**

Major changes should generally be developed using the following process:

1.  Proposed via GitHub Discussion, Core Team discussion on Slack or during meetings, or Steering Council discussion during meetings
2. Tracked via [Github Project board](https://github.com/orgs/wpilibsuite/projects)
3. Documented as a design doc in the [allwpilib repo](https://github.com/wpilibsuite/allwpilib/tree/main/design-docs) or other appropriate location for development and feedback. This should also include the proposed transition plan (timeline, deprecation vs hard-switch, etc.)

Because of the larger scope and implications on team experience Major Changes may take significantly longer to decide on, develop implementation, and reach readiness for incorporation than the other categories. It is not uncommon for Major Changes to take months or even multiple seasons to be fully implemented.
