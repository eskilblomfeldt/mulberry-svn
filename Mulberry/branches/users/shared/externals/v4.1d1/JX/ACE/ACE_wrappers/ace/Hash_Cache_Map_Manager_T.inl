/* -*- C++ -*- */
// Hash_Cache_Map_Manager_T.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

#define ACE_T1 class KEY, class VALUE, class HASH_KEY, class COMPARE_KEYS, class CACHING_STRATEGY, class ATTRIBUTES
#define ACE_T2 KEY, VALUE, HASH_KEY, COMPARE_KEYS, CACHING_STRATEGY, ATTRIBUTES

template <ACE_T1> ACE_INLINE int
ACE_Hash_Cache_Map_Manager<ACE_T2>::bind (const KEY &key,
                                       const VALUE &value)
{
  return ACE_HCMM_BASE::bind (key,
                              value);
}

template <ACE_T1> ACE_INLINE int
ACE_Hash_Cache_Map_Manager<ACE_T2>::rebind (const KEY &key,
                                         const VALUE &value)
{
  return ACE_HCMM_BASE::rebind (key,
                                value);
}

template <ACE_T1> ACE_INLINE int
ACE_Hash_Cache_Map_Manager<ACE_T2>::rebind (const KEY &key,
                                         const VALUE &value,
                                         VALUE &old_value)
{
  return ACE_HCMM_BASE::rebind (key,
                                value,
                                old_value);
}

template <ACE_T1> ACE_INLINE int
ACE_Hash_Cache_Map_Manager<ACE_T2>::rebind (const KEY &key,
                                         const VALUE &value,
                                         KEY &old_key,
                                         VALUE &old_value)
{
  return ACE_HCMM_BASE::rebind (key,
                                value,
                                old_key,
                                old_value);
}

template <ACE_T1> ACE_INLINE int
ACE_Hash_Cache_Map_Manager<ACE_T2>::trybind (const KEY &key,
                                          VALUE &value)
{
  return ACE_HCMM_BASE::trybind (key,
                                 value);
}

template <ACE_T1> ACE_INLINE int
ACE_Hash_Cache_Map_Manager<ACE_T2>::unbind (const KEY &key)
{
  return ACE_HCMM_BASE::unbind (key);
}

template <ACE_T1> ACE_INLINE int
ACE_Hash_Cache_Map_Manager<ACE_T2>::unbind (const KEY &key,
                                         VALUE &value)
{
  return ACE_HCMM_BASE::unbind (key,
                                value);
}

#undef ACE_T1
#undef ACE_T2
