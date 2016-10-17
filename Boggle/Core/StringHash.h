#ifndef STRING_H__
#define STRING_H__

#include "Types.h"

// On english.txt it has collisions Rejected :(

class cStringHash
{
public:
	cStringHash();
	~cStringHash() {};

	static u32 const							kSeed = 0xffffffff;

	static u32									Get(char const * const String, u32 StringSize = 0, u32 const Seed = kSeed);

protected:
	static u32									s_Table[256];

	static cStringHash							s_Instance;

};

#endif // !STRING_H__
