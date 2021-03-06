/* -*- C++ -*- */
// Shared_Memory_MM.inl,v 4.2 2005/05/31 01:11:28 ossama Exp

// Shared_Memory_MM.i

// Return the name of file that is mapped (if any).

ACE_INLINE const ACE_TCHAR *
ACE_Shared_Memory_MM::filename (void) const
{
  return this->shared_memory_.filename ();
}

ACE_INLINE int
ACE_Shared_Memory_MM::open (ACE_HANDLE handle,
                            int length,
                            int prot,
                            int share,
                            char *addr,
                            off_t pos)
{
  ACE_TRACE ("ACE_Shared_Memory_MM::open");
  return shared_memory_.map (handle, length, prot, share, addr, pos);
}

ACE_INLINE int
ACE_Shared_Memory_MM::open (const ACE_TCHAR *file_name,
                            int len,
                            int flags,
                            int mode,
                            int prot,
                            int share,
                            char *addr,
                            off_t pos)
{
  ACE_TRACE ("ACE_Shared_Memory_MM::open");
  return shared_memory_.map (file_name, len, flags, mode,
                             prot, share, addr, pos);
}
