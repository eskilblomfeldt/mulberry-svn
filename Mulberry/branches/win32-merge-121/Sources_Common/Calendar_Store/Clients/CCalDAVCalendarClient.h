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
	CCalDAVCalendarClient.h

	Author:			
	Description:	<describe the CCalDAVCalendarClient class here>
*/

#ifndef CCalDAVCalendarClient_H
#define CCalDAVCalendarClient_H

#include "CWebDAVCalendarClient.h"

namespace iCal
{
	class CICalendarComponent;
	class CICalendarComponentDB;
}

namespace calstore {

class CCalDAVCalendarClient : public CWebDAVCalendarClient
{
public:
	CCalDAVCalendarClient(CCalendarProtocol* owner);
	CCalDAVCalendarClient(const CCalDAVCalendarClient& copy, CCalendarProtocol* owner);
	virtual ~CCalDAVCalendarClient();

private:
			void	InitCalDAVClient();

public:
	virtual CINETClient*	CloneConnection();			// Create duplicate, empty connection

protected:

	// P R O T O C O L

	// C A L E N D A R

	virtual void _CreateCalendar(const CCalendarStoreNode& node);
	virtual bool _CalendarChanged(const CCalendarStoreNode& node, iCal::CICalendar& cal);

	virtual void _ReadFullCalendar(const CCalendarStoreNode& node, iCal::CICalendar& cal);
	virtual void _WriteFullCalendar(const CCalendarStoreNode& node, iCal::CICalendar& cal);

	virtual bool _CanUseComponents() const;
	virtual void _GetComponentInfo(const CCalendarStoreNode& node, iCal::CICalendar& cal, cdstrmap& comps);
	virtual void _AddComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponent& component);
	virtual void _ChangeComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponent& component);
	virtual void _RemoveComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponent& component);
	virtual void _RemoveComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const cdstring& rurl);
	virtual void _ReadComponents(const CCalendarStoreNode& node, iCal::CICalendar& cal, const cdstrvect& rurls);
	virtual iCal::CICalendarComponent* _ReadComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const cdstring& rurl);

	virtual void ListCalendars(CCalendarStoreNode* root, const http::webdav::CWebDAVPropFindParser& parser);

	virtual void ReadCalendarComponents(const CCalendarStoreNode& node, const http::webdav::CWebDAVPropFindParser& parser, iCal::CICalendar& cal);
	virtual void ReadCalendarComponents(const CCalendarStoreNode& node, const cdstrvect& hrefs, iCal::CICalendar& cal);
	virtual void GetCalendarComponents(const CCalendarStoreNode& node, const http::webdav::CWebDAVPropFindParser& parser, iCal::CICalendar& cal, cdstrmap& compinfo, bool last_path);
	virtual iCal::CICalendarComponent* ReadCalendarComponent(const cdstring& url, iCal::CICalendar& cal);
	
	virtual void WriteCalendarComponents(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponentDB& components);
	virtual void WriteCalendarComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponent& component);

	virtual void AddComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponent& component);
	virtual void ChangeComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponent& component);
	virtual void WriteComponent(const CCalendarStoreNode& node, iCal::CICalendar& cal, const iCal::CICalendarComponent& component);

	virtual void SizeCalendar_DAV(CCalendarStoreNode& node);
	virtual void SizeCalendar_HTTP(CCalendarStoreNode& node);

	virtual cdstring GetRURL(const CCalendarStoreNode* node, bool abs = false) const;
	virtual cdstring GetRURL(const cdstring& name, bool directory, bool abs = false) const;

};

}	// namespace calstore

#endif	// CCalDAVCalendarClient_H
