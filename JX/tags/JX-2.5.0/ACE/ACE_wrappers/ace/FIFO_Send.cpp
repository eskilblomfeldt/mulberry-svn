// FIFO_Send.cpp
// FIFO_Send.cpp,v 4.13 2004/06/16 07:57:20 jwillemsen Exp

#include "ace/FIFO_Send.h"
#include "ace/Log_Msg.h"

#if !defined (__ACE_INLINE__)
#include "ace/FIFO_Send.inl"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ace, FIFO_Send, "FIFO_Send.cpp,v 4.13 2004/06/16 07:57:20 jwillemsen Exp")

ACE_ALLOC_HOOK_DEFINE(ACE_FIFO_Send)

void
ACE_FIFO_Send::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_FIFO_Send::dump");
  ACE_FIFO::dump ();
#endif /* ACE_HAS_DUMP */
}

ACE_FIFO_Send::ACE_FIFO_Send (void)
{
//  ACE_TRACE ("ACE_FIFO_Send::ACE_FIFO_Send");
}

int
ACE_FIFO_Send::open (const ACE_TCHAR *rendezvous_name,
                     int flags,
                     int perms,
                     LPSECURITY_ATTRIBUTES sa)
{
  ACE_TRACE ("ACE_FIFO_Send::open");
  return ACE_FIFO::open (rendezvous_name,
                         flags | O_WRONLY,
                         perms,
                         sa);
}

ACE_FIFO_Send::ACE_FIFO_Send (const ACE_TCHAR *fifo_name,
                              int flags,
                              int perms,
                              LPSECURITY_ATTRIBUTES sa)
{
  ACE_TRACE ("ACE_FIFO_Send::ACE_FIFO_Send");
  if (this->ACE_FIFO_Send::open (fifo_name,
                                 flags,
                                 perms,
                                 sa) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%p\n"),
                ACE_LIB_TEXT ("ACE_FIFO_Send::ACE_FIFO_Send")));
}
