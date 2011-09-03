/*
    Copyright (c) 2007-2011 Cyrus Daboo. All rights reserved.
    
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


// CCalendarCheckThread : class to do threaded calendar checking

#include "CCalendarCheckThread.h"

#include "CLog.h"
#include "CCalendarStoreManager.h"

#if __dest_os == __mac_os || __dest_os == __mac_os_x
#include <LSimpleThread.h>
#elif __dest_os == __linux_os
#include <ace/Thread.h>
#endif

CCalendarCheckThread::cthread* CCalendarCheckThread::sThread = NULL;
bool CCalendarCheckThread::sRunning = false;
bool CCalendarCheckThread::sDoCheck = false;
bool CCalendarCheckThread::sPause = false;
bool CCalendarCheckThread::sExit = false;
cdmutex CCalendarCheckThread::_can_run;
cdmutex CCalendarCheckThread::_can_exit;

void CCalendarCheckThread::BeginCalendarCheck()
{
	sRunning = false;
	sDoCheck = false;
	sExit = false;
#if __dest_os == __mac_os || __dest_os == __mac_os_x
	// Start suspended so that sThread is allocated before we call the entry point
	sThread = new LSimpleThread(CalendarCheckThread, NULL);
	sThread->Resume();
#elif __dest_os == __win32_os
	// Start suspended so that sThread is allocated before we call the entry point
	sThread = ::AfxBeginThread(CalendarCheckThread, NULL, THREAD_PRIORITY_BELOW_NORMAL, 0, CREATE_SUSPENDED);
	//sThread = ::AfxBeginThread(CalendarCheckThread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	sThread->ResumeThread();
#elif __dest_os == __linux_os
	ACE_Thread::spawn((ACE_THR_FUNC)CalendarCheckThread);
#endif
}

void CCalendarCheckThread::EndCalendarCheck()
{
	// Set flag to force check loop to terminate
	sExit = true;

	// Try to lock exit lock which is released only when calendar check exits
	// NB Lock it on stack so it will be released on exit to allow thread restart
	// Use of mutex to allow messages to be pumped while waiting for calendar check to exit
	cdmutex::lock_cdmutex _lock(_can_exit);

#if __dest_os == __mac_os || __dest_os == __mac_os_x
	// No need to delete: thread deletes itself on exit from thread proc
#elif __dest_os == __win32_os
	// No need to delete: thread deletes itself on exit from thread proc (MFC 4.2.1)
	//delete sThread;
#elif __dest_os == __linux_os
	// No need to delete: thread never really had an "object" to go with it
	// and it will clean up itself on exit.
#endif
	sThread = NULL;
}

#if __dest_os == __mac_os || __dest_os == __mac_os_x
void CCalendarCheckThread::CalendarCheckThread(cthread& thread, void* arg)
#elif __dest_os == __win32_os
UINT CCalendarCheckThread::CalendarCheckThread(LPVOID pParam)
#elif __dest_os == __linux_os
void* CCalendarCheckThread::CalendarCheckThread(void* param)
#endif
{
	// Grab exit lock
	cdmutex::lock_cdmutex _lock(_can_exit);

	// Must catch all errors - cannot throw out of thread!
	try
	{
		// Loop looking for exit
		while(!sExit)
		{
			if (sDoCheck && !sPause)
			{
				// Grab run lock
				cdmutex::lock_cdmutex _run_lock(_can_run);

				sDoCheck = false;
				sRunning = true;
                calstore::CCalendarStoreManager::sCalendarStoreManager->CheckAllSubscribedCalendars();
				sRunning = false;
			}
			else
#if __dest_os == __mac_os || __dest_os == __mac_os_x
				thread.Sleep(1000);
#elif __dest_os == __win32_os
				::Sleep(1000);
#elif __dest_os == __linux_os
				//ACE_OS::sleep looks to take seconds.  I'm guessing the other ones
				//take miliseconds
				{
					ACE_OS::sleep(1);
				}
#endif
		}
	}
	catch (...)
	{
		CLOG_LOGCATCH(...);

		// Just wait for signalled termination
		sRunning = false;
		while(!sExit){
#if __dest_os == __mac_os || __dest_os == __mac_os_x
			thread.Sleep(1000);
#elif __dest_os == __win32_os
			::Sleep(1000);
#elif __dest_os == __linux_os
			ACE_OS::sleep(1);
#endif
		}
	}

#if __dest_os == __win32_os || __dest_os == __linux_os
	return 0;
#endif
}

#if __dest_os == __win32_os
// It is sometimes necessary to temporarily boost the priority of this thread to ensure the main
// thread does not block waiting on the thread mutex. That is because the calendar check thread normally
// runs at a lower priority than the main thread on Win32
void CCalendarCheckThread::BoostPriority()
{
	if ((sThread != NULL) && sRunning)
	{
		sThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
		::Sleep(250);
		sThread->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	}
}
#endif
