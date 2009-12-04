// DEV_Connector.cpp
// DEV_Connector.cpp,v 4.12 2004/08/14 07:10:06 ossama Exp

#include "ace/DEV_Connector.h"

#include "ace/Handle_Ops.h"

#if !defined (__ACE_INLINE__)
#include "ace/DEV_Connector.inl"
#endif /* __ACE_INLINE__ */


ACE_RCSID (ace,
           DEV_Connector,
           "DEV_Connector.cpp,v 4.12 2004/08/14 07:10:06 ossama Exp")


ACE_ALLOC_HOOK_DEFINE(ACE_DEV_Connector)

void
ACE_DEV_Connector::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_DEV_Connector::dump");
#endif /* ACE_HAS_DUMP */
}

ACE_DEV_Connector::ACE_DEV_Connector (void)
{
  ACE_TRACE ("ACE_DEV_Connector::ACE_DEV_Connector");
}

int
ACE_DEV_Connector::connect (ACE_DEV_IO &new_io,
                            const ACE_DEV_Addr &remote_sap,
                            ACE_Time_Value *timeout,
                            const ACE_Addr &,
                            int,
                            int flags,
                            int perms)
{
  ACE_TRACE ("ACE_DEV_Connector::connect");

  ACE_HANDLE handle = ACE::handle_timed_open (timeout,
                                              remote_sap.get_path_name (),
                                              flags, perms);
  new_io.set_handle (handle);
  new_io.addr_ = remote_sap; // class copy.
  return handle == ACE_INVALID_HANDLE ? -1 : 0;
}
