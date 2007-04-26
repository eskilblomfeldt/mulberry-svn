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


// ACL Support for address books

#ifndef __CADBKACL__MULBERRY__
#define __CADBKACL__MULBERRY__

#include "CACL.h"

// Typedefs
class CAdbkACL;
typedef svector<CAdbkACL> CAdbkACLList;

// CAdbkACL: contains specific ACL item for a mailbox

class CAdbkACL : public CACL
{
public:

	enum EAdbkACL
	{
		eAdbkACL_Lookup = 1L << 0,
		eAdbkACL_Read = 1L << 1,
		eAdbkACL_Write = 1L << 2,
		eAdbkACL_Create = 1L << 3,
		eAdbkACL_Delete = 1L << 4,
		eAdbkACL_Admin = 1L << 5
	};

	CAdbkACL() {}
	CAdbkACL(const CAdbkACL& copy)						// Copy construct
		: CACL(copy) {}

	virtual 		~CAdbkACL() {}
	
	CAdbkACL& operator=(const CAdbkACL& copy);			// Assignment with same type
	int operator==(const CAdbkACL& test) const			// Compare with same type
		{ return CACL::operator==(test); }
	int operator!=(const CAdbkACL& test) const			// Compare with same type
		{ return !operator==(test); }

	virtual cdstring	GetTextRights() const;		// Get full text form of rights
	virtual cdstring	GetFullTextRights() const;	// Get full text form of rights

private:
	virtual void	ParseRights(const char* txt, SACLRight& rights);		// Parse string to specified rigths location

};

#endif
