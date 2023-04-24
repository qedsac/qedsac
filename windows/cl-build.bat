@path %path%;c:\Qt\5.15.2\msvc2019\bin
rcc ..\src\resources.qrc > ..\src\resources.cc

type ..\VERSION | set /p version=
echo #define PACKAGE_VERSION "0.8b" > config.h

set data=%homepath:\=/%/Documents
set doc=%homepath:\=/%/Documents

cl /Fe:qedsac.exe @cl-include-options /Za /DDATA_DIR=\"%data%\" /DDOC_DIR=\"%doc%\" ..\src\*.cc @cl-library-options 
