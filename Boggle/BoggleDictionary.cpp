#include "BoogleDictionary.h"
#include "Core/Utils.h"
#include <ctype.h>

// ****************************************************************************

cBoggleDictionary::cBoggleDictionary() :
	m_MaxConsonants(0),
	m_MaxVowels(0),
	m_MaxConsConsonants(0),
	m_MaxConsVowels(0),
	m_80PercMaxWordSize(0),
	m_95PercMaxWordSize(0)
{
}

// ****************************************************************************

cBoggleDictionary::~cBoggleDictionary()
{
}

// ****************************************************************************

void cBoggleDictionary::AddWord (char const * const Word, u32 WordSize)
{
	ASSERT (Word, "Consider adding a non NULL word");
	if (0 == WordSize)
		WordSize = static_cast<u32>(strlen (Word));
	ASSERT (WordSize, "Consider Adding a real word");

	m_Dictionary.Add (Word, WordSize);
	m_Histogram.Add (WordSize);

	u32 NumVowels				= 0;
	u32 NumConsonants			= 0;
	u32 NumConscVowels			= 0;
	u32 NumConscConsonants		= 0;
	for (u32 i = 0; i < WordSize; ++i)
	{
		char const c			= Word[i];
		ASSERT (isalpha (c) && islower (c), "Word is not lower case or it has non alpha characters");

		bool const IsVowel		= (c=='a') || (c=='e') || (c=='i') || (c=='o') || (c=='u') || (c=='y');

		if (IsVowel)
		{
			NumVowels ++;
			NumConscVowels ++;
			NumConscConsonants = 0;
		}
		else
		{
			NumConsonants ++;
			NumConscConsonants ++;
			NumConscVowels = 0;
		}
		if (NumConscVowels > m_MaxConsVowels)
			m_MaxConsVowels = NumConscVowels;
		if (NumConscConsonants > m_MaxConsConsonants)
			m_MaxConsConsonants = NumConscConsonants;

	}

	if (NumVowels > m_MaxVowels)
		m_MaxVowels = NumVowels;
	if (NumConsonants > m_MaxConsonants)
		m_MaxConsonants = NumConsonants;
}

// ****************************************************************************

void cBoggleDictionary::ComputeStatistics()
{
	u32 Sum = 0;

	m_80PercMaxWordSize = 0;
	m_95PercMaxWordSize = 0;

	u32 const MaxNumAsterisks = 60;
	m_Histogram.ComputeCalculus();
	printf ("Compute Dictionary Statistics...\n");
	printf("Dictionary Words Size Histogram\n");
	for (u32 i = m_Histogram.GetRangeMin(); i <= m_Histogram.GetRangeMax(); i++)
	{
		u32 const Count = m_Histogram.Get(i);
		Sum += Count;

		if (static_cast<double>(Sum) <= 0.8 * static_cast<double>(m_Dictionary.GetNumWords()))
			m_80PercMaxWordSize = i;
		if (static_cast<double>(Sum) <= 0.9 * static_cast<double>(m_Dictionary.GetNumWords()))
			m_95PercMaxWordSize = i;

		u32 const NumAsterisks = MaxNumAsterisks * Count / m_Histogram.GetCountMax();
		printf("%02d - %08d ", i, Count);
		for (u32 j = 0; j < NumAsterisks; ++j)
			printf("*");
		// Is it local Min
		for (u32 j = 0; j < m_Histogram.GetNumLocalMins(); ++j)
			if (i == m_Histogram.GetLocalMin(j))
				printf("[-]");
		for (u32 j = 0; j < m_Histogram.GetNumLocalMaxs(); ++j)
			if (i == m_Histogram.GetLocalMax(j))
				printf("[+]");
		printf("\n");
	}
	printf ("Num Words in Dictionary                %d\n", GetNumWords ());
	printf ("Words Max Size                         %d\n", GetMaxWordSize());
	printf ("80%% of words have a size shorter than  %d\n", Get80PercMaxWordSize () + 1);
	printf ("95%% of words have a size shorter than  %d\n", Get95PercMaxWordSize () + 1);
	printf ("Max Vowels in a Word                   %d\n", GetMaxVowels ());
	printf ("Max Consonants in a Word               %d\n", GetMaxConsonats ());
	printf ("Max Consecutives Vowels in a Word      %d\n", GetMaxConscVowels ());
	printf ("Max Consecutives Consonants in a Word  %d\n", GetMaxConscConsonats ());

	if (Get80PercMaxWordSize () < GetMaxWordSize () / 2)
		printf ("It's a fine looking dictionary ...\n");
}

// ****************************************************************************

void cBoggleDictionary::Empty()
{
	m_Dictionary.Empty ();
	m_Histogram.Empty ();
	m_MaxConsonants = 0;
	m_MaxVowels = 0;
	m_MaxConsConsonants = 0;
	m_MaxConsVowels = 0;
	m_80PercMaxWordSize = 0;
	m_95PercMaxWordSize = 0;
}

// ****************************************************************************

