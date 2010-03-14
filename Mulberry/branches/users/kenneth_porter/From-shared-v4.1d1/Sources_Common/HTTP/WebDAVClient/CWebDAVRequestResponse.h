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
	CWebDAVRequestResponse.h

	Author:			
	Description:	<describe the CWebDAVRequestResponse class here>
*/

#ifndef CWebDAVRequestResponse_H
#define CWebDAVRequestResponse_H

#include "CHTTPRequestResponse.h"

namespace http {

namespace webdav {

class CWebDAVSession;

class CWebDAVRequestResponse : public CHTTPRequestResponse
{
public:
	CWebDAVRequestResponse(const CWebDAVSession* session, ERequestMethod method, const cdstring& ruri);
	CWebDAVRequestResponse(const CWebDAVSession* session, ERequestMethod method, const cdstring& ruri, const cdstring& lock);
	CWebDAVRequestResponse(const CWebDAVSession* session, ERequestMethod method, const cdstring& ruri, const cdstring& etag, bool etag_match);
	virtual ~CWebDAVRequestResponse();

	void SetLock(cdstring& lock)
	{
		mLock = lock;
	}

protected:
	cdstring				mLock;
	
	virtual void WriteHeaderToStream(std::ostream& os);
};

}	// namespace webdav

}	// namespace http

#endif	// CWebDAVRequestResponse_H
