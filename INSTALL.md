# Build/Install Instructions for Unix/Linux

Depending on the download method you used, the `qedsac` project code
will be under a directory named either `qedsac` (`git` command-line), 
`qedsac-main` (GitHub zip archive), or `qedsac-x.y` ("tarball"
archive, version *x.y*).  Use the `cd` command to navigate to this
directory (the "package directory").

Continue with either "System-wide Installation" or "Single-user
Installation," below.  The configuration utility checks that the
necessary libraries and utilities are installed, and it will complain
if they are missing.  To build `qedsac`, you will need a C++11 compiler
(`g++` is assumed), the Qt5 Gui, Widgets, Multimedia and PrintSupport
libraries, the Qt resource compiler (`rcc`), and the `pkgconfig` utility.
The last is usually installed by default on Linux.

The known commands to install the necessary libraries on various Unices are:

- Fedora/RedHat/CentOS
```
	sudo dnf install qt5-qtbase-devel qt5-qtmultimedia-devel
```

- Ubuntu/Debian
```
	sudo apt-get installqtbase5-dev qtmultimedia5-dev
```

- Mac OS X
```
	sudo port install qt5
	sudo port install pkgconfig
```


## System-wide Installation

Run the configuration utility, build the package, and (as root)
install everything:
```
	./configure
	make clean
	make all
	sudo make install
```
This will install the `qedsac` binary in `/usr/local/bin`, and the data
files under `/usr/local/share/qedsac`.  If you want the files elsewhere
than under `/usr/local`, you can use configure's `--prefix` option to
specify a different location.  For example, if you prefer the package
installed under `/opt`, you would run
```
	./configure --prefix=/opt
```
The configuration utility is almost infinitely adaptable.  For a
description of all possible command-line options, run
```
	./configure --help
```
The package can be removed from your system by running the command
```
	sudo make uninstall
```
in the package directory.

## Single-user Installation

To install `qedsac` only in your own account area, use the `--prefix`
options as follows:
```
	./configure --prefix="$HOME"
	make clean
	make all
	make install
```
The binary will be installed in `$HOME/bin` and the data under
`$HOME/share/qedsac`.  Note that not all systems include `$HOME/bin` in
the default `PATH`.  If this is the case, simply add the line
```
	PATH=$PATH:$HOME/bin
```
to your `.bash_profile` file (or the equivalent file for your shell).

The package can be removed from your account by running the command
```
	make uninstall
```
in the package directory.

## Notes

The above instructions have been thoroughly tested under Linux, and
*should* work on any Unix-like system.  They have also been minimally
tested on Mac OS X and MS Windows (using Cygwin).

### Linux

For GUI desktop access, the configuration utility creates a (currently
experimental) `qedsac.desktop` file in the "linux" subdirectory.
Simply install the `.desktop` file appropriately for your distro.
For some (all?) distros the command
```
	xdg-desktop-menu install --novendor linux/qedsac.desktop
```
(after `make install`) does the trick.

### Mac OS X

On Mac OS X, a `QEdsac.app` directory is required in the `/Applications`
folder.  A sample such folder, which seems to work, is provided in the
`macos` directory.  You'll need to copy `macos/QEdsac.app` to
`/Applications`, and provide a link to the actual binary:
```
	cp -r macos/QEdsac.app /Applications
	ln -s /usr/local/bin/qedsac /Applications/QEdsac.app/Contents/MacOS/QEdsac
```
(If you have installed the binary elsewhere than in `/usr/local/bin`,
substitute the actual location for `/usr/local/bin` in the `ln` command.)
