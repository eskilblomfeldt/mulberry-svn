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

__STL_BEGIN_NAMESPACE 

template <class _Alloc >
class svector<const void*, _Alloc> : public _SVector_base<const void*, _Alloc> 
{
private:
  typedef _SVector_base<const void*, _Alloc> _Base;
public:
  typedef const void* value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;

public:
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef random_access_iterator_tag _Iterator_category;

  _STLP_DECLARE_RANDOM_ACCESS_REVERSE_ITERATORS;
  _STLP_FORCE_ALLOCATORS(const void*, _Alloc)
  typedef typename _Base::allocator_type allocator_type;

  allocator_type get_allocator() const {
    return _STLP_CONVERT_ALLOCATOR((const allocator_type&)this->_M_end_of_storage, value_type);
  }
protected:
  typedef typename  __type_traits<const void*>::has_trivial_assignment_operator _TrivialAss;
  typedef typename  __type_traits<const void*>::has_trivial_assignment_operator _IsPODType;
  bool keep_small;

  // handles insertions on overflow
  void _M_insert_overflow(pointer __position, const void*& __x, const __false_type&, 
			  size_type __fill_len, bool __atend = false) {
    const size_type __old_size = size();
    const size_type __len = __old_size + (max)(__old_size, __fill_len);
    
    pointer __new_start = this->_M_end_of_storage.allocate(__len);
    pointer __new_finish = __new_start;
    _STLP_TRY {
      __new_finish = __uninitialized_copy(this->_M_start, __position, __new_start, __false_type());
      // handle insertion
      if (__fill_len == 1) {
        _Construct(__new_finish, __x);
        ++__new_finish;
      } else
        __new_finish = __uninitialized_fill_n(__new_finish, __fill_len, __x, __false_type());
      if (!__atend)
        // copy remainder
        __new_finish = __uninitialized_copy(__position, this->_M_finish, __new_finish, __false_type());
    }
    _STLP_UNWIND((_Destroy(__new_start,__new_finish), 
                  this->_M_end_of_storage.deallocate(__new_start,__len)));
    _M_clear();
    _M_set(__new_start, __new_finish, __new_start + __len);
  }

  void _M_insert_overflow(pointer __position, const void*& __x, const __true_type&, 
			  size_type __fill_len, bool __atend = false) {
    const size_type __old_size = size();
    const size_type __len = __old_size + (max)(__old_size, __fill_len);
    
    pointer __new_start = this->_M_end_of_storage.allocate(__len);
    pointer __new_finish = (pointer)__copy_trivial(this->_M_start, __position, __new_start);
      // handle insertion
    __new_finish = fill_n(__new_finish, __fill_len, __x);
    if (!__atend)
      // copy remainder
      __new_finish = (pointer)__copy_trivial(__position, this->_M_finish, __new_finish);
    _M_clear();
    _M_set(__new_start, __new_finish, __new_start + __len);
  }
 
  void _M_range_check(size_type __n) const {
    if (__n >= size_type(this->_M_finish-this->_M_start))
      __stl_throw_out_of_range("svector");
  }

public:
	inline
	void set_keep_small(bool ksmall) {keep_small = ksmall;}
	inline
	bool get_keep_small() const {return keep_small;}

  iterator begin()             { return this->_M_start; }
  const_iterator begin() const { return this->_M_start; }
  iterator end()               { return this->_M_finish; }
  const_iterator end() const   { return this->_M_finish; }

  reverse_iterator rbegin()              { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const  { return const_reverse_iterator(end()); }
  reverse_iterator rend()                { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const    { return const_reverse_iterator(begin()); }

  size_type size() const        { return size_type(this->_M_finish - this->_M_start); }
  size_type max_size() const    { return size_type(-1) / sizeof(const void*); }
  size_type capacity() const    { return size_type(this->_M_end_of_storage._M_data - this->_M_start); }
  bool empty() const            { return this->_M_start == this->_M_finish; }

  reference operator[](size_type __n) { return *(begin() + __n); }
  const_reference operator[](size_type __n) const { return *(begin() + __n); }

  reference front()             { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back()              { return *(end() - 1); }
  const_reference back() const  { return *(end() - 1); }

  reference at(size_type __n) { _M_range_check(__n); return (*this)[__n]; }
  const_reference at(size_type __n) const { _M_range_check(__n); return (*this)[__n]; }

  explicit svector(const allocator_type& __a = allocator_type()) : 
    _Base(__a), keep_small(true) {}

  svector(size_type __n, const void*& __val,
         const allocator_type& __a = allocator_type()) 
    : _Base(__n, __a), keep_small(true) { 
    this->_M_finish = uninitialized_fill_n(this->_M_start, __n, __val); 
  }

  explicit svector(size_type __n)
    : _Base(__n, allocator_type() ), keep_small(true) {
    this->_M_finish = uninitialized_fill_n(this->_M_start, __n, 0); 
  }

  svector(const svector<const void*, _Alloc>& __x) 
    : _Base(__x.size(), __x.get_allocator()), keep_small(__x.keep_small) { 
    this->_M_finish = __uninitialized_copy((const_pointer)__x._M_start, 
                                           (const_pointer)__x._M_finish, this->_M_start, _IsPODType());
  }
  
#if defined (_STLP_MEMBER_TEMPLATES)

  template <class _Integer>
  void _M_initialize_aux(_Integer __n, _Integer __val, const __true_type&) {
    this->_M_start = this->_M_end_of_storage.allocate(__n);
    this->_M_end_of_storage._M_data = this->_M_start + __n; 
    this->_M_finish = uninitialized_fill_n(this->_M_start, __n, __val);
  }

  template <class _InputIterator>
  void _M_initialize_aux(_InputIterator __first, _InputIterator __last,
                         const __false_type&) {
    _M_range_initialize(__first, __last, _STLP_ITERATOR_CATEGORY(__first, _InputIterator));
  }

  // Check whether it's an integral type.  If so, it's not an iterator.
 # ifdef _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
  template <class _InputIterator>
  svector(_InputIterator __first, _InputIterator __last) :
    _Base(allocator_type()), keep_small(true) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_initialize_aux(__first, __last, _Integral());
  }
 # endif
  template <class _InputIterator>
  svector(_InputIterator __first, _InputIterator __last,
         const allocator_type& __a _STLP_ALLOCATOR_TYPE_DFL ) :
    _Base(__a), keep_small(true) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_initialize_aux(__first, __last, _Integral());
  }

#else
  svector(const void** __first, const void** __last,
         const allocator_type& __a = allocator_type())
    : _Base(__last - __first, __a), keep_small(true) { 
      this->_M_finish = __uninitialized_copy(__first, __last, this->_M_start, _IsPODType()); 
  }
#endif /* _STLP_MEMBER_TEMPLATES */

  ~svector() { _Destroy(this->_M_start, this->_M_finish); }

  svector<const void*, _Alloc>& operator=(const svector<const void*, _Alloc>& __x);

  void reserve(size_type __n);

  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.

  void assign(size_type __n, const void*& __val) { _M_fill_assign(__n, __val); }
  void _M_fill_assign(size_type __n, const void*& __val);
  
#ifdef _STLP_MEMBER_TEMPLATES
  template <class _ForwardIter>
  void _M_assign_aux(_ForwardIter __first, _ForwardIter __last, const forward_iterator_tag &)
#else
  void assign(const_iterator __first, const_iterator __last)
#endif
  {
    size_type __len = distance(__first, __last);
    
    if (__len > capacity()) {
      iterator __tmp = _M_allocate_and_copy(__len, __first, __last);
    _M_clear();
    _M_set(__tmp, __tmp + __len, __tmp + __len);
    }
    else if (size() >= __len) {
      iterator __new_finish = copy(__first, __last, this->_M_start);
      _Destroy(__new_finish, this->_M_finish);
      this->_M_finish = __new_finish;
    }
    else {
# if defined ( _STLP_MEMBER_TEMPLATES )
          _ForwardIter __mid = __first;
          advance(__mid, size());
# else
          const_iterator __mid = __first + size() ;
# endif
    copy(__first, __mid, this->_M_start);
    this->_M_finish = __uninitialized_copy(__mid, __last, this->_M_finish, _IsPODType());
    }
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIter>
  void _M_assign_aux(_InputIter __first, _InputIter __last,
		     const input_iterator_tag &) {
    iterator __cur = begin();
    for ( ; __first != __last && __cur != end(); ++__cur, ++__first)
      *__cur = *__first;
    if (__first == __last)
      erase(__cur, end());
    else
      insert(end(), __first, __last);
  }
  
  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, const __true_type&)
    { assign((size_type) __n, (const void*) __val); }

  template <class _InputIter>
  void _M_assign_dispatch(_InputIter __first, _InputIter __last, const __false_type&)
    { _M_assign_aux(__first, __last, _STLP_ITERATOR_CATEGORY(__first, _InputIter)); }

  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }
#endif /* _STLP_MEMBER_TEMPLATES */

  void push_back(const void*& __x) {
    if (this->_M_finish != this->_M_end_of_storage._M_data) {
      _Construct(this->_M_finish, __x);
      ++this->_M_finish;
    }
    else
      _M_insert_overflow(this->_M_finish, __x, _IsPODType(), 1UL, true);
  }

  void swap(svector<const void*, _Alloc>& __x) {
    _STLP_STD::swap(this->_M_start, __x._M_start);
    _STLP_STD::swap(this->_M_finish, __x._M_finish);
    _STLP_STD::swap(this->_M_end_of_storage, __x._M_end_of_storage);
  }

  iterator insert(iterator __position, const void*& __x) {
    size_type __n = __position - begin();
    if (this->_M_finish != this->_M_end_of_storage._M_data) {
      if (__position == end()) {
        _Construct(this->_M_finish, __x);
        ++this->_M_finish;
      } else {
        _Construct(this->_M_finish, *(this->_M_finish - 1));
        ++this->_M_finish;
        const void* __x_copy = __x;
        __copy_backward_ptrs(__position, this->_M_finish - 2, this->_M_finish - 1, _TrivialAss());
        *__position = __x_copy;
      }
    }
    else
      _M_insert_overflow(__position, __x, _IsPODType(), 1UL);
    return begin() + __n;
  }

# ifndef _STLP_NO_ANACHRONISMS
  void push_back() { push_back(0); }
  iterator insert(iterator __position) { return insert(__position, 0); }
# endif

  void _M_fill_insert (iterator __pos, size_type __n, const void*& __x);

#if defined ( _STLP_MEMBER_TEMPLATES)

  template <class _Integer>
  void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __val,
                          const __true_type&) {
    _M_fill_insert(__pos, (size_type) __n, (const void*) __val);
  }

  template <class _InputIterator>
  void _M_insert_dispatch(iterator __pos,
                          _InputIterator __first, _InputIterator __last,
                          const __false_type&) {
    _M_range_insert(__pos, __first, __last, _STLP_ITERATOR_CATEGORY(__first, _InputIterator));
  }

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_insert_dispatch(__pos, __first, __last, _Integral());
  }

  template <class _InputIterator>
  void _M_range_insert(iterator __pos, 
		       _InputIterator __first, 
		       _InputIterator __last,
		       const input_iterator_tag &) {
    for ( ; __first != __last; ++__first) {
      __pos = insert(__pos, *__first);
      ++__pos;
    }
  }

  template <class _ForwardIterator>
  void _M_range_insert(iterator __position,
                       _ForwardIterator __first,
                       _ForwardIterator __last,
                       const forward_iterator_tag &) 
#else /* _STLP_MEMBER_TEMPLATES */
  void insert(iterator __position,
              const_iterator __first, const_iterator __last)
#endif /* _STLP_MEMBER_TEMPLATES */

  {
    if (__first != __last) {
      size_type __n = distance(__first, __last);

      if (size_type(this->_M_end_of_storage._M_data - this->_M_finish) >= __n) {
        const size_type __elems_after = this->_M_finish - __position;
        pointer __old_finish = this->_M_finish;
        if (__elems_after > __n) {
          __uninitialized_copy(this->_M_finish - __n, this->_M_finish, this->_M_finish, _IsPODType());
          this->_M_finish += __n;
          __copy_backward_ptrs(__position, __old_finish - __n, __old_finish, _TrivialAss());
          copy(__first, __last, __position);
        }
        else {
# if defined ( _STLP_MEMBER_TEMPLATES )
          _ForwardIterator __mid = __first;
          advance(__mid, __elems_after);
# else
          const_pointer __mid = __first + __elems_after;
# endif
          __uninitialized_copy(__mid, __last, this->_M_finish, _IsPODType());
          this->_M_finish += __n - __elems_after;
          __uninitialized_copy(__position, __old_finish, this->_M_finish, _IsPODType());
          this->_M_finish += __elems_after;
          copy(__first, __mid, __position);
        } /* elems_after */
      }
      else {
        const size_type __old_size = size();
		size_type __len = 0;
		if (keep_small)
			__len = __old_size + __n;
		else
			__len = __old_size + max(__old_size, __n);
        pointer __new_start = this->_M_end_of_storage.allocate(__len);
        pointer __new_finish = __new_start;
        _STLP_TRY {
          __new_finish = __uninitialized_copy(this->_M_start, __position, __new_start, _IsPODType());
          __new_finish = __uninitialized_copy(__first, __last, __new_finish, _IsPODType());
          __new_finish = __uninitialized_copy(__position, this->_M_finish, __new_finish, _IsPODType());
        }
        _STLP_UNWIND((_Destroy(__new_start,__new_finish), 
                      this->_M_end_of_storage.deallocate(__new_start,__len)));
        _M_clear();
        _M_set(__new_start, __new_finish, __new_start + __len);
      }
    }
  }
  void insert (iterator __pos, size_type __n, const void*& __x)
  { _M_fill_insert(__pos, __n, __x); }
  
  void pop_back() {
    --this->_M_finish;
    _Destroy(this->_M_finish);
  }
  iterator erase(iterator __position) {
    if (__position + 1 != end())
      __copy_ptrs(__position + 1, this->_M_finish, __position, _TrivialAss());
    --this->_M_finish;
    _Destroy(this->_M_finish);
    return __position;
  }
  iterator erase(iterator __first, iterator __last) {
    pointer __i = __copy_ptrs(__last, this->_M_finish, __first, _TrivialAss());
    _Destroy(__i, this->_M_finish);
    this->_M_finish = __i;
    return __first;
  }

  void resize(size_type __new_size, const void*& __x) {
    if (__new_size < size()) 
      erase(begin() + __new_size, end());
    else
      insert(end(), __new_size - size(), __x);
  }
  void resize(size_type __new_size) { resize(__new_size, 0); }
  void clear() { 
    erase(begin(), end());
  }

protected:

  void _M_clear() {
    //    if (this->_M_start) {
    _Destroy(this->_M_start, this->_M_finish);
    this->_M_end_of_storage.deallocate(this->_M_start, this->_M_end_of_storage._M_data - this->_M_start);
    //    }
  }

  void _M_set(pointer __s, pointer __f, pointer __e) {
    this->_M_start = __s;
    this->_M_finish = __f;
    this->_M_end_of_storage._M_data = __e;
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template <class _ForwardIterator>
  pointer _M_allocate_and_copy(size_type __n, _ForwardIterator __first, 
				_ForwardIterator __last)
#else /* _STLP_MEMBER_TEMPLATES */
  pointer _M_allocate_and_copy(size_type __n, const_pointer __first, 
			       const_pointer __last)
#endif /* _STLP_MEMBER_TEMPLATES */
  {
    pointer __result = this->_M_end_of_storage.allocate(__n);
    _STLP_TRY {
#if !defined(__MRC__)		//*TY 12/17/2000 - added workaround for MrCpp. it confuses on nested try/catch block
      __uninitialized_copy(__first, __last, __result, _IsPODType());
#else
      uninitialized_copy(__first, __last, __result);
#endif
      return __result;
    }
    _STLP_UNWIND(this->_M_end_of_storage.deallocate(__result, __n));
# ifdef _STLP_THROW_RETURN_BUG
	return __result;
# endif
  }


#ifdef _STLP_MEMBER_TEMPLATES
  template <class _InputIterator>
  void _M_range_initialize(_InputIterator __first,  
                           _InputIterator __last, const input_iterator_tag &) {
    for ( ; __first != __last; ++__first)
      push_back(*__first);
  }
  // This function is only called by the constructor. 
  template <class _ForwardIterator>
  void _M_range_initialize(_ForwardIterator __first,
                           _ForwardIterator __last, const forward_iterator_tag &) {
    size_type __n = distance(__first, __last);
    this->_M_start = this->_M_end_of_storage.allocate(__n);
    this->_M_end_of_storage._M_data = this->_M_start + __n;
    this->_M_finish = __uninitialized_copy(__first, __last, this->_M_start, _IsPODType());
  }
  
#endif /* _STLP_MEMBER_TEMPLATES */
};

# define _STLP_TEMPLATE_CONTAINER svector<const void*, _Alloc>
# define _STLP_TEMPLATE_HEADER    template <class _Alloc>
# include <stl/_relops_cont.h>
# undef _STLP_TEMPLATE_CONTAINER
# undef _STLP_TEMPLATE_HEADER

# if defined (_STLP_USE_TEMPLATE_EXPORT) 
_STLP_EXPORT_TEMPLATE_CLASS allocator<void*>;
_STLP_EXPORT_TEMPLATE_CLASS _STLP_alloc_proxy<void**, void*, allocator<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS _SVector_base<void*,allocator<void*> >;
_STLP_EXPORT_TEMPLATE_CLASS svector<void*,allocator<void*> >;
# endif

#  undef  svector
#  undef  __svector__
#  define __svector__ __WORKAROUND_RENAME(svector)

__STL_END_NAMESPACE

# if !defined (__STL_LINK_TIME_INSTANTIATION)
#  include <_svector_special.c>
# endif

__STL_BEGIN_NAMESPACE 

	// T*

template <class T, class Allocator>
class svector<T*, Allocator> : private svector<const void*>
{
private:
  typedef svector<const void*> base;
  typedef base::allocator_type base_allocator;
public:
  typedef T* value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

#if defined ( _STLP_CLASS_PARTIAL_SPECIALIZATION )
  typedef _STLP_STD::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef _STLP_STD::reverse_iterator<iterator> reverse_iterator;
#else /* _STLP_CLASS_PARTIAL_SPECIALIZATION */
# if defined (_STLP_MSVC50_COMPATIBILITY)
  typedef _STLP_STD::reverse_iterator<const_iterator, value_type, const_reference, 
  const_pointer, difference_type> const_reverse_iterator;
  typedef _STLP_STD::reverse_iterator<iterator, value_type, reference, reference*, 
  difference_type> reverse_iterator;
# else
  typedef _STLP_STD::reverse_iterator<const_iterator, value_type, const_reference, 
                                  difference_type> const_reverse_iterator;
  typedef _STLP_STD::reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator;
# endif
#endif /* _STLP_CLASS_PARTIAL_SPECIALIZATION */


		explicit svector(const allocator_type& __a = allocator_type());
		explicit svector(size_type n, const value_type& value = 0, const allocator_type& __a = allocator_type());
		void assign(size_type n, const value_type& u);
		allocator_type get_allocator() const;

		inline
		void set_keep_small(bool ksmall)
			{ base::set_keep_small(ksmall); }
		inline
		bool get_keep_small() const
			{ return  base::get_keep_small(); }

		// iterators:
		iterator               begin();
		const_iterator         begin() const;
		iterator               end();
		const_iterator         end() const;
		reverse_iterator       rbegin();
		const_reverse_iterator rbegin() const;
		reverse_iterator       rend();
		const_reverse_iterator rend() const;
		// _lib.svector.capacity_ capacity:
		size_type size() const;
		size_type max_size() const;
		void      resize(size_type sz, value_type c = 0);
		size_type capacity() const;
		bool      empty() const;
		void      reserve(size_type n);

		// element access:
		reference       operator[](size_type n);
		const_reference operator[](size_type n) const;
		const_reference at(size_type n) const;
		reference       at(size_type n);
		reference       front();
		const_reference front() const;
		reference       back();
		const_reference back() const;
		// _lib.svector.modifiers_ modifiers:
		void push_back(const value_type& x);
		void pop_back();
		iterator insert(iterator position, const value_type& x);
		void	 insert(iterator position, const_iterator first, const_iterator last);
		void     insert(iterator position, size_type n, const value_type& x);
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		void     swap(svector&);
		void     clear();
	};

	template <class T, class Allocator>
	inline
	svector<T*, Allocator>::svector(const allocator_type& a)
		: base(base_allocator(a))
	{
	}

	template <class T, class Allocator>
	inline
	svector<T*, Allocator>::svector(size_type n, const value_type& value, const allocator_type& a)
		: base(n, value, base_allocator(a))
	{
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::assign(size_type n, const value_type& u)
	{
		base::assign(n, u);
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::allocator_type
	svector<T*, Allocator>::get_allocator() const
	{
		return base::get_allocator();
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::iterator
	svector<T*, Allocator>::begin()
	{
		return iterator(base::begin());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_iterator
	svector<T*, Allocator>::begin() const
	{
		return const_iterator(base::begin());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::iterator
	svector<T*, Allocator>::end()
	{
		return iterator(base::end());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_iterator
	svector<T*, Allocator>::end() const
	{
		return const_iterator(base::end());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::reverse_iterator
	svector<T*, Allocator>::rbegin()
	{
		return reverse_iterator(end());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_reverse_iterator
	svector<T*, Allocator>::rbegin() const
	{
		return const_reverse_iterator(end());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::reverse_iterator
	svector<T*, Allocator>::rend()
	{
		return reverse_iterator(begin());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_reverse_iterator
	svector<T*, Allocator>::rend() const
	{
		return const_reverse_iterator(begin());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::size_type
	svector<T*, Allocator>::size() const
	{
		return base::size();
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::size_type
	svector<T*, Allocator>::max_size() const
	{
		return base::max_size();
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::resize(size_type sz, value_type c)
	{
		base::resize(sz, c);
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::size_type
	svector<T*, Allocator>::capacity() const
	{
		return base::capacity();
	}

	template <class T, class Allocator>
	inline
	bool
	svector<T*, Allocator>::empty() const
	{
		return base::empty();
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::reserve(size_type n)
	{
		base::reserve(n);
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::reference
	svector<T*, Allocator>::operator[](size_type n)
	{
		return reference(base::operator[](n));
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_reference
	svector<T*, Allocator>::operator[](size_type n) const
	{
		return const_reference(base::operator[](n));
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_reference
	svector<T*, Allocator>::at(size_type n) const
	{
		return const_reference(base::at(n));
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::reference
	svector<T*, Allocator>::at(size_type n)
	{
		return reference(base::at(n));
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::reference
	svector<T*, Allocator>::front()
	{
		return reference(base::front());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_reference
	svector<T*, Allocator>::front() const
	{
		return const_reference(base::front());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::reference
	svector<T*, Allocator>::back()
	{
		return reference(base::back());
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::const_reference
	svector<T*, Allocator>::back() const
	{
		return const_reference(base::back());
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::push_back(const value_type& x)
	{
		base::push_back((base::value_type)x);
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::pop_back()
	{
		base::pop_back();
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::iterator
	svector<T*, Allocator>::insert(iterator position, const value_type& x)
	{
		return iterator(base::insert(base::iterator(position), (base::value_type)x));
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::insert(iterator position, const_iterator first, const_iterator last)
	{
		base::insert(base::iterator(position), base::const_iterator(first), base::const_iterator(last));
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::insert(iterator position, size_type n, const value_type& x)
	{
		base::insert(base::iterator(position), n, (base::value_type)x);
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::iterator
	svector<T*, Allocator>::erase(iterator position)
	{
		return iterator(base::erase(base::iterator(position)));
	}

	template <class T, class Allocator>
	inline
	typename svector<T*, Allocator>::iterator
	svector<T*, Allocator>::erase(iterator first, iterator last)
	{
		return iterator(base::erase(base::iterator(first), base::iterator(last)));
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::swap(svector& rhs)
	{
		base::swap((base&)rhs);
	}

	template <class T, class Allocator>
	inline
	void
	svector<T*, Allocator>::clear()
	{
		base::clear();
	}

	template <class T, class Allocator>
	inline
	bool
	operator==(const svector<T*, Allocator>& x, const svector<T*, Allocator>& y)
	{
		return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
	}

	template <class T, class Allocator>
	inline
	bool
	operator!=(const svector<T*, Allocator>& x, const svector<T*, Allocator>& y)
	{
		return !(x == y);
	}

	template <class T, class Allocator>
	inline
	bool
	operator< (const svector<T*,Allocator>& x, const svector<T*,Allocator>& y)
	{
		return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
	}

	template <class T, class Allocator>
	inline
	bool
	operator> (const svector<T*,Allocator>& x, const svector<T*,Allocator>& y)
	{
		return y < x;
	}

	template <class T, class Allocator>
	inline
	bool
	operator>=(const svector<T*,Allocator>& x, const svector<T*,Allocator>& y)
	{
		return !(x < y);
	}

	template <class T, class Allocator>
	inline
	bool
	operator<=(const svector<T*,Allocator>& x, const svector<T*,Allocator>& y)
	{
		return !(y < x);
	}

	template <class T, class Allocator>
	inline
	void
	swap(svector<T*,Allocator>& x, svector<T*,Allocator>& y)
	{
		x.swap(y);
	}

__STL_END_NAMESPACE
