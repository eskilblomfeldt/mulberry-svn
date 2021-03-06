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
	CWebDAVMove.h

	Author:			
	Description:	<describe the CWebDAVMove class here>
*/

#ifndef CWebDAVMove_H
#define CWebDAVMove_H

#include "CWebDAVRequestResponse.h"

using namespace http; 

namespace http {

namespace webdav {

class CWebDAVMove: public CWebDAVRequestResponse
{
public:
	CWebDAVMove(CWebDAVSession* session, const cdstring& ruri_old, const cdstring& absuri_new, bool overwrite = false);
	virtual ~CWebDAVMove();

	void SetData();
	void SetData(const cdstring& etag);

protected:
	cdstring mMoveToAbsURI;
	bool mOverWrite;

	virtual void WriteHeaderToStream(ostream& os);

};

}	// namespace webdav

}	// namespace http

#endif	// CWebDAVMove_H
