/*
    Copyright (c) 2007 Cyrus Daboo. All rights reserved.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef __STL_SVECTOR_SPECIAL_C
#define __STL_SVECTOR_SPECIAL_C

# if defined ( __STL_NESTED_TYPE_PARAM_BUG )
#  define __pointer__             const void**
#  define __const_pointer__       const const void**
#  define __size_type__           size_t
#  define __difference_type__     ptrdiff_t
# else
#  define __pointer__         pointer
#  define __const_pointer__   const_pointer
#  define __size_type__       size_type
#  define __difference_type__ difference_type
# endif

#  define __iterator__       __pointer__
#  define __const_iterator__ __const_pointer__  

#  undef  svector
#  define svector __WORKAROUND_DBG_RENAME(svector)

__STL_BEGIN_NAMESPACE

template <class _Alloc>
void 
__svector__<const void*, _Alloc>::reserve(size_type __n) {
  if (capacity() < __n) {
    const size_type __old_size = size();
    pointer __tmp;
    if (this->_M_start) {
      __tmp = _M_allocate_and_copy(__n, this->_M_start, this->_M_finish);
      _M_clear();
    } else {
      __tmp = this->_M_end_of_storage.allocate(__n);
    }
    _M_set(__tmp, __tmp + __old_size, __tmp + __n);
  }
}

template <class _Alloc>
void 
__svector__<const void*, _Alloc>::_M_fill_insert(
				    iterator __position, 
				    size_type __n, const void*& __x) {
  if (__n != 0) {
    if (size_type(this->_M_end_of_storage._M_data - this->_M_finish) >= __n) {
      const void* __x_copy = __x;
      const size_type __elems_after = this->_M_finish - __position;
      pointer __old_finish = this->_M_finish;
      if (__elems_after > __n) {
        __uninitialized_copy(this->_M_finish - __n, this->_M_finish, this->_M_finish, _IsPODType());
        this->_M_finish += __n;
        __copy_backward_ptrs(__position, __old_finish - __n, __old_finish, _TrivialAss());
        _STLP_STD::fill(__position, __position + __n, __x_copy);
      }
      else {
        uninitialized_fill_n(this->_M_finish, __n - __elems_after, __x_copy);
        this->_M_finish += __n - __elems_after;
        __uninitialized_copy(__position, __old_finish, this->_M_finish, _IsPODType());
        this->_M_finish += __elems_after;
        _STLP_STD::fill(__position, __old_finish, __x_copy);
      }
    }
    else 
      _M_insert_overflow(__position, __x, _IsPODType(), __n);
  }
}

template <class _Alloc>
__svector__<const void*,_Alloc>& 
__svector__<const void*,_Alloc>::operator=(const __svector__<const void*, _Alloc>& __x)
{
  if (&__x != this) {
    const size_type __xlen = __x.size();
    if (__xlen > capacity()) {
      pointer __tmp = _M_allocate_and_copy(__xlen, (const_pointer)__x._M_start+0, (const_pointer)__x._M_finish+0);
      _M_clear();
      this->_M_start = __tmp;
      this->_M_end_of_storage._M_data = this->_M_start + __xlen;
    }
    else if (size() >= __xlen) {
      pointer __i = __copy_ptrs((const_pointer)__x._M_start+0, (const_pointer)__x._M_finish+0, (pointer)this->_M_start, _TrivialAss());
      _Destroy(__i, this->_M_finish);
    }
    else {
      __copy_ptrs((const_pointer)__x._M_start, (const_pointer)__x._M_start + size(), (pointer)this->_M_start, _TrivialAss());
      __uninitialized_copy((const_pointer)__x._M_start + size(), (const_pointer)__x._M_finish+0, this->_M_finish, _IsPODType());
    }
    this->_M_finish = this->_M_start + __xlen;
	this-> keep_small = __x.keep_small;
  }
  return *this;
}

template <class _Alloc>
void __svector__<const void*, _Alloc>::_M_fill_assign(size_t __n, const void*& __val) {
  if (__n > capacity()) {
    __svector__<const void*, _Alloc> __tmp(__n, __val, get_allocator());
    __tmp.swap(*this);
  }
  else if (__n > size()) {
    fill(begin(), end(), __val);
    this->_M_finish = _STLP_STD::uninitialized_fill_n(this->_M_finish, __n - size(), __val);
  }
  else
    erase(_STLP_STD::fill_n(begin(), __n, __val), end());
}

__STL_END_NAMESPACE

# undef __pointer__
# undef __const_pointer__
# undef __size_type__
# undef __difference_type__
# undef __iterator__
# undef __const_iterator__
# undef svector

#endif /*  __STL_SVECTOR_C */

      // Local Variables:
	// mode:C++
	// End:
