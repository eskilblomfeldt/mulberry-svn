/* -*- C++ -*- */
// Activation_Queue.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// Activation_Queue.i

ACE_INLINE size_t
ACE_Activation_Queue::method_count (void) const 
{
  return queue_->message_count ();
}

ACE_INLINE int 
ACE_Activation_Queue::is_full (void) const 
{
  return queue_->is_full ();
} 

ACE_INLINE int 
ACE_Activation_Queue::is_empty (void) const 
{
  return queue_->is_empty ();
}

ACE_INLINE ACE_Message_Queue<ACE_SYNCH> *
ACE_Activation_Queue::queue (void) const
{
  return queue_;
}

ACE_INLINE void 
ACE_Activation_Queue::queue (ACE_Message_Queue<ACE_SYNCH> *q)
{
  queue_ = q;
}
