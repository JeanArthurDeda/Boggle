#include "BoggleSolver.h"
#include "Core/ProgressBar.h"
#include "Core/Utils.h"
#include <set>
#include <string.h>
#include <algorithm>
#include <windows.h>

#define RECORD_ITERATION_STATS								(0)


// ****************************************************************************

cBoggleSolver::cBoggleSolver() 
{
	m_TimeOut = false;
}

// ****************************************************************************

cBoggleSolver::~cBoggleSolver()
{
}

// ****************************************************************************

struct tThreadData
{
	typedef std::set<u32> tWordSet;

	// Id of the thread for debug purpose
	u32												Id;

	// Table
	struct tTable
	{
		char const *								pData;
		u32											Width;
		u32											Height;
	};
	tTable											Table;

	// Thread region
	struct tRegion
	{
		u32											X1;
		u32											Y1;
		u32											X2;
		u32											Y2;
	};
	tRegion											Region;

	// For not walking twice on the same square
	u8 *											pUsedBitArray;

	// Where to store the words
	u8 *											pWordUsedBitArray;

	// The dictionary
	cCharTreeDictionary const *						pDictionary;
#if RECORD_ITERATION_STATS
	struct tStats
	{
		typedef std::map<u32, u32> tSizeMap;
		void Inc(tSizeMap *Map, u32 const Size) const
		{
			tSizeMap::iterator it = Map->find (Size);
			if (Map->end() == it)
				Map->operator[](Size) = 1;
			else
				it->second++;
		}
		tSizeMap*									pSizeRejectedByDictonary;
		tSizeMap*									pSizeRejectedByMask;
	};
	tStats											Stats;
#endif // RECORD_ITERATION_STATS

	// Main and thread communication
	volatile bool *									pThreadDoNotWrite_IsTimeOut;
	volatile float									MainDoNotWrite_CompleteRatio;

	// Force Single thread
	bool											ForceSingleThread; // To display a progress bar while profiling multi thread results
};

// ****************************************************************************

bool ThreadWorker	(
						u32 const X, u32 const Y, tThreadData::tTable const &Table, u8 * const pUsedBitArray, 
						cCharTreeDictionary::tLeafs const &ppLeafs,
						u8 * const pWordUsedBitArray,
						volatile bool const &ThreadDoNotWrite_IsTimeOut
						#if RECORD_ITERATION_STATS
						,tThreadData::tStats &Stats
						#endif // RECORD_ITERATION_STATS
					)
{
	// Reject based on Time Out from Main Thread
	if (ThreadDoNotWrite_IsTimeOut)
		return false;

	// Get a character from the table
	char const c									= Table.pData[Y * Table.Width + X];
	// Rejection based on the dictionary
	u8 const Index = cCharTreeDictionary::Index (c);
	if (NULL == ppLeafs[Index])
	{
		#if RECORD_ITERATION_STATS
		Stats.Inc (Stats.pSizeRejectedByDictonary, It.GetStringSize ());
		#endif // RECORD_ITERATION_STATS
		return true;
	}

	// Reject based on the squared of the table being used in the same work
	if (pUsedBitArray[Y * ((Table.Width+7)>>3) + (X>>3)] & (1<<(X&7)))
	{
		#if RECORD_ITERATION_STATS
		Stats.Inc (Stats.pSizeRejectedByMask, It.GetStringSize ());
		#endif // RECORD_ITERATION_STATS
		return true;
	}



	// Mark it as used in this Work
	pUsedBitArray[Y * ((Table.Width+7)>>3) + (X>>3)] |= (1<<(X&7));

	// is it a full word
	cCharTreeDictionary::cLeaf const & Leaf = *ppLeafs[Index];
	if (Leaf.WordId != cCharTreeDictionary::cLeaf::kEmptyId)
		pWordUsedBitArray[Leaf.WordId >> 3] |= 1 << (Leaf.WordId & 7);


	// ============
	// Neightboars
	// ============
	i32 const Offset[3] = {-1, 0, 1};
	for (u32 i = 0; i < 3; ++i)
		for (u32 j = 0; j < 3; ++j)
		{
			// Reject the 0, 0 Offset
			if ((i == 1) && (j == 1))
				continue;

			i32 const Nx = X + Offset[j];
			i32 const Ny = Y + Offset[i];

			// reject based on the table margins
			if ((Nx < 0) || (Nx >= Table.Width))
				continue;
			// reject based on the table margins
			if ((Ny < 0) || (Ny >= Table.Height))
				continue;

			if (false == ThreadWorker 
						(Nx, Ny, Table, 
							pUsedBitArray, ppLeafs[Index]->ppLeafs, 
							pWordUsedBitArray, ThreadDoNotWrite_IsTimeOut
							#if RECORD_ITERATION_STATS
							,Stats
							#endif // RECORD_ITERATION_STATS
						)
				)
				return false; // Time Out
		}

	// Mark it as free in this Work
	pUsedBitArray[Y * ((Table.Width+7)>>3) + (X>>3)] &= ~(1<<(X&7));

	return true;
}

// ****************************************************************************

DWORD WINAPI BoggleThread(LPVOID lpParam)
{
	tThreadData *pData = reinterpret_cast<tThreadData*>(lpParam);

	// =======================
	// Extract the needed data
	// =======================
	// Id of the thread for debug purpose
	u32 const &Id									= pData->Id;
	// table
	tThreadData::tTable const &	Table				= pData->Table;
	// Thread region
	tThreadData::tRegion const & Region				= pData->Region;
	// Used Bit Array
	u8 * const &pUsedBitArray						= pData->pUsedBitArray;
	// Dictionary Iterator and create the Worker Context
	cCharTreeDictionary const & Dictionary			= *pData->pDictionary;
	#if RECORD_ITERATION_STATS
	tThreadData::tStats &Stats						= pData->Stats;
	#endif // RECORD_ITERATION_STATS
	// Out Word Vector
	u8* pWordUsedBitArray							= pData->pWordUsedBitArray;

	// TimeOut
	volatile bool const &ThreadDoNotWrite_IsTimeOut = *pData->pThreadDoNotWrite_IsTimeOut;
	// Complete Ratio
	volatile float & MainDoNotWrite_CompleteRatio	= pData->MainDoNotWrite_CompleteRatio;
	// ForceSingleThread
	bool const ForceSingleThread					= pData->ForceSingleThread;

	// =======================
	// Perform the preparation
	// =======================
	u32 const UsedBitArraySize = Table.Height * ((Table.Width + 7) >> 3);
	memset (pUsedBitArray, 0, UsedBitArraySize);
	memset (pWordUsedBitArray, 0, (Dictionary.GetNumWords () + 7) >> 3);

	// ======================
	// Distribute the workers
	// ======================
	// Linear spacial distribution which is not really good. A random distribution should find more words in a sudden death scenario.
	cProgressBar Bar;
	for (u32 Y = Region.Y1; Y < Region.Y2; Y ++)
		for (u32 X = Region.X1; X < Region.X2; ++X)
		{
			if (false == ThreadWorker 
								(X, Y, Table, 
									pUsedBitArray, Dictionary.GetRoot (), 
									pWordUsedBitArray, ThreadDoNotWrite_IsTimeOut
									#if RECORD_ITERATION_STATS
									,Stats
									#endif // RECORD_ITERATION_STATS
								)
				)
				return 0;

			// Inform the main thread about the completion ratio
			MainDoNotWrite_CompleteRatio = static_cast<float>((Y-Region.Y1) * (Region.X2 - Region.X1) + X - Region.X1) / static_cast<float>((Region.X2 - Region.X1) * (Region.Y2 - Region.Y1));
			if (ForceSingleThread)
				Bar.Update (MainDoNotWrite_CompleteRatio);
		}


	return 0;
}

// ****************************************************************************

struct tUserData
{
	u8 *										pWordUsedBitArray;
	u32											NumWords;
	char **										pOutWords;
	u32											Current;
	u32											Score;
	cProgressBar*								pProgress;
};

// This one iterates through all the words in the dictionary, does a cross reference with the found words and
// so it fills the string
bool DictionaryIterationCB(char const * const Word, u32 const WordSize, u32 const WordId, void *pUserData)
{
	tUserData *pData = reinterpret_cast<tUserData*>(pUserData);

	if (pData->pWordUsedBitArray[WordId >> 3] & (1<<(WordId&7)))
	{
		ASSERT (pData->Current < pData->NumWords, GENERIC_ASSERT_TEXT);
		// Copy the string and convert Q to Qu
		u32 NumQ = 0;
		for (u32 i = 0; i < WordSize; ++ i)
			if (Word[i] == 'q')
				NumQ ++;
		pData->pOutWords[pData->Current] = reinterpret_cast<char*>(malloc (WordSize + 1 + NumQ));
		u32 p = 0;
		for (u32 i = 0; i < WordSize; ++ i)
		{
			pData->pOutWords[pData->Current][p++] = Word[i];
			if (Word[i] == 'q')
				pData->pOutWords[pData->Current][p++] = 'u';
		}
		pData->pOutWords[pData->Current][p++] = 0;
		pData->Current ++;

		// Compute the scoring according to wiki page table
		// Hmmm I need the others players .... well :)
		u32 const RealWordSize = WordSize + NumQ;
		if (RealWordSize >= 8)
			pData->Score += 11;
		else
		{
			switch (RealWordSize)
			{
			case 7:
				pData->Score			+=5;
				break;
			case 6:
				pData->Score			+=3;
				break;
			case 5:
				pData->Score			+=2;
				break;
			case 4:
			case 3:
				pData->Score			+=1;
				break;
			}
		}

		float const Ratio = static_cast<float>(pData->Current) / static_cast<float>(pData->NumWords);
		pData->pProgress->Update (Ratio);

		if (pData->Current == pData->NumWords)
			return false; // No more iteration
		return true;
	}

	return true;
}

// ****************************************************************************

Results cBoggleSolver::Solve(cBoggleDictionary const &Dictionary, char const * const pTable, u32 const Width, u32 const Height, float const TimeOutInSeconds, u32 NumThreads, bool const ForceSingleThread)
{
	Results TheResults;
	memset(&TheResults, 0, sizeof(TheResults));

	ASSERT (NumThreads, GENERIC_ASSERT_TEXT);
	ASSERT (pTable, GENERIC_ASSERT_TEXT);

	// Test for small tables
	if (Width * Height < 3)
		return TheResults;

	if (Width < NumThreads && Height < NumThreads)
		NumThreads = 1; // No more Multi thread

	printf ("Boggle Solver for %dx%d Table\n", Width, Height);
	printf ("NumThreads %d\n", NumThreads);
	printf ("Time Out %5.2fs\n", TimeOutInSeconds);
	printf ("ForceSingleThread %s\n", ForceSingleThread ? "Yes" : "No");
	#if RECORD_ITERATION_STATS
	printf ("****** Record_Iteration_Stats ********\n");
	#endif // RECORD_ITERATION_STATS

	// ========================
	// Generate the Thread Data
	// ========================
	printf ("Generate Thread Data ...\n");
	u32 const WordBitArraySize = (Dictionary.GetNumWords () + 7) >> 3;
	tThreadData *ThreadData;
	ThreadData = new tThreadData[NumThreads];
	for (u32 i = 0; i < NumThreads; ++i)
	{
		memset (ThreadData+i, 0, sizeof (tThreadData));

		// Id of the thread for debug purpose
		ThreadData[i].Id								= i;

		// Table
		ThreadData[i].Table.pData						= pTable;
		ThreadData[i].Table.Width						= Width;
		ThreadData[i].Table.Height						= Height;

		// Thread region
		if (Height >= NumThreads)
		{
			ThreadData[i].Region.X1						= 0;
			ThreadData[i].Region.Y1						= i * Height / NumThreads;
			ThreadData[i].Region.X2						= Width;
			ThreadData[i].Region.Y2						= (i + 1) * Height / NumThreads;
		}
		else
		{
			ThreadData[i].Region.X1						= i * Width / NumThreads;
			ThreadData[i].Region.Y1						= 0;
			ThreadData[i].Region.X2						= (i + 1) * Width / NumThreads;
			ThreadData[i].Region.Y2						= Height;
		}

		// For not walking twice on the same square
		ThreadData[i].pUsedBitArray						= reinterpret_cast<u8*>(malloc (Height*((Width+7)>>3)));

		// Where to store the words
		ThreadData[i].pWordUsedBitArray					= reinterpret_cast<u8*>(malloc (WordBitArraySize));

		// The dictionary iterator
		ThreadData[i].pDictionary						= &Dictionary.GetDictionary();

		// stats
		#if RECORD_ITERATION_STATS
		ThreadData[i].Stats.pSizeRejectedByDictonary	= new tThreadData::tStats::tSizeMap ();
		ThreadData[i].Stats.pSizeRejectedByMask			= new tThreadData::tStats::tSizeMap ();
		#endif // RECORD_ITERATION_STATS

		

		// Main and thread communication
		ThreadData[i].pThreadDoNotWrite_IsTimeOut		= &m_TimeOut;
		ThreadData[i].MainDoNotWrite_CompleteRatio		= 0.0; // Now it's safe :)

		// ForceSingleThread
		ThreadData[i].ForceSingleThread					= ForceSingleThread;
	}

	HANDLE *Handles = NULL;
	if (!ForceSingleThread)
	{
		// ====================
		// Kick Out the Threads
		// ====================
		Handles = new HANDLE [NumThreads];
		printf ("Start them Threads\n");
		for (u32 i = 0; i < NumThreads; ++i)
		{
			Handles[i] = CreateThread(NULL, 0, BoggleThread, (void*)&ThreadData[i], 0, NULL);
			ASSERT (Handles[i], "Unable to create thread");
		}

		// ================
		// Wait for threads
		// ================
		printf ("Wait for the Threads to finish\n");
		bool Busy = true;
		cProgressBar ProgressBar;
		time_t Start;
		time (&Start);
		while (Busy)
		{
			if (WAIT_OBJECT_0 == WaitForMultipleObjects (NumThreads, Handles, TRUE, 100))
				Busy = false;
			float Sum = 0;
			for (u32 i = 0; i < NumThreads; ++i)
				Sum += ThreadData[i].MainDoNotWrite_CompleteRatio;
			Sum /= static_cast<float>(NumThreads);
			ProgressBar.Update (Sum);

			// Time out check
			time_t Now;
			time (&Now);
			double S = difftime (Now, Start);
			if (S > TimeOutInSeconds)
				m_TimeOut = true;				// This will send the TimeOut Signal to all the threads
		}
		ProgressBar.Update (1.0, 0.0f);
		if (m_TimeOut)
			printf ("Time out was signaled\n");
		m_TimeOut = false;						// Reset the time out
	}
	else
	{
		for (u32 i = 0; i < NumThreads; ++i)
		{
			printf ("SingleCore Job %d out of %d\n", i, NumThreads);
			BoggleThread ((void*)&ThreadData[i]);
		}
	}


	// ==================
	// Gather Thread Data
	// ==================
	printf ("Done ... now relax, let's have a coke, gather the data ..\n");
	u32 NumWords = 0;
	for (u32 i = 1; i < NumThreads; ++i)
	{
		for (u32 j = 0; j < WordBitArraySize; ++j)
			ThreadData[0].pWordUsedBitArray[j] |= ThreadData[i].pWordUsedBitArray[j];
	}
	for (u32 i = 0; i < Dictionary.GetNumWords (); ++i)
		if (ThreadData[0].pWordUsedBitArray[i >> 3] & (1<<(i&7)))
			NumWords ++;
	// Lets fill the results strings
	char **Words = reinterpret_cast<char**>(malloc (NumWords * sizeof (char*)));
	memset (Words, 0, NumWords * sizeof (char*));
	tUserData UserData;
	UserData.Current = 0;
	UserData.pOutWords = Words;
	UserData.pWordUsedBitArray = ThreadData[0].pWordUsedBitArray;
	UserData.NumWords = NumWords;
	UserData.Score = 0;
	cProgressBar GatherBar;
	UserData.pProgress = &GatherBar;
	GatherBar.Start ();
	Dictionary.GetDictionary ().Iterate (DictionaryIterationCB, &UserData);
	GatherBar.Update (1.0, 0.0);
	ASSERT (UserData.Current == NumWords, GENERIC_ASSERT_TEXT);

	#if RECORD_ITERATION_STATS
	// colapse all the stats
	tThreadData::tStats::tSizeMap SizeRejectedByDictonary;
	tThreadData::tStats::tSizeMap SizeRejectedByMask;
	for (u32 i = 0; i < NumThreads; ++i)
	{
		SizeRejectedByDictonary.insert (ThreadData[i].Stats.pSizeRejectedByDictonary->begin (), ThreadData[i].Stats.pSizeRejectedByDictonary->end ());
		SizeRejectedByMask.insert      (ThreadData[i].Stats.pSizeRejectedByMask->begin (),      ThreadData[i].Stats.pSizeRejectedByMask->end ());
	}
	u32 Total = 0;
	tThreadData::tStats::tSizeMap::iterator it;
	printf ("Rejection by Dictionary\n");
	printf (" Size      Count\n");
	for (it = SizeRejectedByDictonary.begin(); it != SizeRejectedByDictonary.end(); it++)
	{
		printf ("%5d %10d\n", it->first, it->second);
		Total += it->second;
	}
	printf ("Total %5d\n\n", Total);

	Total = 0;
	printf ("Rejection by Mask\n");
	printf (" Size      Count\n");
	for (it = SizeRejectedByMask.begin(); it != SizeRejectedByMask.end(); it++)
	{
		printf ("%5d %10d\n", it->first, it->second);
		Total += it->second;
	}
	printf ("Total %5d\n\n", Total);

	#endif // RECORD_ITERATION_STATS


	// =======================
	// Release the thread data
	// =======================
	for (u32 i = 0; i < NumThreads; ++i)
	{
		// For not walking twice on the same square
		free (ThreadData[i].pUsedBitArray);

		// Where to store the words
		free (ThreadData[i].pWordUsedBitArray);

		// stats
		#if RECORD_ITERATION_STATS
		delete ThreadData[i].Stats.pSizeRejectedByDictonary	;
		delete ThreadData[i].Stats.pSizeRejectedByMask		;
		#endif // RECORD_ITERATION_STATS


		// Handle
		if (false == ForceSingleThread)
			CloseHandle (Handles[i]);
	}
	delete[] ThreadData;
	if (Handles)
		delete[] Handles;

	// ===================
	// Fill up The Results
	// ===================
	TheResults.Count = NumWords;
	TheResults.Words = UserData.pOutWords;
	TheResults.Score = UserData.Score;

	return TheResults;
}

// ****************************************************************************

void cBoggleSolver::Free(Results Result) const
{
	for (u32 i = 0; i < Result.Count; i ++)
	{
		free(const_cast<char*>(Result.Words[i]));
	}
	free(const_cast<char**>(Result.Words));
}

// ****************************************************************************
