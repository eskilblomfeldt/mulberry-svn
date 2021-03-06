// Shared_Memory_SV.cpp,v 4.5 2005/05/31 01:11:28 ossama Exp

#include "ace/Shared_Memory_SV.h"

#if !defined (__ACE_INLINE__)
#include "ace/Shared_Memory_SV.inl"
#endif /* __ACE_INLINE__ */


ACE_RCSID (ace,
           Shared_Memory_SV,
           "Shared_Memory_SV.cpp,v 4.5 2005/05/31 01:11:28 ossama Exp")


ACE_ALLOC_HOOK_DEFINE(ACE_Shared_Memory_SV)

void
ACE_Shared_Memory_SV::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_Shared_Memory_SV::dump");
#endif /* ACE_HAS_DUMP */
}

ACE_Shared_Memory_SV::ACE_Shared_Memory_SV (key_t id,
					    int length,
					    int create,
					    int perms,
					    void *addr,
					    int flags)
  : shared_memory_ (id, length, create, perms, addr, flags)
{
  ACE_TRACE ("ACE_Shared_Memory_SV::ACE_Shared_Memory_SV");
}

// The overall size of the segment.

int
ACE_Shared_Memory_SV::get_segment_size (void) const
{
  ACE_TRACE ("ACE_Shared_Memory_SV::get_segment_size");
  // This cast is ok since the 'open' method for this class allows only
  // an 'int' size. Therefore, this case should not lose information.
  return static_cast<int> (this->shared_memory_.get_segment_size ());
}

// Removes the shared memory segment.

int
ACE_Shared_Memory_SV::remove (void)
{
  ACE_TRACE ("ACE_Shared_Memory_SV::remove");
  return shared_memory_.remove ();
}

// Closes (detaches) the shared memory segment.

int
ACE_Shared_Memory_SV::close (void)
{
  ACE_TRACE ("ACE_Shared_Memory_SV::close");
  return shared_memory_.detach ();
}

void *
ACE_Shared_Memory_SV::malloc (size_t)
{
  ACE_TRACE ("ACE_Shared_Memory_SV::malloc");
  return this->shared_memory_.get_segment_ptr ();
}

ACE_HANDLE
ACE_Shared_Memory_SV::get_id (void) const
{
  ACE_TRACE ("ACE_Shared_Memory_SV::get_id");
  return this->shared_memory_.get_id ();
}

int
ACE_Shared_Memory_SV::free (void *p)
{
  ACE_TRACE ("ACE_Shared_Memory_SV::free");
  return p != 0;
}
