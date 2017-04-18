# Design Docs Guidelines

In this folder are design documents that describe new feature additions to WPILib. These documents are submitted before significant work on a new feature is started, so that the community has time to examine, criticize, and improve on features before they are added to WPILib. Please follow the guidelines outlined here when submitting a design-doc pull request.

## Format

Design docs should be written in a markup language understood by the GitHub rendering engine, with an appropriate extension for proper rendering. Either markdown or asciidoc are preferred. Documents should have a short, descriptive name, such as `hal_resouce_management.md`, with underlines instead of spaces in the name. All design documents should be added to the master list in this README, with a short 1-2 sentence description. There is no one standard format for the design document itself, but documents should answer the following questions:

- What feature being added?
- How will this feature benefit teams?
- How will this feature be implemented? Be as detailed as you can.
- Where in the libraries will this change be made?

The second question is of particular importance. What problem are teams currently having, and how will this address it? Or, if this is not a team-facing change (such as changes to the HAL), how will this indirectly benefit teams? For example, moving resource allocation into the HAL will simplify resource management code from a library perspective, and help to unify currently fragmented code spread out across 3 code bases.

## Approval Process

Anyone is welcome to submit design documents to this project. After a pull request is submitted, the WPILib developers and interested community members will review it, making suggestions and comments as appropriate. If we decide to approve the pull request, we'll merge it in and work on the new feature can be submitted in earnest. Note that not all design documents will be accepted; if we feel that a feature is not appropriate for WPILib, we will not merge the design document.

# Submitted Design Documents

This is a list of all previously submitted and approved design documents:a

- TODO: Have design documents
