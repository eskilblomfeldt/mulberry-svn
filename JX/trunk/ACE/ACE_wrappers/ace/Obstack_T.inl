// Obstack_T.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

template <class CHAR> ACE_INLINE size_t
ACE_Obstack_T<CHAR>::length () const
{
  return this->size_ / sizeof (CHAR);
}

template <class CHAR> ACE_INLINE size_t
ACE_Obstack_T<CHAR>::size () const
{
  return this->size_;
}

template <class CHAR> ACE_INLINE void
ACE_Obstack_T<CHAR>::grow_fast (CHAR c)
{
  * (reinterpret_cast<CHAR *> (this->curr_->cur_)) = c;
  this->curr_->cur_ += sizeof (CHAR);
}

template <class CHAR> ACE_INLINE CHAR *
ACE_Obstack_T<CHAR>::freeze (void)
{
  CHAR *retv = reinterpret_cast<CHAR *> (this->curr_->block_);
  * (reinterpret_cast<CHAR *> (this->curr_->cur_)) = 0;

  this->curr_->cur_ += sizeof (CHAR);
  this->curr_->block_ = this->curr_->cur_;
  return retv;
}
