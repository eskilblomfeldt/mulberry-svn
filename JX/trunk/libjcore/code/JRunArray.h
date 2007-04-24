/******************************************************************************
 JRunArray.h

	Interface for JRunArray class

	Copyright � 1994-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JRunArray
#define _H_JRunArray

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>
#include <JRunArrayIterator.h>

template <class T>
class JRunArrayElement
{
public:

	JSize	length;
	T		data;

	JRunArrayElement()
		:
		length( 0 )
		{ };

	JRunArrayElement
		(
		const JSize	runLength,
		const T&	runData
		)
		:
		length( runLength ),
		data( runData )
		{ };
};

template <class T>
class JRunArray : public JOrderedSet<T>
{
	friend class JRunArrayIterator<T>;

public:

	JRunArray();
	JRunArray(const JRunArray<T>& source);

	virtual ~JRunArray();

	const JRunArray<T>& operator=(const JRunArray<T>& source);

	virtual void	InsertElementAtIndex(const JIndex index, const T& data);
	void			InsertElementsAtIndex(const JIndex elementIndex,
										  const T& data, const JSize newElementCount);
	void			PrependElements(const T& data, const JSize newElementCount);
	void			AppendElements(const T& data, const JSize newElementCount);

	void			InsertElementsAtIndex(const JIndex insertionIndex,
										  const JRunArray<T>& source,
										  const JIndex firstIndex, const JSize count);

	virtual void	RemoveNextElements(const JIndex firstIndex, const JSize count);
	virtual void	RemoveAll();

	virtual const T	GetElement(const JIndex index) const;
	virtual void	SetElement(const JIndex index, const T& data);
	void			SetNextElements(const JIndex firstIndex, const JSize count, const T& data);

	virtual void	MoveElementToIndex(const JIndex currentIndex, const JIndex newIndex);
	virtual void	SwapElements(const JIndex index1, const JIndex index2);

	#if !defined J_NO_HAS_STATIC_TEMPLATE_DATA
	virtual void	Sort();
	virtual JIndex	SearchSorted1(const T& target,
								  const JOrderedSetT::SearchReturn which,
								  JBoolean* found) const;
	#endif

	#if !defined __SUNPRO_CC
	void	QuickSort(int (*compare)(const void*, const void*));
	#endif

	virtual JOrderedSetIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0);
	virtual JOrderedSetIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const;

	void	SetBlockSize(const JSize newBlockSize);

	// specific to JRunArray

	JSize		GetRunCount() const;
	JSize		GetRunLength(const JIndex runIndex) const;
	T			GetRunData(const JIndex runIndex) const;
	const T&	GetRunDataRef(const JIndex runIndex) const;
	void		SetRunData(const JIndex runIndex, const T& data);
	JBoolean	FindRun(const JIndex elementIndex,
						JIndex* runIndex, JIndex* firstIndexInRun) const;

	JInteger	SumElements(const JIndex startIndex, const JIndex endIndex,
							JInteger (*value)(const T& data)) const;
	JBoolean	FindPositiveSum(const JInteger requestedSum, const JIndex startIndex,
								JIndex* endIndex, JInteger* trueSum,
								JInteger (*value)(const T& data)) const;

	// use with extreme care

	void		InsertElementsAtIndex(const JIndex elementIndex,
									  const T& data, const JSize newElementCount,
									  JIndex* runIndex, JIndex* firstIndexInRun);
	void		RemoveNextElements(const JIndex firstIndex, const JSize count,
								   JIndex* runIndex, JIndex* firstIndexInRun);
	void		SetNextElements(const JIndex firstIndex, const JSize count, const T& data,
								JIndex* runIndex, JIndex* firstIndexInRun);
	JBoolean	FindRun(const JIndex origIndex, const JIndex newIndex,
						JIndex* runIndex, JIndex* firstIndexInRun) const;

private:

	JArray< JRunArrayElement<T> >*	itsRuns;	// JArray object that stores the data

	#if !defined J_NO_HAS_STATIC_TEMPLATE_DATA
	static const JElementComparison<T>*	itsCurrentCompareObj;	// NULL unless sorting
	#endif

private:

	JIndex	PrivateInsertElementsAtIndex(const JIndex elementIndex,
										 const T& data, const JSize newElementCount);
	JIndex	PrivateInsertElementsAtIndex(const JIndex elementIndex,
										 const T& data, const JSize newElementCount,
										 JIndex* runIndex, JIndex* firstIndexInRun);

	void	PrivateRemoveNextElements(const JIndex firstIndex, const JSize count);
	void	PrivateRemoveNextElements(const JIndex firstIndex, const JSize count,
									  JIndex* runIndex, JIndex* firstIndexInRun);

	JBoolean	PrivateSetElement(const JIndex elementIndex, const T& data);
	JBoolean	PrivateSetElement(const JIndex elementIndex, const T& data,
								  JIndex* runIndex, JIndex* firstIndexInRun);

	void	IteratorSetElement(const JIndex elementIndex, const T& data,
							   const JIndex runIndex, const JIndex firstIndexInRun);
	void	IteratorRemoveNextElements(const JIndex firstIndex, const JSize count,
									   const JIndex runIndex, const JIndex firstIndexInRun);

	void	InsertRun(const JIndex runIndex, const JSize runLength, const T& item);
	void	RemoveRun(const JIndex runIndex,
					  JIndex* newRunIndex = NULL, JIndex* newFirstInRun = NULL);

	void	SetRunLength(const JIndex runIndex, const JSize newLength);
	void	IncrementRunLength(const JIndex runIndex, const JSize delta);
	void	DecrementRunLength(const JIndex runIndex, const JSize delta);

	void	SetRunInfo(const JIndex runIndex, const JSize newLength,
					   const T& data);

	void	MergeAdjacentRuns();

	#if !defined J_NO_HAS_STATIC_TEMPLATE_DATA
	static JOrderedSetT::CompareResult
		CompareRuns(const JRunArrayElement<T>& r1, const JRunArrayElement<T>& r2);
	#endif

	#if !defined __SUNPRO_CC
	static int	QuickSortCompareRuns(const void* p1, const void* p2);
	#endif
};

#endif
