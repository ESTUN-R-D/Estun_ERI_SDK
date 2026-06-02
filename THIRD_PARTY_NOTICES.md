# Third-Party Notices

This public repository may reference third-party components in build scripts or
documentation, and official ESTUN SDK release packages may distribute
third-party components in binary or source form. Release maintainers should
verify the final contents of each published archive before distribution.

## log4cpp

- Project: log4cpp (Log for C++)
- Upstream site: https://log4cpp.sourceforge.net/
- Upstream project page: https://sourceforge.net/projects/log4cpp/
- License: LGPL 2.1 or later, according to the upstream project site
- Current official Linux release packaging baseline: `log4cpp` source package
  version `1.1.3-3` when the Linux release archive includes
  `liblog4cpp.so`, unless the release notes for a specific version state
  otherwise
- Typical use in official ESTUN SDK release packages: runtime logging library
  distributed as `log4cpp.dll` on Windows or `liblog4cpp.so` on Linux when
  such binaries are included in a release archive
- Packaging note: the public repository does not need to contain the log4cpp
  binary itself, but any official release archive that includes a log4cpp
  binary should also include the materials listed below

## Release Checklist

When an official release package includes `log4cpp` or any other third-party
component, the release package should also include:

- the applicable third-party license text;
- any copyright or attribution notices required by the upstream license;
- a record of the exact upstream version used in that release package;
- for LGPL-covered binaries, the exact corresponding source code for the
  distributed version, or a durable and clearly documented way to obtain that
  exact corresponding source code from the same release channel;
- any additional notices required for modified third-party binaries, if such
  modifications are distributed.

## Source Availability Guidance for log4cpp

If an official ESTUN SDK release package includes `log4cpp.dll`,
`liblog4cpp.so`, or another binary form of log4cpp, the associated GitHub
Release should also provide the exact corresponding log4cpp source code, or a
clearly labeled link in the same release notes pointing to the exact
corresponding source package for that same version.

If the distributed log4cpp binary is modified from upstream, the release
package should also identify that it is a modified build and preserve any
notices required by the LGPL for modified versions.

Update this file whenever new third-party components are added to the
repository or to the official release package contents.
