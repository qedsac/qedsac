# Build/Install Instructions for MS Windows

[**Note:**
The following instructions are preliminary, and at present, deal only
with building a `qedsac.exe` executable from the command line.
]

The Visual Studio C++ installation includes two C/C++ compilers, `cl`
and `clang++`.  The Qt Windows installation includes various flavors
of the `mingw` compiler.  In the `windows` subdirectory of the `qedsac`
package directory, we provide batch files for each of these:

- `cl-build.bat`
- `clang32-build.bat`
- `ming32-build.bat`

along with `@`-files specifying the location of include files and the
Qt libraries:

- `cl-include-options` & `cl-library-options`
- `clang32-include-options` & `clang32-library-options`
- `ming32-include-options` & `ming32-library-options`

These produce 32-bit executables, which will also run on 64-bit
versions of Windows.

The batch files must be executed from the `windows` subdirectory in
the `qedsac` package directory.

The compilers themselves must be accessible via the `%PATH%` variable.
Both the Visual Studio and Qt installations provide command-line
environments with `%PATH%` properly set for the `cl`/`clang` and
`mingw` compilers, respectively.

Depending on where you installed Qt, you may have to modify both the
batch file and the `@`-files you want to use.  If you want a 64-bit
executable, you will need to make similar modifications.

You will need to copy the contents of the `src\data` and `src\doc`
directories into your `Documents` folder.  The command
```
xcopy /s ..\src\doc\*.* ..\src\data\*.* %homepath%\Documents
```
should accomplish this.  If you prefer to put these files elsewhere,
you'll need to modify the lines in the batch file that set the `%doc%`
and `%data%` variables appropriately.

Ideally, there should be a Windows-specific `make` file and
`configure.bat` utility to deal with file dependencies and
configuration options (as in the Unix/Linux instructions).  Also
ideally, it should be possible to automatically create a `setup.exe`
installation program.  These features will have to wait until we can
find someone willing (and able) to develop them.
