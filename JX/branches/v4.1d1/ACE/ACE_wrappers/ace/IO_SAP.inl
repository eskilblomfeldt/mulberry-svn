/* -*- C++ -*- */
// IO_SAP.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// IO_SAP.i

#include "ace/OS_NS_stropts.h"

ACE_INLINE
ACE_IO_SAP::~ACE_IO_SAP (void)
{
  ACE_TRACE ("ACE_IO_SAP::~ACE_IO_SAP");
}

// Used to return the underlying handle_.

ACE_INLINE ACE_HANDLE
ACE_IO_SAP::get_handle (void) const
{
  ACE_TRACE ("ACE_IO_SAP::get_handle");
  return this->handle_;
}

// Used to set the underlying handle_.

ACE_INLINE void
ACE_IO_SAP::set_handle (ACE_HANDLE handle)
{
  ACE_TRACE ("ACE_IO_SAP::set_handle");
  this->handle_ = handle;
}

// Provides access to the ACE_OS::ioctl system call.

ACE_INLINE int
ACE_IO_SAP::control (int cmd, void *arg) const
{
  ACE_TRACE ("ACE_IO_SAP::control");
  return ACE_OS::ioctl (this->handle_, cmd, arg);
}


