/* -*- C++ -*- */
// SPIPE.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

ACE_INLINE int
ACE_SPIPE::disable (int signum) const
{
#if defined (ACE_WIN32)
  ACE_UNUSED_ARG (signum) ;
  return 0 ;
#else  /* ACE_WIN32 */
  return ACE_IPC_SAP::disable (signum) ;
#endif /* ACE_WIN32 */
}

