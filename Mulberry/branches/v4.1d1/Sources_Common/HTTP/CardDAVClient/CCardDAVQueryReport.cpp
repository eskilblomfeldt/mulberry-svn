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
	CCardDAVQueryReport.cpp

	Author:			
	Description:	<describe the CCardDAVQueryReport class here>
*/

#include "CCardDAVQueryReport.h"

#include "CHTTPDataString.h"
#include "CWebDAVDefinitions.h"
#include "XMLDocument.h"
#include "XMLNode.h"

#include <strstream>

using namespace webdav; 
using namespace carddav; 

CCardDAVQueryReport::CCardDAVQueryReport(CWebDAVSession* session, const cdstring& ruri, const cdstring& search_prop, const cdstring& search_text) :
	CWebDAVReport(session, ruri)
{
	InitRequestData(search_prop, search_text);
}


CCardDAVQueryReport::~CCardDAVQueryReport()
{
}


void CCardDAVQueryReport::InitRequestData(const cdstring& search_prop, const cdstring& search_text)
{
	// Write XML info to a string
	std::ostrstream os;
	GenerateXML(os, search_prop, search_text);
	os << ends;
	
	mRequestData = new CHTTPInputDataString(os.str(), "text/xml; charset=utf-8");
}

void CCardDAVQueryReport::GenerateXML(std::ostream& os, const cdstring& search_prop, const cdstring& search_text)
{
	using namespace xmllib;

	// Structure of document is:
	//
	// <CardDAV:adbk-query>
	//   <DAV:prop>
	//     <DAV:getetag>
	//   </DAV:prop>
	//   <C:adbk-data/>
	//	 <C:filter>
	//	 <C:comp-filter>
	//	   <C:prop-filter name="...">
	//	     <C:text-match caseless="yes">...</C:text-match>
	//	   </C:prop-filter>
	//	 </C:comp-filter>
	//	 </C:filter>
	//   ...
	// </CardDAV:adbk-query>
	
	// Create document and get the root
	xmllib::XMLDocument xmldoc;

	// <CardDAV:adbk-query> element
	xmllib::XMLNode* query = xmldoc.GetRoot();
	xmllib::XMLNamespace dav_namespc(webdav::cNamespace, "D");
	xmllib::XMLNamespace carddav_namespc(carddav::cNamespace, "C");
	query->SetName(cElement_adbkquery.Name(), carddav_namespc);
	query->AddNamespace(dav_namespc);
	query->AddNamespace(carddav_namespc);
	
	// <DAV:prop> element
	xmllib::XMLNode* prop = new xmllib::XMLNode(&xmldoc, query, cElement_prop.Name(), dav_namespc);
	
	// <DAV:getetag> element
	new xmllib::XMLNode(&xmldoc, prop, cProperty_getetag.Name(), dav_namespc);
	
	// <CardDAV:adbk-data> element
	new xmllib::XMLNode(&xmldoc, query, cElement_adbkdata.Name(), carddav_namespc);
	
	// <CardDAV:filter> element
	xmllib::XMLNode* filter = new xmllib::XMLNode(&xmldoc, query, cElement_filter.Name(), carddav_namespc);
	
	// <CardDAV:comp-filter> element
	xmllib::XMLNode* comp_filter = new xmllib::XMLNode(&xmldoc, filter, cElement_compfilter.Name(), carddav_namespc);
	
	// <CardDAV:prop-filter name="..."> element
	xmllib::XMLNode* prop_filter = new xmllib::XMLNode(&xmldoc, comp_filter, cElement_propfilter.Name(), carddav_namespc);
	prop_filter->AddAttribute(cAttribute_name.Name(), search_prop);

	// <C:text-match caseless="yes">...</C:text-match>
	xmllib::XMLNode* text_match = new xmllib::XMLNode(&xmldoc, prop_filter, cElement_textmatch.Name(), carddav_namespc, search_text);
	text_match->AddAttribute(cAttribute_caseless.Name(), cAttributeValue_yes.Name());

	// Now we have the complete document, so write it out (no indentation)
	xmldoc.Generate(os, false);
}
