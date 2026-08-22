# Contributing Guidelines

## Ways to Contribute

Note that this file only contains a guide for **code contributions**. However,
there are many other ways how to contribute to the LibrePCB project, see
**[librepcb.org/contribute](https://librepcb.org/contribute/)** for details.

## Notes

- **Before spending lots of time on something, please ask for feedback on
  your idea first!** Also note that we do not accept major changes
  (e.g. changes affecting the file format) at any time, see details in our
  [development workflow documentation](https://developers.librepcb.org/da/dbc/doc_release_workflow.html#doc_release_workflow_branches).
- Please search issues and pull requests before adding something new to avoid
  duplicating efforts and conversations.
- We use some labels to mark issues as suitable for contributors, check those
  out to find something to work on:
  - [`help wanted`](https://github.com/LibrePCB/LibrePCB/labels/help%20wanted)
  - [`easy`](https://github.com/LibrePCB/LibrePCB/labels/easy)
- To contact us, use one of the options listed at https://librepcb.org/help/.

## Use of AI/LLM/Agents

I, [@ubruhin](https://github.com/ubruhin), am tired of reading LLM generated
text or code slop and don't want to waste time with it. Therefore I kindly
ask you to follow some **important** rules about the use of LLM / agents:

- Do not use LLM for writing pull request descriptions, issues, or comments
  on those. If I want to chat with an LLM, I will do so on myself. In issues
  and pull requests, I want to chat with humans.
- Any usage of LLM has to be declared clearly. No matter if you write a comment
  referencing "knowledge" you gained through an LLM, or if you used an LLM to
  write some of the code of a pull request - make it transparent what you have
  used it for.
- Any output from an LLM (no matter whether "knowledge" or code) has to be
  verified **in detail** by yourself. It is not my job to correct what the LLM
  told you wrong or coded wrong.

To summarize: Use of LLMs is not forbidden when contributing to LibrePCB. Just
use your brain to make sure no AI slop is ending up in issues or pull requests.

As soon as I suspect the use of an LLM without following those rules, I'll
stop the discussion and close the pull request or issue.

## Getting Started

- Make sure you have a [GitHub account](https://github.com/signup/free).
- Open a new issue for your idea, assuming one does not already exist.
- Fork the repository on GitHub.
- Have a look at our
  [development resources](https://github.com/LibrePCB/LibrePCB/tree/master/dev),
  especially at the [developers documentation](https://developers.librepcb.org/).
- When using QtCreator, import and use our
  [code style guide file](https://github.com/LibrePCB/LibrePCB/blob/master/dev/CodingStyle_QtCreator.xml).

## Making Changes

- Create a topic branch from where you want to base your work.
  - This is usually the master branch.
  - To quickly create a topic branch based on master:
    `git checkout -b my_contribution master`
  - Please avoid working directly on the `master` branch.
- Write code which follows our
  [code style guides](https://developers.librepcb.org/df/d24/doc_code_style_guide.html)
  and [.editorconfig settings](https://github.com/LibrePCB/LibrePCB/blob/master/.editorconfig).
  - You can use [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to
    automatically format the code. To format all files at once, just run the
    script [`./dev/format_code.sh`](dev/format_code.sh).
  - Our CI will also run a static code analysis on every pull request. To run
    it locally, call [`./dev/clang_tidy.sh`](dev/clang_tidy.sh) (Linux only).
- Make commits of logical units.
  - Make sure your commit messages are in the
    [proper format](http://chris.beams.io/posts/git-commit/):
    ```
    ScopeGuardList: Fix crash when constructing with size

    Default constructed std::function is empty and throws an
    std::bad_function_call when being called.

    Check if it is empty and use reserve() when constructing with size.

    Fixes #62
    ```
- Make sure you have added the necessary tests for your changes.
- Run all tests to ensure nothing else was accidentally broken.
  - This is done by running the binary
    `./build/tests/unittests/librepcb-unittests`.

## Submitting Changes

- Push your changes to a topic branch in your fork of the repository.
- Submit a pull request to the repository in the LibrePCB organization.
- We will then check the pull request and give you feedback quickly.

Please also take a look at our
[Pull Request Guidelines](https://developers.librepcb.org/df/d30/doc_developers.html#doc_developers_pullrequests).

## Additional Resources

- [General GitHub documentation](https://help.github.com/)
- [GitHub pull request documentation](https://help.github.com/send-pull-requests/)
