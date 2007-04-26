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


// Header for pvector class

#ifndef __PVECTOR__MULBERRY__
#define __PVECTOR__MULBERRY__

#include "svector.h"
#include <iterator>

// Classes

#ifdef MSIPL_USING_NAMESPACE
	namespace std {
#endif

template <class T> class pvector : public svector<void*>
{
public:
    typedef          allocator<T>                     allocator_type;
    typedef          T                                value_type;
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename svector<T>::iterator	      	  iterator;
    typedef typename svector<T>::const_iterator       const_iterator;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
    typedef typename allocator_type::size_type        size_type;
    typedef typename allocator_type::difference_type  difference_type;

    typedef typename std::reverse_iterator<const_iterator>	const_reverse_iterator;
    typedef typename std::reverse_iterator<iterator>		reverse_iterator;

		pvector() {};
		~pvector() {};

	iterator begin()
		{ return *static_cast<iterator*>((void*) &svector<void*>::begin());  }
	const_iterator begin() const
		{ return *static_cast<const_iterator*>((void*) &svector<void*>::begin());;  }
	iterator end()
		{ return *static_cast<iterator*>((void*) &svector<void*>::end());  }
	const_iterator end() const
		{ return *static_cast<const_iterator*>((void*) &svector<void*>::end());;  }

    reverse_iterator       rbegin () 
      { return  reverse_iterator (end ()); }
    const_reverse_iterator rbegin () const
      { return const_reverse_iterator (end ()); }
    reverse_iterator       rend () 
      { return reverse_iterator (begin ()); }
    const_reverse_iterator rend () const 
      { return const_reverse_iterator (begin ()); }

	const_reference at(size_type n) const
		{ return (const_reference) svector<void*>::at(n); }
	reference at(size_type n)
		{ return (reference) svector<void*>::at(n); }

	reference front()
		{ return *begin(); }
	const_reference front() const
		{ return *begin(); }
	reference back()
		{ return *(end() - 1); }
	const_reference back() const
		{ return *(end() - 1); }

    void push_back(const T& x)
    	{ svector<void*>::push_back((void*&) x); }

    iterator insert(iterator position, const T& x = T ())
    	{ return  *static_cast<iterator*>((void*) &svector<void*>::insert(*static_cast<svector<void*>::iterator*>((void*) &position), x)); }
    void insert(iterator position, size_type n, const T& x)
    	{ svector<void*>::insert(*static_cast<svector<void*>::iterator*>((void*) &position), n, x); }
    void insert(iterator position, 
                  const_iterator first, const_iterator last)
    	{ svector<void*>::insert(*static_cast<svector<void*>::iterator*>((void*) &position),
    								*static_cast<svector<void*>::iterator*>((void*) &first),
    								*static_cast<svector<void*>::iterator*>((void*) &last)); }

    iterator erase(iterator position)
    	{ return *static_cast<iterator*>((void*) &svector<void*>::erase(*static_cast<svector<void*>::iterator*>((void*) &position))); }
    iterator erase(iterator first, iterator last)
    	{ return *static_cast<iterator*>((void*) &svector<void*>::erase(*static_cast<svector<void*>::iterator*>((void*) &first), *static_cast<svector<void*>::iterator*>((void*) &last))); }
};

#ifdef MSIPL_USING_NAMESPACE
	} // namespace std 
#endif

#endif
