#include "CharTreeDictionary.h"
#include "../Core/Utils.h"

// ****************************************************************************

cCharTreeDictionary::cLeaf::~cLeaf()
{
	for (u8 i = 0; i < kNumLeafs; i++)
	{
		if (NULL == ppLeafs[i])
			continue;
		delete ppLeafs[i];
	}
}

// ****************************************************************************

cCharTreeDictionary::cCharTreeDictionary() :
	m_NumWords (0),
	m_MaxWordSize (0)
{
	memset (m_ppRoot, 0, sizeof (m_ppRoot));
}

// ****************************************************************************

cCharTreeDictionary::~cCharTreeDictionary()
{
	Empty ();
}

// ****************************************************************************

void cCharTreeDictionary::Empty()
{
	for (u8 i = 0; i < cLeaf::kNumLeafs; i++)
	{
		if (NULL == m_ppRoot[i])
			continue;
		delete m_ppRoot[i];
	}
	memset (m_ppRoot, 0, sizeof (m_ppRoot));
	m_MaxWordSize = 0;
	m_NumWords = 0;
}

// ****************************************************************************

u32 cCharTreeDictionary::Add(char const * const Word, u32 WordSize)
{
	ASSERT (Word, "Please consider adding a valid word.");
	
	if (0 == WordSize)
		WordSize = strlen (Word);

	if (WordSize > m_MaxWordSize)
		m_MaxWordSize = WordSize;

	u8 const c = Index (Word[0]);

	if (NULL == m_ppRoot[c])
	{
		m_ppRoot[c] = new cLeaf(1 == WordSize ? m_NumWords ++ : cLeaf::kEmptyId);
		if (1 == WordSize)
			return m_ppRoot[c]->WordId;
	}
	return AddRec (m_ppRoot[c]->ppLeafs, Word + 1, WordSize - 1);
}

// ****************************************************************************

u32 cCharTreeDictionary::AddRec(cLeaf::tLeafs &ppLeafs, char const * const String, u32 const StringSize)
{
	u8 const c = Index (String[0]);

	if (NULL == ppLeafs[c])
	{
		ppLeafs[c] = new cLeaf (1 == StringSize ? m_NumWords ++ : cLeaf::kEmptyId);
		if (1 == StringSize)
			return ppLeafs[c]->WordId;
	}

	if (1 == StringSize)
	{
		if (ppLeafs[c]->WordId == cLeaf::kEmptyId)
			ppLeafs[c]->WordId = m_NumWords ++;
		else
			int PlaceBreakPointOnDebugForWordDuplication = 0;
		return ppLeafs[c]->WordId;
	}
	return AddRec (ppLeafs[c]->ppLeafs, String + 1, StringSize - 1);

	return 0;
}

// ****************************************************************************

bool cCharTreeDictionary::Iterate(tWordIteratorCB WordCB, void *pUserData) const
{
	char * const String = reinterpret_cast<char*>(malloc (m_MaxWordSize + 1));

	for (u8 i = 0; i < cLeaf::kNumLeafs; ++i)
	{
		if (NULL == m_ppRoot[i])
			continue;

		String[0] = Char (i);
		String[1] = 0;

		if (cLeaf::kEmptyId != m_ppRoot[i]->WordId)
			if (false == WordCB (String, 1, m_ppRoot[i]->WordId, pUserData))
			{
				free (String);
				return false;
			}
		if (false == IterateRec (m_ppRoot[i]->ppLeafs, String + 1, 1, WordCB, pUserData))
		{
			free (String);
			return false;
		}
	}

	free (String);
	return true;
}

// ****************************************************************************

bool cCharTreeDictionary::IterateRec(cLeaf::tLeafs &ppLeafs, char * const String, u32 const StringSize, tWordIteratorCB WordCB, void * pUserData) const
{
	for (u8 i = 0; i < cLeaf::kNumLeafs; ++i)
	{
		if (NULL == ppLeafs[i])
			continue;
		String[0] = Char (i);
		String[1] = 0;
		if (cLeaf::kEmptyId != ppLeafs[i]->WordId)
			if (false == WordCB (String - StringSize, StringSize + 1, ppLeafs[i]->WordId, pUserData))
				return false;
		if (false == IterateRec (ppLeafs[i]->ppLeafs, String + 1, StringSize + 1, WordCB, pUserData))
			return false;
	}

	return true;
}

// ****************************************************************************
