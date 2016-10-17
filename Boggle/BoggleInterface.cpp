#include <string.h>
#include "BoggleInterface.h"
#include "BoggleSolver.h"
#include "Core/StreamString.h"
#include "Core/Histogram.h"
#include "Core/ProgressBar.h"
#include <string>
#include <algorithm>

// ****************************************************************************

cBoggleDictionary Dictionary;
cBoggleSolver Solver;

// ****************************************************************************

void LoadDictionary(char const * path)
{
	FILE *f = NULL;
	fopen_s(&f, path, "rb");
	if (NULL == f)
	{
		printf("File `%s` not found.\n", path);
		return;
	}

	// find the size of the file
	fseek(f, 0, SEEK_END);
	u32 const FileSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	printf("Reading Dictionary `%s` FileSize %d\n", path, FileSize);


	u32 WordSize, CurFileSize;
	u32 ReadCount = 0;
	u32 NonAlphaOnlyCount = 0;
	u32 NonBiggerThan2Count = 0;
	u32 QWithoutQuCount = 0;
	std::string DupWord;
	DupWord.reserve(256);
	cStreamString Reader(f, FileSize, 10 * 1024, 256, "\r\n");


	cProgressBar ProgressBar;
	ProgressBar.Start();
	char const *Word = "Begin";
	while (Word)
	{
		Word = Reader.Read(WordSize, CurFileSize);
		if (!Word)
			continue;

		ReadCount++;

		// Bigger than 2
		if (WordSize < 3)
		{
			NonBiggerThan2Count ++;
			continue;
		}

		// Alpha character only
		bool AlphaOnly = true;
		for (u32 i = 0; i < WordSize; ++i)
			if (!isalpha(Word[i]))
			{
				AlphaOnly = false;
				break;
			}
		if (!AlphaOnly)
		{
			NonAlphaOnlyCount ++;
			continue;
		}

		// Should I increase the DupWord capacity
		if (WordSize > DupWord.capacity())
			DupWord.reserve(WordSize);


		// convert it to lower case
		DupWord.assign (Word, WordSize);
		std::transform(DupWord.begin(), DupWord.end(), DupWord.begin(), ::tolower);

		// does it have a q without a qu
		size_t PosQ  = DupWord.find ('q');
		bool QWithoutQU = false;
		while (PosQ != DupWord.npos)
		{
			if (PosQ == DupWord.size () - 1 ||
				DupWord[PosQ + 1] != 'u')
			{
				QWithoutQU = true;
				break;
			}
			PosQ  = DupWord.find ('q', PosQ + 1);
		}
		if (QWithoutQU)
		{
			QWithoutQuCount ++;
			continue;
		}

		// convert qu to q to :)
		size_t PosQu = DupWord.find ("qu");
		while (PosQu != DupWord.npos)
		{
			DupWord.erase (PosQu + 1, 1);
			PosQu = DupWord.find ("qu", PosQu + 1);	// squshy and squushy
		}

		// Add it
		Dictionary.AddWord(DupWord.c_str(), static_cast<u32>(DupWord.size ()));

		float const Ratio = static_cast<float>(CurFileSize) / static_cast<float>(FileSize);
		ProgressBar.Update(Ratio);

	}
	printf("Done ...\n");
	printf("Words Read Count                         %d\n", ReadCount);
	printf("Words Rejected with Non Alpha Characters %d\n", NonAlphaOnlyCount);
	printf("Words Rejected with Size Less than 3     %d\n", NonBiggerThan2Count);
	printf("Words Rejected due to q without qu       %d\n", QWithoutQuCount);
	printf("Total Rejected Words                     %d\n", QWithoutQuCount + NonAlphaOnlyCount + NonBiggerThan2Count);
	printf("Words added to Dictionary                %d\n", ReadCount - NonAlphaOnlyCount - NonBiggerThan2Count - QWithoutQuCount);
	Dictionary.ComputeStatistics ();

	fclose(f);
}

// ****************************************************************************

Results FindWords (const char* board, unsigned width, unsigned height)
{
	return Solver.Solve (Dictionary, board, width, height, 180.0f, 6);
//	return Solver.Solve (Dictionary, board, width, height, 180.0f, 1, true);
}

// ****************************************************************************

void FreeWords(Results Result) 
{
	Solver.Free (Result);
}

// ****************************************************************************

void FreeDictionary()
{
	Dictionary.Empty ();
}

// ****************************************************************************
