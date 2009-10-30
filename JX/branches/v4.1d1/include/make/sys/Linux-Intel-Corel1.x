# configuration for compiling with g++

#
# Adjustable parameters
#

# This tells us where to find the X libraries.

J_X11_LIB_DIR := -L/usr/X11R6/lib

# Uncomment this and change it if your X11 headers are not in /usr/include/X11.

# J_X11_INCLUDE_DIR := -I/usr/include

# Uncomment this if you want to compile libXpm into libjx.

# JX_INCLUDE_LIBXPM := yes

# This sets the optimization level.

J_OPTIMIZE_LEVEL := 0

# Comment this out if your system is not ELF.

J_BUILD_SHARED_LIB := -fPIC

# This flag tells the linker to build a shared library.

J_SHARED_LIB_LINK_OPTION := -shared

# Uncomment this if libACE.so doesn't work

# J_FORCE_ACE_STATIC := yes

# Uncomment this if your system requires separate .o files for
# static and shared libraries.  The actual value is not used.

# J_NEED_SEPARATE_O_FILES := yes

# Uncomment this if you want debugging versions of the libraries.

J_COMPILE_DEBUG := -g

# This command is used to strip debugging info but not the symbol table.

J_STRIP_DEBUG := strip -g

# Comment this out if ACE or other code gives you warnings that
# you can't avoid.

J_DIE_ON_WARNING := -Werror

# Comment this out if new[] and delete[] are not overridable.

J_ARRAY_NEW_OVERRIDABLE := -D_J_ARRAY_NEW_OVERRIDABLE

# Uncomment this if your version of GNU make does not automatically
# generate the -o parameter.

# J_MAKE_NEEDS_OUTPUT_ARG = -o $@

# Comment this out if your version of "ar" does not accept the -s option.

J_RANLIB_VIA_AR_S := -D_J_RANLIB_VIA_AR_S

# Uncomment this if your version of "man" requires -s to specify the section.

# J_MAN_SECTION_VIA_DASH_S := -D_J_MAN_SECTION_VIA_DASH_S

# Uncomment this if you need to use readdir_r() instead of readdir().

# J_USE_READDIR_R := -DJ_USE_READDIR_R

# Uncomment this if your system doesn't have memmove(). (used by jlib/regex)

# J_REGEX_USE_BCOPY := -DUSEBCOPY

# Add to this until everything compiles.
#
# You must include "-DSIZEOF_LONG=N -DSIZEOF_INT=M"
# On most systems, N and M are both 4.  Check /usr/include/limits.h

J_RAW_SYSTEM_STUFF := \
  -DSIZEOF_LONG=4 -DSIZEOF_INT=4 \
  -D_LOOSE_KERNEL_NAMES \
  -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED \
  -D_BSD_SOURCE -D_GNU_SOURCE

#
# Nothing below this line should ever need to be changed.
#

# makemake variables

LINKER := g++

# make variables

CC       := gcc
CXX      := g++
CPPFLAGS  = ${DEPENDFLAGS}
CFLAGS    = ${J_MAKE_NEEDS_OUTPUT_ARG}
CXXFLAGS  = ${J_MAKE_NEEDS_OUTPUT_ARG}

# libraries to link

J_GCC_LIBS  := -lstdc++ -lm -lc -ldl
J_ACE_LIBS  := -L${JX_ROOT}/lib -lACE-${ACE_LIB_VERSION}
J_X11_LIBS  := -L${JX_ROOT}/lib ${J_X11_LIB_DIR} -lXext -lXpm -lX11
J_MESA_LIBS := -L${JX_ROOT}/lib -lGLU -lGL

# modify settings based on OS, compiler, etc.

include ${JX_ROOT}/include/make/jx_config_common
