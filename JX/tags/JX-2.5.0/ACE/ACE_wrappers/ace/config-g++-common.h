// -*- C++ -*-
//
// config-g++-common.h,v 4.67 2005/05/19 07:41:57 ossama Exp

// This configuration file is designed to be included by another,
// specific configuration file.  It provides config information common
// to all g++ platforms, including egcs.

#ifndef ACE_GNUG_COMMON_H
#define ACE_GNUG_COMMON_H
#include /**/ "ace/pre.h"

#define ACE_HAS_CPLUSPLUS_HEADERS
#define ACE_HAS_STDCPP_STL_INCLUDES
#define ACE_HAS_TEMPLATE_TYPEDEFS
#define ACE_HAS_TYPENAME_KEYWORD
#define ACE_HAS_STANDARD_CPP_LIBRARY 1
#define ACE_HAS_TEMPLATE_SPECIALIZATION
#define ACE_HAS_USING_KEYWORD
#define ACE_HAS_WORKING_EXPLICIT_TEMPLATE_DESTRUCTOR
#define ACE_USES_STD_NAMESPACE_FOR_STDCPP_LIB 1
#define ACE_TEMPLATES_REQUIRE_SOURCE

#if ( __GNUC__ == 2 && __GNUC_MINOR__ < 97 )
  // gcc 2.97 and lower use old iostreams
# define ACE_USES_OLD_IOSTREAMS
#endif /* __GNUC__ >= 2.97 */

#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
# define ACE_EXPLICIT_TEMPLATE_DESTRUCTOR_TAKES_ARGS
#endif /* __GNUC__ >= 3.4 */

#if (__GNUC__ < 3)
# define ACE_LACKS_MEMBER_TEMPLATES
#endif /* __GNUC__ < 3 */

// __EXCEPTIONS is defined with -fexceptions, the egcs default.  It
// is not defined with -fno-exceptions, the ACE default for g++.
// ACE_HAS_EXCEPTIONS is defined in
// include/makeinclude/wrapper_macros.GNU, so this really isn't
// necessary.  Just in case . . .
#if defined (__EXCEPTIONS) && !defined (ACE_HAS_EXCEPTIONS)
#  define ACE_HAS_EXCEPTIONS
#endif /* __EXCEPTIONS && ! ACE_HAS_EXCEPTIONS */

#if defined (ACE_HAS_EXCEPTIONS)
#  define ACE_NEW_THROWS_EXCEPTIONS
#endif /* ACE_HAS_EXCEPTIONS */

#if (defined (i386) || defined (__i386__)) && !defined (ACE_SIZEOF_LONG_DOUBLE)
# define ACE_SIZEOF_LONG_DOUBLE 12
#endif /* i386 */

#if !defined (__MINGW32__) && (defined (i386) || defined (__i386__))
  // If running an Intel, assume that it's a Pentium so that
  // ACE_OS::gethrtime () can use the RDTSC instruction.  If running a
  // 486 or lower, be sure to comment this out.  (If not running an
  // Intel CPU, this #define will not be seen because of the i386
  // protection, so it can be ignored.)
# define ACE_HAS_PENTIUM
#endif /* i386 */

// GNU g++ >= 4.x implements "#pragma once".
#if (__GNUC__ < 4) && !defined (ACE_LACKS_PRAGMA_ONCE)
// We define it with a -D with make depend.
# define ACE_LACKS_PRAGMA_ONCE
#endif /* ! ACE_LACKS_PRAGMA_ONCE */

// Take advantage of G++ (>= 4.x) visibility attributes to generate
// improved shared library binaries.
#if (__GNUC__ > 3)
# define ACE_HAS_CUSTOM_EXPORT_MACROS
# define ACE_Proper_Export_Flag __attribute__ ((visibility("default")))
# define ACE_Proper_Import_Flag
# define ACE_EXPORT_SINGLETON_DECLARATION(T)
# define ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#define ACE_IMPORT_SINGLETON_DECLARATION(T)
#define ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif  /* __GNU__ > 3 */

#if defined (ACE_HAS_GNU_REPO)
  // -frepo causes unresolved symbols of basic_string left- and
  // right-shift operators with ACE_HAS_STRING_CLASS.
# if defined (ACE_HAS_STRING_CLASS)
#   undef ACE_HAS_STRING_CLASS
# endif /* ACE_HAS_STRING_CLASS */
#endif /* ! ACE_HAS_GNU_REPO */

#include /**/ "ace/post.h"
#endif /* ACE_GNUG_COMMON_H */
