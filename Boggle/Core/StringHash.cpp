#include "StringHash.h"
#include "Utils.h"
#include <string.h>

// ****************************************************************************

// based on https://github.com/damieng/DamienGKit/blob/master/CSharp/DamienG.Library/Security/Cryptography/Crc32.cs

cStringHash cStringHash::s_Instance;
u32 cStringHash::s_Table[256];

// ****************************************************************************

cStringHash::cStringHash()
{
	// Generate the table
	u32 const Polynomial = 0xedb88320u;

	for (u32 i = 0; i < 256; i++)
	{
		u32 Entry = i;
		for (u32 j = 0; j < 8; j++)
			if ((Entry & 1) == 1)
				Entry = (Entry >> 1) ^ Polynomial;
			else
				Entry = Entry >> 1;
		s_Table[i] = Entry;
	}
}

// ****************************************************************************

u32 cStringHash::Get(char const * const String, u32 StringSize, u32 const Seed)
{
	if (0 == StringSize)
		StringSize = static_cast<u32>(strlen(String));
	u32 Hash = Seed;
	for (u32 i = 0; i < StringSize; i++)
		Hash = (Hash >> 8) ^ s_Table[String[i] ^ Hash & 0xff];
	return ~Hash;
}

// ****************************************************************************

