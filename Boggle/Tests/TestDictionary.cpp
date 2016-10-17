#include "TestDictonary.h"
#include "../Dictionary/CharTreeDictionary.h"
#include "../Core/ProgressBar.h"
#include "../Core/Utils.h"
#include <vector>


// ****************************************************************************

std::vector<char const *>	Words;

// ****************************************************************************

bool FindWord(char const * const Word, u32 const WordSize, u32 const WordId, void *pUserData)
{
	for (u32 i = 0; i < Words.size(); ++i)
		if (0 == strcmp(Word, Words[i]))
			return true;
	ASSERT_RELEASE(false, "Word was not found!!!!");
	return false;
}

// ****************************************************************************

void DoDictionaryTests()
{
	u32 const NumTests = 32;
	u32 const MaxSize = 200000;
	u32 const MaxWordSize = 40;

	// set the random seed
	srand(0);

	for (u32 TestCount = 0; TestCount < NumTests; ++TestCount)
	{
		cCharTreeDictionary Dictionary;
		u32 const Size = (rand() % MaxSize) + 1;


		printf("Test: %d\n", TestCount);
		printf("\tSize of Dictionary:\t\t%d\n", Size);
		printf("\tSize of Max Word:\t\t%d\n", MaxWordSize);

		Words.reserve(Size);

		cProgressBar ProgressBar;
		ProgressBar.Start();
		// Generate the words for the test
		u32 DupCount = 0;
		for (u32 i = 0; i < Size; ++i)
		{
			u32 const WordSize	= (rand() % MaxWordSize) + 1;
			char * const Word	= reinterpret_cast<char*>(malloc(WordSize + 1));
			for (u32 j = 0; j < WordSize; ++j)
			{
				char const c = (rand() % ('z' - 'a')) + 'a';
				Word[j] = c;
			}


			Word[WordSize] = 0;
			Words.push_back(Word);
			Dictionary.Add(Word, WordSize);

			ProgressBar.Update(static_cast<float>(i) / static_cast<float>(Size));
		}

		ASSERT_RELEASE(Dictionary.GetNumWords() <= Words.size(), "Error With Num Words - Test Failed");
		if (Dictionary.GetNumWords() != Words.size())
		{
			printf("\tWordsCount %d DictionaryWordsCount %d\n", static_cast<u32>(Words.size()), Dictionary.GetNumWords());
			printf("\tDuplicated words %d %d\n", static_cast<u32>(Words.size()) - Dictionary.GetNumWords(), DupCount);
		}
		Dictionary.Iterate(FindWord);

		// Empty the generate words
		for (u32 i = 0; i < Size; ++i)
			free(const_cast<char*>(Words[i]));
		Words.clear();

	}
	printf("Dictionary Test Completed\n");
}

// ****************************************************************************