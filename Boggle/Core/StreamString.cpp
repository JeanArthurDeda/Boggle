#include "StreamString.h"
#include "Utils.h"
#include <ctype.h>

// ****************************************************************************

cStreamString::cStreamString() :
	m_String(16 * 1024),
	m_F(NULL),
	m_FileSize(0),
	m_Index(0),
	m_BufferStart(0),
	m_BufferSize(0),
	m_Buffer(NULL),
	m_Valid (false)
{
	m_String.reserve(256);

	// Set the non alpha as separators
	memset(m_Separators, 0, sizeof(m_Separators));
	for (u32 i = 0; i < 256; ++i)
		if (isalnum(i) == 0)
			m_Separators[i >> 3] |= 1 << (i & 7);
}

// ****************************************************************************

cStreamString::cStreamString(FILE *f, u32 const FileSize, u32 const ReadBlockSize, u32 const StringGrowSize, char const * const Separators) :
	m_String(StringGrowSize),
	m_F(f),
	m_FileSize(FileSize),
	m_Index(0),
	m_BufferStart(0),
	m_BufferSize(ReadBlockSize),
	m_Buffer(NULL),
	m_Valid(false)
{
	ASSERT(m_F, "Consider passing a valid file to StreamString");
	ASSERT(m_BufferSize, "Consider passing a valid buffer size to StreamString");

	m_Buffer = reinterpret_cast<u8*>(malloc(m_BufferSize));

	// Read in the buffer
	fread(m_Buffer, m_FileSize > m_BufferSize ? m_BufferSize : m_FileSize, 1, f);

	if (NULL == Separators)
	{
		// Set the non alpha as separators
		memset(m_Separators, 0, sizeof(m_Separators));
		for (u32 i = 0; i < 256; ++i)
			if (isalnum(i) == 0)
				m_Separators[i >> 3] |= 1 << (i & 7);
	}
	else
	{
		memset(m_Separators, 0, sizeof(m_Separators));
		u32 const Size = static_cast<u32>(strlen(Separators));
		for (u32 i = 0; i < Size; i++)
		{
			u8 const c = Separators[i];
			m_Separators[c >> 3] |= 1 << (c & 7);
		}
	}

	m_Valid = (m_Separators[m_Buffer[0] >> 3] & (1 << (m_Buffer[0] & 7))) == 0;

	m_String.reserve(StringGrowSize);
}

// ****************************************************************************

cStreamString::~cStreamString()
{
	if (m_Buffer)
		free(m_Buffer);
}

// ****************************************************************************

char const * cStreamString::Read(u32 &OutSize, u32 &CurrFileSize)
{
	m_String.clear();

	// Check for eof
	if (m_Index + m_BufferStart >= m_FileSize)
	{
		OutSize = 0;
		CurrFileSize = m_FileSize;
		return NULL;
	}

	while (true)
	{
		char c = m_Buffer[m_Index];
		m_Index++;
		bool const Valid = (m_Separators[c >> 3] & (1 << (c & 7))) == 0;

		// Check for eof
		if (m_Index + m_BufferStart >= m_FileSize)
		{
			m_Valid = Valid;
			if (Valid)
				m_String.Push(c);
			m_String.Push(0);
			OutSize = static_cast<u32>(m_String.size() - 1);
			CurrFileSize = m_BufferStart + m_Index;
			if (OutSize == 0)
				return NULL;
			return m_String.Get();
		}

		// Check reading a new buffer
		if (m_Index >= m_BufferSize)
		{
			m_BufferStart += m_BufferSize;
			m_Index = 0;
			fread(m_Buffer, m_FileSize > m_BufferSize ? m_BufferSize : m_FileSize, 1, m_F);
		}

		// if state change and was alpha numeric return the string
		if ((m_Valid != Valid) && m_Valid)
		{
			m_Valid = Valid;
			m_String.Push(0);
			OutSize = static_cast<u32>(m_String.size()-1);
			CurrFileSize = m_BufferStart + m_Index;
			if (OutSize)
				return m_String.Get();
		}
		if (Valid)
			m_String.Push(c);
		m_Valid = Valid;
	}
}

// ****************************************************************************
