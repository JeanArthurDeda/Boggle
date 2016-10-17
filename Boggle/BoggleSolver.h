#ifndef BOGGLE_SOLVER_H__
#define BOGGLE_SOLVER_H__

#include "Core/Types.h"
#include "BoogleDictionary.h"


// part of the agreed-upon interface
struct Results
{
	const char* const* Words;												// pointers to unique found words, each terminated by a non-alpha char
	unsigned           Count;												// number of words found
	unsigned           Score;												// total score
	void*              UserData;											// ignored
};

// ========================================
// Encapsulate a multi-thread boggle solver
// ========================================
class cBoggleSolver
{
public:
	cBoggleSolver();
	virtual ~cBoggleSolver();

	Results												Solve(cBoggleDictionary const &Dictionary, char const * const pTable, u32 const Width, u32 const Height, float const TimeOutInSeconds = 180.0f, u32 NumThreads = 4, bool const ForceSingleThread = false);

	void												Free(Results Result) const;

protected:
	volatile bool										m_TimeOut;
};

#endif // !BOGGLE_SOLVER_H__
