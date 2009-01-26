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
	CWebDAVDefinitions.h

	Author:			
	Description:	<describe the CWebDAVDefinitions class here>
*/

#ifndef CWebDAVDefinitions_H
#define CWebDAVDefinitions_H

#include "XMLName.h"

namespace http {

namespace webdav 
{

// RFC2518 ¤ - WebDAV Properties

extern const char*	cNamespace;

extern const xmllib::XMLName	cElement_multistatus;
extern const xmllib::XMLName	cElement_response;
extern const xmllib::XMLName	cElement_href;
extern const xmllib::XMLName	cElement_status;

extern const xmllib::XMLName	cElement_propname;
extern const xmllib::XMLName	cElement_propfind;
extern const xmllib::XMLName	cElement_prop;
extern const xmllib::XMLName	cElement_propstat;

extern const xmllib::XMLName	cProperty_getcontentlength;
extern const xmllib::XMLName	cProperty_getcontenttype;
extern const xmllib::XMLName	cProperty_resourcetype;
extern const xmllib::XMLName	cProperty_collection;
extern const xmllib::XMLName	cProperty_getetag;

// RFC3744 ¤ - ACL Properties
extern const xmllib::XMLName	cProperty_supported_privilege_set;

extern const xmllib::XMLName	cProperty_current_user_privilege_set;

extern const xmllib::XMLName	cProperty_acl;
	extern const xmllib::XMLName	cProperty_ace;
		extern const xmllib::XMLName	cProperty_principal;
		extern const xmllib::XMLName	cProperty_href;
		extern const xmllib::XMLName	cProperty_all;
		extern const xmllib::XMLName	cProperty_authenticated;
		extern const xmllib::XMLName	cProperty_unauthenticated;
		extern const xmllib::XMLName	cProperty_property;
		extern const xmllib::XMLName	cProperty_self;
		extern const xmllib::XMLName	cProperty_invert;
		extern const xmllib::XMLName	cProperty_grant;
		extern const xmllib::XMLName	cProperty_deny;
		extern const xmllib::XMLName	cProperty_protected;
		extern const xmllib::XMLName	cProperty_inherited;

extern const xmllib::XMLName	cProperty_privilege;
	extern const xmllib::XMLName	cProperty_privilege_read;
	extern const xmllib::XMLName	cProperty_privilege_write;
	extern const xmllib::XMLName	cProperty_privilege_write_properties;
	extern const xmllib::XMLName	cProperty_privilege_write_content;
	extern const xmllib::XMLName	cProperty_privilege_unlock;
	extern const xmllib::XMLName	cProperty_privilege_read_acl;
	extern const xmllib::XMLName	cProperty_privilege_read_current_user_privilege_set;
	extern const xmllib::XMLName	cProperty_privilege_write_acl;
	extern const xmllib::XMLName	cProperty_privilege_bind;
	extern const xmllib::XMLName	cProperty_privilege_unbind;
	extern const xmllib::XMLName	cProperty_privilege_all;

}

namespace caldav 
{

extern const char*	cNamespace;

extern const xmllib::XMLName	cProperty_caldavcalendar;

extern const xmllib::XMLName	cElement_calendarquery;
extern const xmllib::XMLName	cElement_calendarmultiget;
extern const xmllib::XMLName	cElement_freebusyquery;

	extern const xmllib::XMLName	cElement_calendardata;
		extern const xmllib::XMLName	cAttribute_returncontenttype;

	extern const xmllib::XMLName	cElement_comp;
		extern const xmllib::XMLName	cAttribute_name;
	extern const xmllib::XMLName	cElement_allcomp;
	extern const xmllib::XMLName	cElement_allprop;
	extern const xmllib::XMLName	cElement_prop;
		extern const xmllib::XMLName	cAttribute_novalue;
	extern const xmllib::XMLName	cElement_expandrecurrenceset;
		extern const xmllib::XMLName	cAttribute_start;
		extern const xmllib::XMLName	cAttribute_end;

	extern const xmllib::XMLName	cElement_filter;
	extern const xmllib::XMLName	cElement_compfilter;
	extern const xmllib::XMLName	cElement_propfilter;
	extern const xmllib::XMLName	cElement_paramfilter;

	extern const xmllib::XMLName	cElement_isdefined;
	extern const xmllib::XMLName	cElement_textmatch;
		extern const xmllib::XMLName	cAttribute_caseless;
	extern const xmllib::XMLName	cElement_timerange;

	extern const xmllib::XMLName	cAttributeValue_yes;
	extern const xmllib::XMLName	cAttributeValue_no;
}


namespace slide
{
extern const char*	cNamespace;

	extern const xmllib::XMLName	cInheritable_comp;
	extern const xmllib::XMLName	cNonInheritable_comp;
}

}	// namespace http

#endif	// CWebDAVRequest_H
