/* -*- C++ -*- */
// SOCK.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// SOCK.i

#include "ace/OS_NS_sys_socket.h"

ACE_INLINE
ACE_SOCK::~ACE_SOCK (void)
{
  // ACE_TRACE ("ACE_SOCK::~ACE_SOCK");
}

ACE_INLINE int
ACE_SOCK::set_option (int level,
		      int option,
		      void *optval,
		      int optlen) const
{
  ACE_TRACE ("ACE_SOCK::set_option");
  return ACE_OS::setsockopt (this->get_handle (), level,
			     option, (char *) optval, optlen);
}

// Provides access to the ACE_OS::getsockopt system call.

ACE_INLINE int
ACE_SOCK::get_option (int level,
		      int option,
		      void *optval,
		      int *optlen) const
{
  ACE_TRACE ("ACE_SOCK::get_option");
  return ACE_OS::getsockopt (this->get_handle (), level,
			     option, (char *) optval, optlen);
}
