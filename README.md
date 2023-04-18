# README

QEdsac is a Qt/C++ translation of Martin Campbell-Kelly's [original
EDSAC simulator](https://www.dcs.warwick.ac.uk/~edsac).  This open source
distribution is available at [GitHub](https://github.com/qedsac/qedsac).

There are only minor differences from the original, mostly cosmetic,
some due to Qt requirements, others are minor "improvements."

The latest version (as of 18 April 2023) is 0.8b.

We encourage those interested in (and capable of) producing binary
distributions (Linux .deb/.rpm, Mac OS X .pkg, etc.) to do so, and make
them available in public repositories.  We are particularly interested
in finding someone who is interested in producing and making available
a standalone version for Windows using either MingGW or Visual Studio
(only Cygwin-based binaries have been built, as yet).

Questions, comments, suggestions, bug reports, etc. should be reported
to [qedsac@outlook.com](mailto:qedsac@outlook.com).

## Downloading
There are three ways to download the `qedsac` source code into a
"package directory," from which an executable can be built.

### Git Command Line (preferred)
The simplest method is to use the `git` command line:
```
	git clone https://github.com/qedsac/qedsac.git
```
This creates a package directory named `qedsac`.

This method has the advantage that the source code can be updated to
the latest version simply by invoking the command
```
	git pull
```
in the package directory.

### GitHub Website
Alternatively, you can choose "Downoad ZIP" from the "Code" menu on
the `qedsac` [GitHub page](https://github.com/qedsac/qedsac).  This
method downloads a file named `qedsac-main.zip`, which when unzipped,
creates a package directory named `qedsac-main`.

### Source Code Tarball
Some sites may provide a `tar` archive named `qedsac-x.y.tar.gz`
(where *x.y* is the version number).  The command
```
	tar xvf qedsac-x.y.tar.gz
```
(with the appropriate version number substituted for *x.y*) creates a
package directory named `qedsac-x.y`.

## Compiling and Installation
See the file [`INSTALL.md`](INSTALL.md) for detailed instructions.
