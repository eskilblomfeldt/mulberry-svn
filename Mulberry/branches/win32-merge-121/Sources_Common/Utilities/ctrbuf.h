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


// ctrbuf.h
//
// Copyright 2006, Cyrus Daboo.  All Rights Reserved.
//
// Created: 21-Nov-1997
// Author: Cyrus Daboo
// Platforms: Mac OS, Win32
//
// Description:
// This class implements a streambuf that counts bytes sent to it.
//
// History:
// 21-Nov-1997: Created initial header and implementation.
//

#ifndef __CTRBUF__MULBERRY__
#define __CTRBUF__MULBERRY__

#ifdef __MSL__
#include <mslconfig>
#endif

#include <iostream.h>

#if __MSL__
#pragma options align=native
#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
	#pragma import on
#endif

#ifdef MSIPL_USING_NAMESPACE
namespace std {
#endif

#define __msipl_temp_bufsize   8
#endif

class ctrbuf : public streambuf
{
public:
    ctrbuf();
    virtual ~ctrbuf();

	size_t get_size(void)
		{ return ctr; }
	void reset(void)
		{ ctr = 0; }
#ifndef __MSL__
    typedef char char_type;
    typedef long int_type;
#endif
protected:
     virtual streamsize xsputn (const char_type* s, streamsize n);
#ifdef __MSL__
     virtual int_type overflow (int_type c=traits_type::eof ());
#else
     virtual int_type overflow (int_type c=EOF);
#endif
private:
    size_t ctr;
};

inline ctrbuf::ctrbuf()
	: streambuf()
{
	ctr = 0;
}

inline ctrbuf::~ctrbuf()
{
}

inline ctrbuf::int_type ctrbuf::overflow (int_type c)
{
#ifdef __MSL__
    if (traits_type::eq_int_type (c, traits_type::eof ()))
        return traits_type::not_eof (c);
#else
    if (c == EOF) {
      return EOF + 1;
    }
#endif

	ctr++;
    return (int_type) c;
}

inline streamsize ctrbuf::xsputn (const char_type* s, streamsize n)
{
	ctr += n;
	return n;
}

#ifdef __MSL__
#ifdef MSIPL_USING_NAMESPACE
} /* namespace std */
#endif

#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
	#pragma import reset
#endif
#pragma options align=reset
#endif

#endif
