#pragma once

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>

// Keeps the n most recent elements without storing duplicates
//
// T is the type of element to be stored in the set
// T needs to be hashable, or you need to provide a Hash
/*
 * Comparison: 
 * 	1) Keeping the strings in a circular buffer:
 * 		The memory consumption will grow linearly with the
 * 		size of the buffer and average size of a string
 * 	2) Using FlyweightCircularBuffer
 * 		Memory grows with the number of unique strings
 * 		with a bit of overhead used for bookkeeping.
 * 		This is a tradeoff. FlyweightCircularBuffer uses
 * 		slightly more CPU and potentially much less memory.
 *
 * 		Worst case memory complexity: object_size x number of objects 
 * 		Best case memory complexity: object_size 
 * 
 * Open question: can we wrap mElementIdx around mSize so that it doesn't grow indefinitely? 
 *
 * To do: 
 * 	- instead of a reference counting map, use an array of smart pointers with a custom deleter that deletes the element
 * 	- swap the arguments of EXPECT_EX in the unit tests
 * */
template<typename T,
	 typename Hasher = std::hash<T>>
class FlyweightCircularBuffer
{
public:

	explicit FlyweightCircularBuffer(int size) 
	: mCircularBuffer(size, nullptr),
	  mSize(size)
	{
	}

	void Insert(const T& element)
	{
		const auto& [mapIt, inserted] = mReferenceCountingMap.emplace(element, 1);
		++mapIt->second;

		if (auto* oldestElement = mCircularBuffer[mElementIdx%mSize]; oldestElement)
		{
			const auto& oldestIt = mReferenceCountingMap.find(*oldestElement);
			assert(oldestIt != mReferenceCountingMap.end() 
					&& "Broken invariant: the elements should be found in the map");

			if (--oldestIt->second == 1)
			{
				mReferenceCountingMap.erase(oldestIt);
			}
		}

		mCircularBuffer[mElementIdx%mSize] = std::addressof(mapIt->first);
		++mElementIdx;
	}

	const T& Query(int idx) const
	{
		if (idx >= std::min(mSize, mElementIdx))
			throw std::out_of_range("index out of bounds");

		int firstIdx = std::max(mElementIdx - mSize, 0);
		int bufferIdx = (firstIdx + idx) % mSize;
		return *mCircularBuffer.at(bufferIdx); 
	}

	inline bool Contains(const T& element) const
	{
		return mReferenceCountingMap.count(element);
	}

	inline int DistinctObjectCount() const { return mReferenceCountingMap.size(); }
	inline int NumberOfElements() const { return std::min(mElementIdx, mSize); }
	inline int Size() const { return mSize; }

	// Will throw if the object is not in mReferenceCountingMap
	inline int CountOccurrences(const T& element) const { return mReferenceCountingMap.at(element); }

	template<typename U>
	friend std::ostream& operator<<(std::ostream&, const FlyweightCircularBuffer<T>&);
private:
	const int mSize;
	int mElementIdx = 0;
	std::unordered_map<T, int, Hasher> mReferenceCountingMap;

	std::vector<const T*> mCircularBuffer;
};

template<typename U>
std::ostream& operator<<(std::ostream& os, const FlyweightCircularBuffer<U>& cb)
{
	const int firstIdx = cb.mElementIdx % cb.mSize;
	int idx = firstIdx;
	do
	{
		os << cb.mCircularBuffer[idx] << ' ';
		idx =((idx+1)%cb.mSize);
	} while (idx != firstIdx);

	return os;
}

