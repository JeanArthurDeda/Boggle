#include "TestThreadPool.h"
#include "../Core/Types.h"
#include "../Core/Utils.h"
#include <windows.h>
#include <vector>
#include <time.h>

// ****************************************************************************

class cU32Vector : public std::vector<u32>
{
public:
	cU32Vector(u32 const GrowSize) : m_GrowSize(GrowSize) {}
	~cU32Vector() {};

	void							Push(u32 const Value)
	{
		if (size() == capacity())
			reserve(size() + m_GrowSize);
		push_back(Value);
	}

protected:
	cU32Vector() {}

	u32								m_GrowSize;
};

// ****************************************************************************

struct tThreadData
{
	cU32Vector*						pPool;
	u32								Size;
	u32								Id;
};

// ****************************************************************************

DWORD WINAPI Thread(LPVOID lpParam)
{
	tThreadData *pData = reinterpret_cast<tThreadData*>(lpParam);

	cU32Vector &Pool = *pData->pPool;
	u32 const Size = pData->Size;

	for (u32 i = 0; i < Size; i++)
		Pool.Push(i);

	return 0;
}

// ****************************************************************************

void DoThreadPoolTests()
{
	u32 const Size = 10 * 1024 * 1024;
	u32 const GrowSize = 400 * 1024;
	u32 const NumThreads = 4;

	printf("Size %d\n", Size);
	printf("GrowSize %d\n", GrowSize);
	printf("NumThreads %d\n", NumThreads);

	// Generate the Threads Data
	tThreadData Data[NumThreads];
	for (u32 i = 0; i < NumThreads; i++)
	{
		Data[i].pPool = new cU32Vector(GrowSize);
		Data[i].Size = Size;
		Data[i].Id = i;
	}

	// Kick Start the threads
	time_t begin;
	time(&begin);
	HANDLE Threads[NumThreads];
	for (u32 i = 0; i < NumThreads; ++i)
		Threads[i] = CreateThread(NULL, 0, Thread, (void*)&Data[i], 0, NULL);

	// Wait for the threads to finish
	WaitForMultipleObjects(NumThreads, Threads, TRUE, INFINITE);

	time_t now;
	time(&now);
	double seconds = difftime(now, begin);
	printf("Duration %f\n", seconds);


	// test the data
	for (u32 i = 0; i < NumThreads; ++i)
	{
		cU32Vector &Pool = *Data[i].pPool;
		for (u32 j = 0; j < Size; ++j)
			if (Pool[j] != j)
			{
				printf("Test Failed\n");
				BREAKPOINT_RELEASE;
			}
	}

	printf("Thread Pool Tests passed\n");
}

// ****************************************************************************