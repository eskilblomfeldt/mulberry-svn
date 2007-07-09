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

/* 
	CCardDAVMakeAdbk.h

	Author:			
	Description:	<describe the CCardDAVMakeAdbk class here>
*/

#ifndef CCardDAVMakeAdbk_H
#define CCardDAVMakeAdbk_H

#include "CWebDAVRequestResponse.h"

using namespace http; 
using namespace webdav; 

namespace http {

namespace carddav {

class CCardDAVMakeAdbk: public CWebDAVRequestResponse
{
public:
	CCardDAVMakeAdbk(CWebDAVSession* session, const cdstring& ruri);
	virtual ~CCardDAVMakeAdbk();

};

}	// namespace carddav

}	// namespace http

#endif	// CCardDAVMakeAdbk_H
