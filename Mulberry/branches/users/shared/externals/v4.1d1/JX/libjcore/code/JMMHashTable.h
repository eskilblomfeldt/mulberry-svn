#ifndef _H_JMMHashTable
#define _H_JMMHashTable

/******************************************************************************
 JMMHashTable.h

	Interface for the JMMHashTable class.

	Copyright � 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/


// Superclass header
#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JMMTable.h>

#include <JHashTable.h>

#include <JArray.h>

class JMMHashTable : public JMMTable
{
public:

	JMMHashTable(JMemoryManager* manager, const JBoolean recordDelete);
	virtual ~JMMHashTable();

	virtual JSize GetAllocatedCount() const;
	virtual JSize GetDeletedCount() const;
	virtual JSize GetTotalCount() const;

	virtual void PrintAllocated(const JBoolean printInternal = kJFalse) const;

protected:

	virtual void _CancelRecordDeallocated();

	virtual void _AddNewRecord(const JMMRecord& record,
	                                   const JBoolean checkDoubleAllocation);

	virtual JBoolean _SetRecordDeleted(JMMRecord* record, const void* block,
	                                   const JCharacter* file, const JUInt32 line,
	                                   const JBoolean isArray);

private:

	JHashTable<JMMRecord>* itsAllocatedTable;
	JHashTable<JMMRecord>* itsDeletedTable;

private:

	// not allowed

	JMMHashTable(const JMMHashTable& source);
	const JMMHashTable& operator=(const JMMHashTable& source);
};

#endif
