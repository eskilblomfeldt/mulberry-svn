/* -*- C++ -*- */
// Flag_Manip.inl,v 4.2 2004/08/14 07:03:12 ossama Exp

// Return flags currently associated with handle.

#include "ace/OS_NS_fcntl.h"

ACE_INLINE int
ACE::get_flags (ACE_HANDLE handle)
{
  ACE_TRACE ("ACE::get_flags");

#if defined (ACE_LACKS_FCNTL)
  // ACE_OS::fcntl is not supported, e.g., on VxWorks.  It
  // would be better to store ACE's notion of the flags
  // associated with the handle, but this works for now.
  ACE_UNUSED_ARG (handle);
  return 0;
#else
  return ACE_OS::fcntl (handle, F_GETFL, 0);
#endif /* ACE_LACKS_FCNTL */
}
