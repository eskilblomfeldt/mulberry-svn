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
	CWebDAVSession.cpp

	Author:			
	Description:	<describe the CWebDAVSession class here>
*/

#include "CWebDAVSession.h"

#include "CHTTPAuthorization.h"
#include "CHTTPDataString.h"
#include "CHTTPRequestResponse.h"

using namespace webdav; 

CWebDAVSession::CWebDAVSession()
{
	mInitialised = false;
	mVersion = eDAVUnknown;
}

CWebDAVSession::CWebDAVSession(const cdstring& host) :
	CHTTPSession(host)
{
	mInitialised = false;
	mVersion = eDAVUnknown;
}

CWebDAVSession::~CWebDAVSession()
{
}

// Initialise the session by contacting the server and making sure that the base_uri supports DAV
// Also check whether authorization is required
bool CWebDAVSession::Initialise(const cdstring& host, const cdstring& base_uri)
{
	// Set host change
	SetHost(host);
	
	// Loop repeating until we can do it or get a fatal error
	bool first_time = true;
	while(true)
	{
		// Create OPTIONS request for the base_uri
		auto_ptr<CHTTPRequestResponse> request(new CHTTPRequestResponse(this, CHTTPRequestResponse::eRequest_OPTIONS, base_uri));
		request->SetSession(this);
		CHTTPOutputDataString sout;
		request->SetData(NULL, &sout);

		// Add request and process it
		SendRequest(request.get());
		
		// Check response
		if (request->GetStatusCode() == eStatus_Unauthorized)
		{
			// If we had authorization before, then chances are auth details are wrong - so delete and try again with new auth
			if (HasAuthorization())
			{
				delete mAuthorization;
				mAuthorization = NULL;
				
				// Display error so user knows why the prompt occurs again
				DisplayHTTPError(request.get());
			}

			// Get authorization object (prompt the user) and redo the request
			mAuthorization = GetAuthorization(first_time, request->GetResponseHeader(cHeaderWWWAuthenticate));
			
			// Check for auth cancellation
			if (mAuthorization == (CHTTPAuthorization*) -1)
			{
				mAuthorization = NULL;
				return false;
			}

			first_time = false;
			
			// Repeat the request loop with new authorization
			continue;
		}
		
		// Look for success and exit loop for further processing
		if ((request->GetStatusCode() == eStatus_OK) || (request->GetStatusCode() == eStatus_NoContent))
		{
			// Grab the server string
			if (request->GetResponseHeaders().count(cHeaderServer) != 0)
			{
				SetServerDescriptor((*request->GetResponseHeaders().find(cHeaderServer)).second);
			}
			
			// Now check the response headers for a DAV version
			if (request->GetResponseHeaders().count(cHeaderDAV) != 0)
			{
				// Get the dav version value
				cdstring dav_version = (*request->GetResponseHeaders().find(cHeaderDAV)).second;
				mVersion = eDAVUnknown;

				// Tokenize on commas
				const char* p = ::strtok(dav_version.c_str_mod(), ",");
				while(p != NULL)
				{
					cdstring temp(p);
					temp.trimspace();
					if (temp == cHeaderDAV1)
						mVersion |= eDAV1;
					if (temp == cHeaderDAV2)
						mVersion |= eDAV2;
					if (temp == cHeaderDAVbis)
						mVersion |= eDAV2bis;
					if (temp == cHeaderDAVACL)
						mVersion |= eDAVACL;
					if (temp == caldav::cHeaderCalendarAccess)
						mVersion |= eCALDAVaccess;
					if (temp == caldav::cHeaderCalendarSchedule)
						mVersion |= eCALDAVsched;
					if (temp == slide::cHeaderCyrusoftInheritable)
						mVersion |= eCyrusoftInheritable;
					
					p = ::strtok(NULL, ",");
				}
				
				SetServerType(mVersion);
			}

			// Put other strings into capability
			cdstring capa;
			for(cdstrmultimapcasei::const_iterator iter = request->GetResponseHeaders().begin(); iter != request->GetResponseHeaders().end(); iter++)
			{
				if (((*iter).first.compare(cHeaderServer, true) != 0) &&
					((*iter).first.compare(cHeaderDate, true) !=0) &&
					((*iter).first.compare_start("Content-", true) != 0))
				{
					capa += (*iter).first;
					capa += ": ";
					capa += (*iter).second;
					capa += os_endl;
				}
			}
			SetServerCapability(capa);

			// Just assume any version is fine for now
			break;
		}
		
		// If we get here we had some kind of fatal error
		HandleHTTPError(request.get());
		return false;
	}
	
	mInitialised = true;

	return true;
}
