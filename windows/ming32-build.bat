:: path change in next line not necessary when run from Qt mingw window
::@path %path%;c:\Qt\5.15.2\mingw81_32\bin
rcc ..\src\resources.qrc > ..\src\resources.cc

type ..\VERSION | set /p version=
echo #define PACKAGE_VERSION "0.8b" > config.h

set data=%homepath:\=/%/Documents
set doc=%homepath:\=/%/Documents

g++ -O2 -o qedsac.exe @ming32-include-options -DDATA_DIR=\"%data%\" -DDOC_DIR=\"%doc%\" ../src/*.cc @ming32-library-options 
