#pragma once

#ifndef STREAM_STRING_H__
#define STREAM_STRING_H__

#include "Types.h"
#include <vector>

// A fast class that allows to stream directly into a growing string for when you need to read strings
// from files and you have no idea what's the max string.

class cStreamString
{
public:
	// Separators = NULL means all non alpha numeric
	cStreamString(FILE *f, u32 const FileSize, u32 const ReadBlockSize, u32 const StringGrowSize = 16*1024, char const *Separators = NULL);
	~cStreamString();

	char const * Read (u32 &OutSize, u32 &CurrFileSize);

protected:
	cStreamString();

	// Based on the C++98 standard
	// 23.2.6 Class template vector[vector] ....  
	// The elements of a vector are stored contiguously, meaning that if v is a vector where T is some type other than bool, then it obeys the identity &v[n] == &v[0] + n for all 0 <= n < v.size().
	class cGrowingString : public std::vector<char>
	{
	public:
		cGrowingString() : m_Grow(16 * 1024) {}
		cGrowingString(u32 const Grow) : m_Grow(Grow) {}
		~cGrowingString() {}

		inline void Push(char const c)
		{
			if (size() + 1 == capacity())
				reserve(size() + m_Grow);
			push_back(c);
		}

		inline char const * Get() const { return &operator[](0); }

	protected:
		u32								m_Grow;
	};


	cGrowingString						m_String;
	FILE								*m_F;
	u32									m_Index;
	u32									m_BufferStart;
	u32									m_FileSize;
	u32									m_BufferSize;
	u8 *								m_Buffer;

	bool								m_Valid;
	u8									m_Separators[32];
};

#endif // !STREAM_STRING_H__

