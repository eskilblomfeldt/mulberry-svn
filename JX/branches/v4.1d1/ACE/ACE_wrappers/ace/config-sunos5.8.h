/* -*- C++ -*- */
// config-sunos5.8.h,v 4.5 2005/02/16 13:04:43 elliott_c Exp

// The following configuration file is designed to work for SunOS 5.8
// (Solaris 8) platforms using the SunC++ 4.x, 5.x, 6.x, or g++ compilers.

#ifndef ACE_CONFIG_H

// ACE_CONFIG_H is defined by one of the following #included headers.

// #include the SunOS 5.7 config, then add any SunOS 5.8 updates below.
#include "ace/config-sunos5.7.h"

#undef ACE_WCHAR_IN_STD_NAMESPACE

// This may be true for versions prior to Solaris 8 as well, but I don't
// have any to try it on.
#if !defined (ACE_HAS_TIMEZONE)
#  define ACE_HAS_TIMEZONE
#endif

// The range of thread priorities for 5.8 differs from 5.7 in the
// minimum priority for the SCHED_OTHER policy (i.e.,
// ACE_THR_PRI_OTHER_MIN)
# define ACE_THR_PRI_OTHER_MIN (long) -20

# if defined (_POSIX_PTHREAD_SEMANTICS)
#  ifdef ACE_LACKS_RWLOCK_T
#   undef ACE_LACKS_RWLOCK_T
#  endif /* ACE_LACKS_RWLOCK_T */
# endif /* _POSIX_PTHREAD_SEMANTICS */

#endif /* ACE_CONFIG_H */
