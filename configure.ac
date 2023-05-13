#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([Qt-based EDSAC Simulator],[0.9],[],[qedsac])
: ${CXXFLAGS="-std=c++11 -g -O3"}
# following line requires config.sub & config.guess files in distro
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile 
                 src/Makefile])

AC_PREFIX_DEFAULT([/usr/local])

# Checks for programs.
AC_PROG_CC
AC_LANG([C++])
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL

AC_MSG_CHECKING(whether fPIC compiler option is accepted)
SAVED_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -fPIC -Werror"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [return 0;])],
    [AC_MSG_RESULT(yes)
     CXXFLAGS="$SAVED_CXXFLAGS -fPIC"],
    [AC_MSG_RESULT(no)
     CXXFLAGS="$SAVED_CXXFLAGS"])

AC_CHECK_PROGS([RCC], [rcc-qt5 rcc])

# Checks for libraries.
PKG_CHECK_MODULES([QT],
	[Qt5Gui Qt5Widgets Qt5Multimedia Qt5PrintSupport])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT32_T
AC_TYPE_INT32_T
AC_TYPE_UINT64_T
AC_TYPE_INT64_T
AC_TYPE_UINT16_T

# Checks for library functions.


AC_OUTPUT

expand_vble() {
    x="$1"
    while echo $x | fgrep '$' >/dev/null
    do
        x=`eval echo $x`
    done
    echo $x
}

tname="`expand_vble ${PACKAGE_TARNAME}`"
pgm="`expand_vble $bindir`/${tname}"
data="`expand_vble $datadir`/${tname}"
docdir="${prefix}/share/doc/${tname}"
distdir="${PACKAGE_TARNAME}-${PACKAGE_VERSION}"

echo "${PACKAGE_VERSION}" > VERSION
#sed "s:DISTDIR:${distdir}:g;s:PGM:${PACKAGE_TARNAME}:g"	\
#			INSTALL.in > INSTALL
sed "s:VERSION:${PACKAGE_VERSION}:g;s:DATADIR:${data}:g;s:BINARY:${pgm}:g" \
         ${tname}.desktop.in > linux/${tname}.desktop

echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'
 Binary: '${pgm}'
 Data Files: '${data}'
 Documentation: '${docdir}'

 Compiler: '${CXX} ${CXXFLAGS} ${CPPFLAGS}'

 Now type 'make @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries
     install            - install everything
     uninstall          - uninstall everything
     dist               - create distribution tarball
	                      (${tname}-${PACKAGE_VERSION}.tar.gz)

--------------------------------------------------"
